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
    https://github.com/opencv/opencv/blob/4.10.0/samples/dnn/object_detection.py
--]]

local __file__ = arg[0]

local argparse = require("argparse")
local sysPath = require("path")
local opencv_lua = require("init")
local common = require("common")

local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = function(val) return opencv_lua.math.int(tonumber(val)) end
local float = tonumber
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local findFile = common.findFile
local INDEX_BASE = 1 -- lua is 1-based indexed

local default_models = sysPath.join(sysPath.dirname(sysPath.abspath(__file__)), 'models.yml')
local backends = { cv.dnn.DNN_BACKEND_DEFAULT, cv.dnn.DNN_BACKEND_HALIDE, cv.dnn.DNN_BACKEND_INFERENCE_ENGINE, cv.dnn
    .DNN_BACKEND_OPENCV,
    cv.dnn.DNN_BACKEND_VKCOM, cv.dnn.DNN_BACKEND_CUDA }
local targets = { cv.dnn.DNN_TARGET_CPU, cv.dnn.DNN_TARGET_OPENCL, cv.dnn.DNN_TARGET_OPENCL_FP16, cv.dnn
    .DNN_TARGET_MYRIAD, cv.dnn.DNN_TARGET_HDDL,
    cv.dnn.DNN_TARGET_VULKAN, cv.dnn.DNN_TARGET_CUDA, cv.dnn.DNN_TARGET_CUDA_FP16 }

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
parser:option('--framework'):choices({ 'caffe', 'tensorflow', 'torch', 'darknet', 'dldt', 'onnx' })
    :description('Optional name of an origin framework of the model. ' ..
        'Detect it automatically if it does not set.')
parser:option('--thr'):convert(float):default(0.5):description('Confidence threshold')
parser:option('--nms'):convert(float):default(0.4):description('Non-maximum suppression threshold')
parser:option('--backend'):choices(common.map_tostring(backends)):default(cv.dnn.DNN_BACKEND_DEFAULT):convert(int)
    :description(string.format("Choose one of computation backends: " ..
        "%d: automatically (by default), " ..
        "%d: Halide language (http://halide-lang.org/), " ..
        "%d: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), " ..
        "%d: OpenCV implementation, " ..
        "%d: VKCOM, " ..
        "%d: CUDA", unpack(backends)))
parser:option('--target'):choices(common.map_tostring(targets)):default(cv.dnn.DNN_TARGET_CPU):convert(int)
    :description(string.format('Choose one of target computation devices: ' ..
        '%d: CPU target (by default), ' ..
        '%d: OpenCL, ' ..
        '%d: OpenCL fp16 (half-float precision), ' ..
        '%d: NCS2 VPU, ' ..
        '%d: HDDL VPU, ' ..
        '%d: Vulkan, ' ..
        '%d: CUDA, ' ..
        '%d: CUDA fp16 (half-float preprocess)', unpack(targets)))
parser:option('--async'):convert(int):default(cv.getNumThreads())
    :description('Number of asynchronous forwards at the same time. ' ..
        'Choose 0 for synchronous mode')
parser:option('--fps'):convert(int):default(0):description('Input frames per second')
parser:option('-H --dheight'):convert(int):default(0):description('Displayed height')
parser:option('-W --dwidth'):convert(int):default(0):description('Displayed width')
parser:flag('--skip'):description('Skip frames to honor input fps')

local default_cmdline = rawget(_G, "arg") or {}

local known_args = {}

