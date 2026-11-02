#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) ..
    arg[0]:gsub("[^/\\]+%.lua", '../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://learnopencv.com/deep-learning-based-object-detection-using-yolov3-with-opencv-python-c/
    https://github.com/sjinzh/awesome-yolo-object-detection
    https://github.com/ultralytics/yolov5
    https://github.com/ultralytics/yolov5/issues/251
    https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
    https://github.com/ultralytics/ultralytics/blob/main/examples/YOLOv8-CPP-Inference/inference.cpp
    https://docs.opencv.org/4.x/d4/d2f/tf_det_tutorial_dnn_conversion.html
    https://github.com/opencv/opencv/blob/5.0.0/samples/dnn/object_detection.py
--]]

local __file__ = arg[0]

local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local INDEX_BASE = 1 -- lua is 1-based indexed

local argparse = require("argparse")
local opencv_lua = require("opencv_lua")
local common = require("common")
local sysPath = common.path

local __dirname__ = sysPath.dirname(sysPath.abspath(__file__))

local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local int = function(val) return opencv_lua.math.int(tonumber(val)) end
local float = tonumber
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local findFile = common.findFile
local findModel = common.findModel
local get_backend_id = common.get_backend_id
local get_target_id = common.get_target_id

local function help()
    print(
        [[
        Firstly, download required models using `download_models.py` (if not already done). Set environment variable OPENCV_DOWNLOAD_CACHE_DIR to specify where models should be downloaded. Also, point OPENCV_SAMPLES_DATA_PATH to opencv/samples/data.\n"\n

        To run:
            lua object_detection.lua model_name(e.g yolov8) --input=path/to/your/input/image/or/video (don't pass --input to use device camera)

        Sample command:
            lua object_detection.lua yolov8 --input=path/to/image
        Model path can also be specified using --model argument
        ]]
    )
end

local default_models = sysPath.join(__dirname__, 'models.yml')
local backends = { "default", "openvino", "opencv", "vkcom", "cuda", "webnn" }
local targets = { "cpu", "opencl", "opencl_fp16", "ncs2_vpu", "hddl_vpu", "vulkan", "cuda", "cuda_fp16" }

local parser = argparse() {
    name = sysPath.basename(__file__),
    description = "Use this script to run object detection deep learning networks using OpenCV."
}

parser:option("--zoo", "An optional path to file with preprocessing parameters.", default_models)
parser:option('--input', 'Path to input image or video file. Skip this argument to capture frames from a camera.')
parser:option('--out_tf_graph'):default('graph.pbtxt')
    :description('For models from TensorFlow Object Detection API, you may ' ..
        'pass a .config file which was used for training through --config ' ..
        'argument. This way an additional .pbtxt file with TensorFlow graph will be created.')
parser:option('--thr'):convert(float):default(0.5):description('Confidence threshold')
parser:option('--nms'):convert(float):default(0.4):description('Non-maximum suppression threshold')
parser:option('--backend'):choices(backends):default("default")
    :description("Choose one of computation backends: " ..
        "default: automatically (by default), " ..
        "openvino: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), " ..
        "opencv: OpenCV implementation, " ..
        "vkcom: VKCOM, " ..
        "cuda: CUDA, " ..
        "webnn: WebNN")
parser:option('--target'):choices(targets):default("cpu")
    :description("Choose one of target computation devices: " ..
        "cpu: CPU target (by default), " ..
        "opencl: OpenCL, " ..
        "opencl_fp16: OpenCL fp16 (half-float precision), " ..
        "ncs2_vpu: NCS2 VPU, " ..
        "hddl_vpu: HDDL VPU, " ..
        "vulkan: Vulkan, " ..
        "cuda: CUDA, " ..
        "cuda_fp16: CUDA fp16 (half-float preprocess)")
parser:option('--async'):convert(int):default(cv.getNumThreads())
    :description('Number of asynchronous forwards at the same time. ' ..
        'Choose 0 for synchronous mode')
parser:option('--fps'):convert(int):default(0):description('Input frames per second')
parser:option('-H --dheight'):convert(int):default(0):description('Displayed height')
parser:option('-W --dwidth'):convert(int):default(0):description('Displayed width')
parser:flag('--skip'):description('Skip frames to honor input fps')

local default_cmdline = rawget(_G, "arg") or {}

local known_args = {}

; (function()
    local i = 2

    while i <= #default_cmdline do
        if default_cmdline[i] == "--zoo" then
            known_args[#known_args + 1] = default_cmdline[i]
            if i + 1 <= #default_cmdline then
                i = i + 1
                known_args[#known_args + 1] = default_cmdline[i]
            end
        elseif default_cmdline[i]:sub(1, #"--zoo=") == "--zoo=" then
            known_args[#known_args + 1] = default_cmdline[i]
        end

        i = i + 1
    end
end)()

known_args = parser:parse(known_args)

common.add_preproc_args(known_args.zoo, parser, 'object_detection')

local args = parser:parse()

if args.alias == nil then
    help()
    os.exit(1)
end

local MODELS_PATH = os.getenv('MODELS_PATH') or sysPath.join(__dirname__, 'models')

local OPENCV_SAMPLES_DATA_PATH = opencv_lua.env.OPENCV_SAMPLES_DATA_PATH

local PYTHON_VENV_PATH = os.getenv('PYTHON_VENV_PATH') or
    sysPath.join(opencv_lua.fs_utils.findFile("samples", opencv_lua.kwargs({
        directory = __dirname__
    })), ".venv")
local PYTHONPATH = sysPath.join(sysPath.dirname(OPENCV_SAMPLES_DATA_PATH), "dnn")

if package.config:sub(1, 1) == '\\' then
    -- assume windows
    local code = common.execute({
        "powershell.exe", "-ExecutionPolicy", "UnRestricted",
        "-File", sysPath.join(__dirname__, 'download_model.ps1'),
        "-Model", args.alias,
        "-Venv", PYTHON_VENV_PATH,
        "-Zoo", args.zoo,
        "-Destination", MODELS_PATH,
        "-PythonPath", PYTHONPATH,
    })
    if code ~= 0 then os.exit(code) end
else
    -- assume linux
    local code = common.execute({
        "bash", sysPath.join(__dirname__, 'download_model.sh'),
        PYTHON_VENV_PATH,
        PYTHONPATH,
        args.alias,
        args.zoo,
        MODELS_PATH, sysPath.join(__dirname__, 'download_model.py'),
    })
    if code ~= 0 then os.exit(code) end
end

cv.samples.addSamplesDataSearchPath(MODELS_PATH)
cv.samples.addSamplesDataSearchPath(__dirname__)
cv.samples.addSamplesDataSearchPath(sysPath.join(OPENCV_SAMPLES_DATA_PATH, "dnn"))

cv.utils.logging.setLogLevel(cv.utils.logging.LOG_LEVEL_INFO)
args.model = findModel(args.model, args.sha1)
if args.config ~= nil then
    args.config = findModel(args.config, args.config_sha1)
end
if args.labels ~= nil then
    args.labels = findFile(args.labels)
end

-- shared variables
local capture_counter = 0
local network_counter = 0
local tick_init = 0
local fps = common.FPS(60)
local frame = (function() if args.target == "cpu" then return cv.Mat() else return cv.UMat() end end)()
local blob = frame.new()
local input_fps = args.fps
local future_outputs
local winName = 'Deep learning object detection in OpenCV'

-- Load names of classes
local labels
if args.labels then
    local content = common.read_file(args.labels)
    labels = {}
    for str in content:gmatch("([^\r\n]+)") do
        labels[#labels + 1] = str
    end
end

-- Load a network
local engine = cv.dnn.ENGINE_AUTO
if args.backend ~= "default" or args.target ~= "cpu" then
    engine = cv.dnn.ENGINE_CLASSIC
end
local net = cv.dnn.readNet(args.model, args.config, "", engine)
net:setPreferableBackend(get_backend_id(args.backend))
net:setPreferableTarget(get_target_id(args.target))
net:setProfilingMode(cv.dnn.DNN_PROFILE_SUMMARY)
local outNames = net:getUnconnectedOutLayersNames()

local confThreshold = args.thr
local nmsThreshold = args.nms
local stdSize = 0.8
local stdWeight = 2
local stdImgSize = 512

local function get_color(class_id)
    local r = math.min(
        bit.band(bit.arshift(class_id, 0), 1) * 128 + bit.band(bit.arshift(class_id, 3), 1) * 64 +
        bit.band(bit.arshift(class_id, 6), 1) * 32 + 80, 255)
    local g = math.min(
        bit.band(bit.arshift(class_id, 1), 1) * 128 + bit.band(bit.arshift(class_id, 4), 1) * 64 +
        bit.band(bit.arshift(class_id, 7), 1) * 32 + 40, 255)
    local b = math.min(
        bit.band(bit.arshift(class_id, 2), 1) * 128 + bit.band(bit.arshift(class_id, 5), 1) * 64 +
        bit.band(bit.arshift(class_id, 8), 1) * 32 + 40, 255)
    return { b, g, r }
end

local function get_text_color(bg_color)
    local luminance = 0.299 * bg_color[2 + INDEX_BASE] + 0.587 * bg_color[1 + INDEX_BASE] +
        0.114 * bg_color[0 + INDEX_BASE]

    return (function() if luminance > 128 then return { 0, 0, 0 } else return { 255, 255, 255 } end end)()
end

local UNSUPPORTED_YOLO_VERSION = 'Unsupported yolo version. Supported versions are v3, v4, v5, v6, v7, v8.'

local DESIRED_HEIGHT = args.dheight
local DESIRED_WIDTH = args.dwidth


local function resize_and_show(title, image)
    local h, w = image.height, image.width

    if h < w then
        h = math.floor(h / (w / DESIRED_WIDTH))
        w = DESIRED_WIDTH
    else
        w = math.floor(w / (h / DESIRED_HEIGHT))
        h = DESIRED_HEIGHT
    end

    local width, height = image.width, image.height
    local interpolation = (DESIRED_WIDTH > width or DESIRED_HEIGHT > height) and cv.INTER_CUBIC or cv.INTER_AREA
    local img = cv.resize(image, { w, h }, kwargs({ interpolation = interpolation }))
    cv.imshow(title, img)
    return img.width / image.width
end


local function drawPred(frame, classIds, confidences, boxes, indices, fontSize, fontThickness)
    for _, i in ipairs(indices) do
        local box = boxes[i + INDEX_BASE]
        local left = box[0 + INDEX_BASE]
        local top = box[1 + INDEX_BASE]
        local right = left + box[2 + INDEX_BASE]
        local bottom = top + box[3 + INDEX_BASE]

        local bg_color = get_color(classIds[i + INDEX_BASE])

        -- Draw a bounding box.
        cv.rectangle(frame, { left, top }, { right, bottom }, bg_color, fontThickness)

        local label = string.format('%.2f', confidences[i + INDEX_BASE])

        -- Print a label of class.
        if labels then
            assert(classIds[i + INDEX_BASE] < #labels)
            label = string.format('%s: %s', labels[classIds[i + INDEX_BASE] + INDEX_BASE], label)
        end

        local labelSize, baseLine = cv.getTextSize(label, cv.FONT_HERSHEY_SIMPLEX, fontSize, fontThickness)
        top = math.max(top, labelSize[1 + INDEX_BASE])
        cv.rectangle(frame, { int(left - fontThickness / 2), top - labelSize[1 + INDEX_BASE] },
            { left + labelSize[0 + INDEX_BASE], top + baseLine }, bg_color, cv.FILLED)
        cv.putText(frame, label, { left, top - fontThickness }, cv.FONT_HERSHEY_SIMPLEX, fontSize,
            get_text_color(bg_color), fontThickness)
    end
end


local function postprocess_yolo(box_scale_w, box_scale_h, out, classIds, confidences, boxes)
    for _, detection in pairs(out) do
        local scores, obj_conf

        if args.postprocessing == 'yolov8' then
            scores = common.slice(detection, 4 + INDEX_BASE)
            obj_conf = 1
        else
            scores = common.slice(detection, 5 + INDEX_BASE)
            obj_conf = detection[4 + INDEX_BASE]
        end

        local classId = common.argmax(scores) - INDEX_BASE
        local confidence = scores[classId + INDEX_BASE] * obj_conf
        if confidence > confThreshold then
            local center_x = detection[0 + INDEX_BASE] * box_scale_w
            local center_y = detection[1 + INDEX_BASE] * box_scale_h
            local width = int(detection[2 + INDEX_BASE] * box_scale_w)
            local height = int(detection[3 + INDEX_BASE] * box_scale_h)
            local left = int(center_x - width / 2)
            local top = int(center_y - height / 2)

            classIds[#classIds + 1] = classId
            confidences[#confidences + 1] = confidence
            boxes[#boxes + 1] = { left, top, width, height }
        end
    end
end


local function postprocess(frame, imgScale, inpWidth, inpHeight, outs)
    local startTime = os.clock()

    local classIds = {}
    local confidences = {}
    local boxes = {}

    local box_scale_w, box_scale_h

    if args.postprocessing == 'ssd' then
        -- Network produces output blob with a shape 1x1xNx7 where N is a number of
        -- detections and an every detection is a vector of values
        -- [batchId, classId, confidence, left, top, right, bottom]
        for i = 1, #outs do
            local out = outs[i]:table()
            for _, detection in pairs(out[0 + INDEX_BASE][0 + INDEX_BASE]) do
                if detection[5 + INDEX_BASE] - detection[3 + INDEX_BASE] < 1 then
                    -- relative coordinates
                    box_scale_w = inpWidth * imgScale
                    box_scale_h = inpHeight * imgScale
                else
                    -- absolute coordinate
                    box_scale_w = imgScale
                    box_scale_h = imgScale
                end

                local confidence = detection[2 + INDEX_BASE]
                if confidence > confThreshold then
                    local left = int(detection[3 + INDEX_BASE] * box_scale_w)
                    local top = int(detection[4 + INDEX_BASE] * box_scale_h)
                    local width = int(detection[5 + INDEX_BASE] * box_scale_w - left + 1)
                    local height = int(detection[6 + INDEX_BASE] * box_scale_h - top + 1)

                    local classId = int(detection[1 + INDEX_BASE])
                    local skip_background_label = 0 <= args.background_label_id and args.background_label_id <= classId

                    classIds[#classIds + 1] = (function() if skip_background_label then return classId - 1 else return classId end end)()
                    confidences[#confidences + 1] = float(confidence)
                    boxes[#boxes + 1] = { left, top, width, height }
                end
            end
        end
    elseif args.postprocessing == 'yolov4' or args.postprocessing == 'yolov3' then
        -- # boxes[b,N,1,4]+confs[b,N,classes] (normalized) or boxes[b,N,4]+scores[b,N]+classIdx[b,N] (model-px)
        if #outs == 3 and outs[0 + INDEX_BASE].dims == 3 and outs[0 + INDEX_BASE].sizes[2 + INDEX_BASE] == 4 then
            -- absolute coordinate
            box_scale_w = imgScale
            box_scale_h = imgScale

            local boxesIndex = 1
            local scoresIndex = 2
            local classIdxIndex = 3

            for i, outName in ipairs(outNames) do
                if outName == "boxes" then
                    boxesIndex = i
                elseif outName == "scores" then
                    scoresIndex = i
                elseif outName == "class_idx" then
                    classIdxIndex = i
                else
                    error('Unsupported outLayerName: ' .. outName)
                end
            end

            local boxesArr = outs[boxesIndex]:table()[0 + INDEX_BASE]
            local scoresArr = outs[scoresIndex]:table()[0 + INDEX_BASE]
            local classIdxArr = outs[classIdxIndex]:table()[0 + INDEX_BASE]

            for j = 1, #boxesArr do
                local score = float(scoresArr[j])
                if score > confThreshold then
                    local x1 = boxesArr[j][0 + INDEX_BASE] * box_scale_w
                    local y1 = boxesArr[j][1 + INDEX_BASE] * box_scale_h
                    local x2 = boxesArr[j][2 + INDEX_BASE] * box_scale_w
                    local y2 = boxesArr[j][3 + INDEX_BASE] * box_scale_h
                    local left = int(x1)
                    local top = int(y1)
                    local width = int(x2 - x1)
                    local height = int(y2 - y1)
                    classIds[#classIds + 1] = int(classIdxArr[j])
                    confidences[#confidences + 1] = score
                    boxes[#boxes + 1] = { left, top, width, height }
                end
            end
        elseif #outs == 2 and outs[0 + INDEX_BASE].dims == 4 and outs[0 + INDEX_BASE].sizes[#(outs[0 + INDEX_BASE].sizes) - 1 + INDEX_BASE] == 4 then
            -- relative coordinates
            box_scale_w = inpWidth * imgScale
            box_scale_h = inpHeight * imgScale

            local boxesMat = outs[0 + INDEX_BASE]
            local confsMat = outs[1 + INDEX_BASE]
            local numBoxes = int(boxesMat:total() / 4)
            local boxesArr = boxesMat:reshape(1, numBoxes):table()
            local confsArr = confsMat:reshape(1, numBoxes):table()

            for j = 1, numBoxes do
                local classId = common.argmax(confsArr[j]) - INDEX_BASE
                local confidence = float(confsArr[j][classId + INDEX_BASE])
                if confidence > confThreshold then
                    local box = boxesArr[j]
                    local left = int(box[0 + INDEX_BASE] * box_scale_w)
                    local top = int(box[1 + INDEX_BASE] * box_scale_h)
                    local width = int((box[2 + INDEX_BASE] - box[0 + INDEX_BASE]) * box_scale_w)
                    local height = int((box[3 + INDEX_BASE] - box[1 + INDEX_BASE]) * box_scale_h)
                    classIds[#classIds + 1] = classId
                    confidences[#confidences + 1] = confidence
                    boxes[#boxes + 1] = { left, top, width, height }
                end
            end
        else
            error('Unsupported YOLO ONNX output format')
        end
    elseif args.postprocessing == 'yolov8' or args.postprocessing == 'yolov7' or args.postprocessing == 'yolov6' or args.postprocessing == 'yolov5' then
        for i = 1, #outs do
            local out = outs[i]:table()
            local shape

            shape = common.get_dims(out)
            if #shape ~= 3 then
                error(UNSUPPORTED_YOLO_VERSION .. " #shape ~= 3")
            end

            if shape[1] ~= 1 then
                error(UNSUPPORTED_YOLO_VERSION .. " shape[1] ~= 1")
            end

            out = out[0 + INDEX_BASE]
            shape = common.get_dims(out)

            -- absolute coordinate
            box_scale_w = imgScale
            box_scale_h = imgScale

            if shape[2] == #labels + 5 then
                -- yolo v5, v6, v7 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
            elseif shape[1] == #labels + 4 then
                -- yolo v8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
                out = cv.transpose(frame.createFromArray(out, cv.CV_32F)):table()
            else
                error(UNSUPPORTED_YOLO_VERSION .. ' shape[1] ~= #classes + 4 && shape[2] != #classes + 5')
            end

            postprocess_yolo(box_scale_w, box_scale_h, out, classIds, confidences, boxes)
        end
    else
        error('Unknown postprocessing method: ' .. args.postprocessing)
    end

    if capture_counter == 1 then
        print(string.format('postprocess : %.6f s', os.clock() - startTime))
    end

    -- Perform non maximum suppression to eliminate redundant overlapping bounding boxes with lower confidences.
    local indices = cv.dnn.NMSBoxes(boxes, confidences, confThreshold, nmsThreshold)

    network_counter = network_counter + 1

    local fontSize = stdSize -- 0.3 * imgScale
    local fontThickness = imgScale * 0.5  -- max(1, int(imgScale * 0.5))

    drawPred(frame, classIds, confidences, boxes, indices, fontSize, fontThickness)
    resize_and_show(winName, frame)
end

-- Process inputs
cv.namedWindow(winName, cv.WINDOW_AUTOSIZE)

local function callback(pos)
    confThreshold = pos / 100.0
end

cv.createTrackbar('Confidence threshold, %', winName, int(confThreshold * 100), 99, callback)

local input = args.input and cv.samples.findFileOrKeep(args.input) or 0
local cap = cv.VideoCapture(input)

if not cap:isOpened() then
    print('!>Error: cannot open the input file ' .. input)
    os.exit(1)
end

local function process_frame()
    local async = args.async

    if async > 0 and async == #future_outputs then
        -- Skip the frame
        return
    end

    local frameHeight = frame.height
    local frameWidth = frame.width

    local inpWidth = args.width ~= nil and args.width or frameWidth
    local inpHeight = args.height ~= nil and args.height or frameHeight

    local imgScale = frameWidth > frameHeight and (frameWidth / inpWidth) or (frameHeight / inpHeight)

    -- The model expects images of size [ inpWidth x inpHeight ]
    -- Performing a high quality shrinking, instead of the provided one in blobFromImage
    -- improves detection
    local image = frame.zeros(inpHeight, inpWidth, frame:type())
    local imgWidth = math.floor(frameWidth / imgScale)
    local imgHeight = math.floor(frameHeight / imgScale)
    cv.resize(frame, kwargs({
        dsize = { imgWidth, imgHeight },
        interpolation = imgScale < 1 and cv.INTER_CUBIC or cv.INTER_AREA,
        dst = image:new({ 0, 0, imgWidth, imgHeight }),
    }))

    -- Create a 4D blob from a frame.
    cv.dnn.blobFromImage(image, kwargs({
        blob = blob,
        scalefactor = args.scale,
        mean = args.mean,
        size = { inpWidth, inpHeight },
        swapRB = args.rgb,
        ddepth = cv.CV_32F,
    }))

    -- Run a model
    net:setInput(blob)

    if async > 0 and args.backend == cv.dnn.DNN_BACKEND_INFERENCE_ENGINE then
        future_outputs[#future_outputs + 1] = { net:forwardAsync(), frame, inpWidth, inpHeight, imgScale }
    else
        local outs = net:forward(outNames)
        -- net:printPerfProfile()
        postprocess(frame, imgScale, inpWidth, inpHeight, outs)
    end

    local outs = {}
    local future_output
    while #future_outputs ~= 0 and future_outputs[0 + INDEX_BASE][0 + INDEX_BASE]:wait_for(0) do
        future_output, frame, inpWidth, inpHeight, imgScale = unpack(future_outputs[0 + INDEX_BASE])
        table.remove(future_outputs, 0 + INDEX_BASE)
        outs[0 + INDEX_BASE] = future_output:get()
        postprocess(frame, imgScale, inpWidth, inpHeight, outs)
    end
end

local function read_frame()
    local expected, capture_fps, network_fps

    if capture_counter == 0 then
        future_outputs = {}

        if input_fps == 0 then
            input_fps = cap:get(cv.CAP_PROP_FPS)
            if input_fps == 0 then
                input_fps = 30
                cap:set(cv.CAP_PROP_FPS, input_fps)
            end
        end

        if DESIRED_HEIGHT == 0 then
            DESIRED_HEIGHT = int(cap:get(cv.CAP_PROP_FRAME_HEIGHT))
            if DESIRED_HEIGHT == 0 then
                DESIRED_HEIGHT = 640
            end
        end

        if DESIRED_WIDTH == 0 then
            DESIRED_WIDTH = int(cap:get(cv.CAP_PROP_FRAME_WIDTH))
            if DESIRED_WIDTH == 0 then
                DESIRED_WIDTH = 640
            end
        end

        tick_init = cv.getTickCount()
        fps:reset()
    else
        -- get elapsed time since the last read
        local elapsed = (cv.getTickCount() - tick_init) / cv.getTickFrequency()

        -- compute delay before starting read to honor input_fps
        local delay = 1
        if input_fps > 0 then
            expected = input_fps * elapsed
            if expected < capture_counter then
                delay = math.max(1, int(1000 * (capture_counter - expected) / input_fps))
            end
        end

        -- Wait until user press some key
        local key = cv.waitKey(delay)
        if key ~= -1 then return false end

        capture_fps = capture_counter / elapsed
        network_fps = network_counter / elapsed

        fps:frame()
    end

    if not cap:read(frame) then
        cap:release()
        cap = nil
        return false
    end

    if frame:empty() then
        error("frame is empty")
    end

    -- flip to gve the mirror impression if we are capturing a camera
    if type(input) == 'number' then cv.flip(frame, 1, frame) end

    -- Put efficiency information.
    if capture_counter ~= 0 then
        local fontSize = stdSize / 2 -- 0.3 * imgScale / 2
        local fontThickness = 1      -- max(1, int(imgScale * 0.5))


        local label = string.format('Skipped frames: %d', 1000)
        local labelSize, baseLine = cv.getTextSize(label, cv.FONT_HERSHEY_SIMPLEX, fontSize, fontThickness)

        cv.rectangle(frame, { 0, 0 }, { labelSize[0 + INDEX_BASE] + int(5 * fontSize), 5 * (labelSize[1 + INDEX_BASE]) }, { 255, 255, 255 }, cv.FILLED)

        label = string.format('Capture: %.2f', capture_fps)
        cv.putText(frame, label, { 0, int(25 * fontSize) }, cv.FONT_HERSHEY_SIMPLEX, fontSize, { 0, 0, 0 }, fontThickness)

        label = string.format('Network: %.2f', network_fps)
        cv.putText(frame, label, { 0, int(2 * 25 * fontSize) }, cv.FONT_HERSHEY_SIMPLEX, fontSize, { 0, 0, 0 }, fontThickness)

        label = string.format('Instant: %.2f', fps:compute())
        cv.putText(frame, label, { 0, int(3 * 25 * fontSize) }, cv.FONT_HERSHEY_SIMPLEX, fontSize, { 0, 0, 0 }, fontThickness)

        label = string.format('FPS:     %.2f', input_fps)
        cv.putText(frame, label, { 0, int(4 * 25 * fontSize) }, cv.FONT_HERSHEY_SIMPLEX, fontSize, { 0, 0, 0 }, fontThickness)

        label = string.format('Skipped frames: %d', (capture_counter - network_counter))
        cv.putText(frame, label, { 0, int(5 * 25 * fontSize) }, cv.FONT_HERSHEY_SIMPLEX, fontSize, { 0, 0, 0 }, fontThickness)
    end

    capture_counter = capture_counter + 1

    if expected ~= nil and expected > capture_counter and args.skip then
        -- Skip the frame
        resize_and_show(winName, frame)
        return true
    end

    process_frame()
    return true
end

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly cause unwanted behaviour
    -- for that reason, callbacks are suspended and will be resumed when calling yield
    opencv_lua.yield()

    if not read_frame() then break end
end