for i = 2, #default_cmdline do
    if default_cmdline[i] == "--zoo" then
        known_args[#known_args + 1] = default_cmdline[i]
        if i + 1 <= #default_cmdline then
            i = i + 1
            known_args[#known_args + 1] = default_cmdline[i]
        end
    elseif default_cmdline[i]:sub(1, #"--zoo=") == "--zoo=" then
        known_args[#known_args + 1] = default_cmdline[i]
    end
end

known_args = parser:parse(known_args)

common.add_preproc_args(known_args.zoo, parser, 'object_detection')

local args = parser:parse()

local __dirname__ = sysPath.dirname(sysPath.abspath(__file__))
local MODELS_PATH = os.getenv('MODELS_PATH') or sysPath.join(__dirname__, 'models')

local OPENCV_SAMPLES_DATA_PATH = opencv_lua.env.OPENCV_SAMPLES_DATA_PATH

local SAMPLES_PATH = opencv_lua.fs_utils.findFile("samples")
local PYTHON_VENV_PATH = os.getenv('PYTHON_VENV_PATH') or sysPath.join(SAMPLES_PATH, ".venv")
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

args.model = findFile(args.model)
args.config = findFile(args.config)
args.classes = findFile(args.classes)

-- shared variables
local capture_counter = 0
local network_counter = 0
local tick_init = 0
local frame = cv.UMat()
local input_fps = args.fps
local future_outputs
local winName = 'Deep learning object detection in OpenCV'

-- Load names of classes
local classes
if args.classes then
    local content = common.read_file(args.classes)
    classes = {}
    for str in content:gmatch("([^\r\n]+)") do
        classes[#classes + 1] = str
    end
end

-- Load a network
local net = cv.dnn.readNet(args.model, args.config, args.framework)
net:setPreferableBackend(args.backend)
net:setPreferableTarget(args.target)
local outNames = net:getUnconnectedOutLayersNames()

local confThreshold = args.thr
local nmsThreshold = args.nms

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


local function drawPred(frame, classId, conf, left, top, right, bottom)
    local fontScale = 0.8
    local thickness = 2

    -- Draw a bounding box.
    cv.rectangle(frame, { left, top }, { right, bottom }, { 0, 255, 0 }, thickness)

    local label = string.format('%.2f', conf)

    -- Print a label of class.
    if classes then
        assert(classId < #classes)
        label = string.format('%s: %s', classes[classId + INDEX_BASE], label)
    end

    local labelSize, baseLine = cv.getTextSize(label, cv.FONT_HERSHEY_SIMPLEX, fontScale, thickness)
    top = math.max(top, labelSize[1 + INDEX_BASE])
    cv.rectangle(frame, { left, top - labelSize[1 + INDEX_BASE] }, { left + labelSize[0 + INDEX_BASE], top + baseLine },
        { 255, 255, 255 }, cv.FILLED)
    cv.putText(frame, label, { left, top }, cv.FONT_HERSHEY_SIMPLEX, fontScale, { 0, 0, 0 }, thickness)
end


local function yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, offset)
    for i, detection in pairs(out) do
        if offset ~= 5 or args.background_label_id >= 0 or detection[4 + INDEX_BASE] > confThreshold then
            local classId = common.argmax(common.slice(detection, offset + INDEX_BASE)) - INDEX_BASE
            local confidence = detection[classId + offset + INDEX_BASE]
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
end


local function postprocess(frame, imgScale, inpWidth, inpHeight, outs)
    local layerNames = net:getLayerNames()
    local lastLayerId = net:getLayerId(layerNames[#layerNames])
    local lastLayer = net:getLayer(lastLayerId)

    local startTime = os.clock()

    local classIds = {}
    local confidences = {}
    local boxes = {}

    local box_scale_w, box_scale_h

    if lastLayer.type == 'DetectionOutput' then
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
                    local skip_background = 0 <= args.background_label_id and args.background_label_id <= classId

                    classIds[#classIds + 1] = skip_background and classId - 1 or classId
                    confidences[#confidences + 1] = float(confidence)
                    boxes[#boxes + 1] = { left, top, width, height }
                end
            end
        end
    elseif lastLayer.type == 'Region' then
        -- yolo v3, v4

        -- relative coordinates
        box_scale_w = inpWidth * imgScale
        box_scale_h = inpHeight * imgScale

        -- Network produces output blob with a shape NxC where N is a number of
        -- detected objects and C is a number of classes + 4 where the first 4
        -- numbers are [center_x, center_y, width, height]
        for i = 1, #outs do
            local out = outs[i]:table()
            yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, 5)
        end
    elseif lastLayer.type == "Identity" then
        for i = 1, #outs do
            local out = outs[i]:table()
            local shape, offset

            shape = common.get_dims(out)
            if #shape ~= 3 then
                print(UNSUPPORTED_YOLO_VERSION .. " #shape ~= 3")
                os.exit(1)
            end

            if shape[1] ~= 1 then
                print(UNSUPPORTED_YOLO_VERSION .. " shape[1] ~= 1")
                os.exit(1)
            end

            out = out[0 + INDEX_BASE]
            shape = common.get_dims(out)

            -- absolute coordinate
            box_scale_w = imgScale
            box_scale_h = imgScale

            if (shape[2] == #classes + 5) then
                -- yolo v5, v6, v7 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
                offset = 5
            elseif shape[1] == #classes + 4 then
                -- yolo v8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
                offset = 4
                out = cv.transpose(frame.createFromArray(out, cv.CV_32F)):table()
            else
                print(UNSUPPORTED_YOLO_VERSION .. ' shape[1] ~= #classes + 4 && shape[2] != #classes + 5')
                os.exit(1)
            end

            yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, offset)
        end
    else
        print('Unknown output layer type: ' .. lastLayer.type)
        os.exit(1)
    end

    if capture_counter == 1 then
        print(string.format('postprocess : %.3f s', os.clock() - startTime))
    end

    -- Perform non maximum suppression to eliminate redundant overlapping bounding boxes with lower confidences.
    local indices = cv.dnn.NMSBoxes(boxes, confidences, confThreshold, nmsThreshold)

    for _, i in ipairs(indices) do
        local box = boxes[i + INDEX_BASE]
        local left = box[0 + INDEX_BASE]
        local top = box[1 + INDEX_BASE]
        local width = box[2 + INDEX_BASE]
        local height = box[3 + INDEX_BASE]
        drawPred(frame, classIds[i + INDEX_BASE], confidences[i + INDEX_BASE], left, top, left + width, top + height)
    end

    resize_and_show(winName, frame)

    network_counter = network_counter + 1
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

local function process_frame(future_outputs, frame)
    if args.async > 0 and args.async == #future_outputs then
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
    local blob = cv.dnn.blobFromImage(image, kwargs({
        size = { inpWidth, inpHeight },
        swapRB = args.rgb,
        ddepth = cv.CV_8U,
    }))

    -- Run a model
    net:setInput(blob, kwargs({ scalefactor = args.scale, mean = args.mean }))
    if net:getLayer(0):outputNameToIndex('im_info') ~= -1 then -- Faster-RCNN or R-FCN
        net:setInput(frame.createFromArray({ { inpHeight, inpWidth, 1.6 } }, cv.CV_32F), 'im_info')
    end

    if args.async > 0 and args.backend == cv.dnn.DNN_BACKEND_INFERENCE_ENGINE then
        future_outputs[#future_outputs + 1] = { net:forwardAsync(), frame, inpWidth, inpHeight, imgScale }
    else
        local outs = net:forward(outNames)
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
    local expected, capture_fps, network_fps, label

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
    else
        -- get elapsed time since the last read
        local elapsed = (cv.getTickCount() - tick_init) / cv.getTickFrequency()

        -- compute delay before starting read to honor input_fps
        local delay = 1
        expected = input_fps * elapsed
        if expected < capture_counter then
            delay = math.max(1, int(1000 * (capture_counter - expected) / input_fps))
        end

        -- Wait until user press some key
        local key = cv.waitKey(delay)
        if key ~= -1 then return false end

        capture_fps = capture_counter / elapsed
        network_fps = network_counter / elapsed
    end

    if not cap:read(frame) then
        cap:release()
        cap = nil
        return false
    end

    -- flip to gve the mirror impression if we are capturing a camera
    if type(input) == 'number' then cv.flip(frame, 1, frame) end

    -- Put efficiency information.
    if capture_counter ~= 0 then
        label = string.format('Capture: %.2f/%.2f FPS', capture_fps, input_fps)
        cv.putText(frame, label, { 0, 15 }, cv.FONT_HERSHEY_SIMPLEX, 0.5, { 0, 255, 0 })

        label = string.format('Network: %.2f/%.2f FPS', network_fps, input_fps)
        cv.putText(frame, label, { 0, 30 }, cv.FONT_HERSHEY_SIMPLEX, 0.5, { 0, 255, 0 })

        label = string.format('Skipped frames: %d', (capture_counter - network_counter))
        cv.putText(frame, label, { 0, 45 }, cv.FONT_HERSHEY_SIMPLEX, 0.5, { 0, 255, 0 })
    end

    capture_counter = capture_counter + 1

    if expected ~= nil and expected > capture_counter and args.skip then
        -- Skip the frame
        resize_and_show(winName, frame)
        return true
    end

    process_frame(future_outputs, frame)
    return true
end

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly cause unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    if not read_frame() then break end
end
