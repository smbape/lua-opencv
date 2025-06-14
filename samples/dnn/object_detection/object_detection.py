# PYTHONPATH="$(realpath opencv-4.10.0-windows/opencv/sources/samples/dnn)" samples/.venv/Scripts/python.exe samples/dnn/object_detection/object_detection.py ssd_tf --input vtest.avi

import argparse
import math
import subprocess
import sys
import copy
import time
from threading import Thread

import numpy as np

if sys.version_info[0] == 2:
    import Queue as queue
else:
    import queue

from common import *
from tf_text_graph_common import readTextMessage
from tf_text_graph_ssd import createSSDGraph
from tf_text_graph_faster_rcnn import createFasterRCNNGraph

backends = (cv.dnn.DNN_BACKEND_DEFAULT, cv.dnn.DNN_BACKEND_HALIDE, cv.dnn.DNN_BACKEND_INFERENCE_ENGINE, cv.dnn.DNN_BACKEND_OPENCV,
            cv.dnn.DNN_BACKEND_VKCOM, cv.dnn.DNN_BACKEND_CUDA)
targets = (cv.dnn.DNN_TARGET_CPU, cv.dnn.DNN_TARGET_OPENCL, cv.dnn.DNN_TARGET_OPENCL_FP16, cv.dnn.DNN_TARGET_MYRIAD, cv.dnn.DNN_TARGET_HDDL,
           cv.dnn.DNN_TARGET_VULKAN, cv.dnn.DNN_TARGET_CUDA, cv.dnn.DNN_TARGET_CUDA_FP16)

parser = argparse.ArgumentParser(add_help=False)
parser.add_argument('--zoo', default=os.path.join(os.path.dirname(os.path.abspath(__file__)), 'models.yml'),
                    help='An optional path to file with preprocessing parameters.')
parser.add_argument('--input', help='Path to input image or video file. Skip this argument to capture frames from a camera.')
parser.add_argument('--out_tf_graph', default='graph.pbtxt',
                    help='For models from TensorFlow Object Detection API, you may '
                         'pass a .config file which was used for training through --config '
                         'argument. This way an additional .pbtxt file with TensorFlow graph will be created.')
parser.add_argument('--framework', choices=['caffe', 'tensorflow', 'torch', 'darknet', 'dldt', 'onnx'],
                    help='Optional name of an origin framework of the model. '
                         'Detect it automatically if it does not set.')
parser.add_argument('--thr', type=float, default=0.5, help='Confidence threshold')
parser.add_argument('--nms', type=float, default=0.4, help='Non-maximum suppression threshold')
parser.add_argument('--backend', choices=backends, default=cv.dnn.DNN_BACKEND_DEFAULT, type=int,
                    help="Choose one of computation backends: "
                         "%d: automatically (by default), "
                         "%d: Halide language (http://halide-lang.org/), "
                         "%d: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), "
                         "%d: OpenCV implementation, "
                         "%d: VKCOM, "
                         "%d: CUDA" % backends)
parser.add_argument('--target', choices=targets, default=cv.dnn.DNN_TARGET_CPU, type=int,
                    help='Choose one of target computation devices: '
                         '%d: CPU target (by default), '
                         '%d: OpenCL, '
                         '%d: OpenCL fp16 (half-float precision), '
                         '%d: NCS2 VPU, '
                         '%d: HDDL VPU, '
                         '%d: Vulkan, '
                         '%d: CUDA, '
                         '%d: CUDA fp16 (half-float preprocess)' % targets)
parser.add_argument('--async', type=int, default=0,
                    dest='asyncN',
                    help='Number of asynchronous forwards at the same time. '
                         'Choose 0 for synchronous mode')
parser.add_argument('--fps', type=int, default=0, help='Input frames per second')
parser.add_argument('-H', '--dheight', type=int, default=0, help='Displayed height')
parser.add_argument('-W', '--dwidth', type=int, default=0, help='Displayed width')
args, _ = parser.parse_known_args()

# add hack to force argument add when the command line is empty
remove_alias = False
if len(sys.argv) <= 1:
    remove_alias = True
    sys.argv.append("--invalid")

add_preproc_args(args.zoo, parser, 'object_detection')

# remove hack to force argument add when the command line is empty
if remove_alias:
    sys.argv.pop()

parser = argparse.ArgumentParser(parents=[parser],
                                 description='Use this script to run object detection deep learning networks using OpenCV.',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)

args = parser.parse_args()

__dirname__ = os.path.dirname(os.path.abspath(__file__))
MODELS_PATH = os.getenv('MODELS_PATH') or os.path.join(__dirname__, 'models')

if not os.path.exists(MODELS_PATH):
    os.mkdir(MODELS_PATH)

if len(sys.argv) > 1:
    subprocess.run([
        sys.executable or 'python',
        os.path.join(__dirname__, 'download_model.py'),
        sys.argv[1],
        '--zoo',
        args.zoo
    ], cwd=MODELS_PATH)

cv.samples.addSamplesDataSearchPath(os.path.abspath(os.path.join(__dirname__, '../../../opencv-4.10.0-windows/opencv/sources/samples/data')))
cv.samples.addSamplesDataSearchPath(os.path.abspath(os.path.join(__dirname__, '../../../opencv-4.10.0-windows/opencv/sources/samples/data/dnn')))
cv.samples.addSamplesDataSearchPath(MODELS_PATH)
cv.samples.addSamplesDataSearchPath(__dirname__)

args.model = findFile(args.model)
args.config = findFile(args.config)
args.classes = findFile(args.classes)

# If config specified, try to load it as TensorFlow Object Detection API's pipeline.
config = readTextMessage(args.config)
if 'model' in config:
    print('TensorFlow Object Detection API config detected')
    if 'ssd' in config['model'][0]:
        print('Preparing text graph representation for SSD model: ' + args.out_tf_graph)
        createSSDGraph(args.model, args.config, args.out_tf_graph)
        args.config = args.out_tf_graph
    elif 'faster_rcnn' in config['model'][0]:
        print('Preparing text graph representation for Faster-RCNN model: ' + args.out_tf_graph)
        createFasterRCNNGraph(args.model, args.config, args.out_tf_graph)
        args.config = args.out_tf_graph


# Load names of classes
classes = None
if args.classes:
    with open(args.classes, 'rt') as f:
        classes = f.read().rstrip('\n').split('\n')

# Load a network
net = cv.dnn.readNet(args.model, args.config, args.framework)
net.setPreferableBackend(args.backend)
net.setPreferableTarget(args.target)
outNames = net.getUnconnectedOutLayersNames()

confThreshold = args.thr
nmsThreshold = args.nms

UNSUPPORTED_YOLO_VERSION = 'Unsupported yolo version. Supported versions are v3, v4, v5, v6, v7, v8.'

DESIRED_HEIGHT = args.dheight
DESIRED_WIDTH = args.dwidth


def resize_and_show(title, image):
    h, w = image.shape[:2]

    if h < w:
        h = math.floor(h / (w / DESIRED_WIDTH))
        w = DESIRED_WIDTH
    else:
        w = math.floor(w / (h / DESIRED_HEIGHT))
        h = DESIRED_HEIGHT

    height, width = image.shape[:2]
    interpolation = cv.INTER_CUBIC if DESIRED_WIDTH > width or DESIRED_HEIGHT > height else cv.INTER_AREA
    img = cv.resize(image.astype(np.uint8), (w, h), interpolation=interpolation)
    cv.imshow(title, img)
    return img.shape[0] / image.shape[0]


def drawPred(frame, imgScale, classId, conf, left, top, right, bottom):
    fontScale = 0.3 * imgScale
    thickness = max(1, int(imgScale))

    # Draw a bounding box.
    cv.rectangle(frame, (left, top), (right, bottom), (0, 255, 0), thickness)

    label = '%.2f' % conf

    # Print a label of class.
    if classes:
        assert(classId < len(classes))
        label = '%s: %s' % (classes[classId], label)

    labelSize, baseLine = cv.getTextSize(label, cv.FONT_HERSHEY_SIMPLEX, fontScale, thickness)
    top = max(top, labelSize[1])
    cv.rectangle(frame, (left, top - labelSize[1]), (left + labelSize[0], top + baseLine), (255, 255, 255), cv.FILLED)
    cv.putText(frame, label, (left, top), cv.FONT_HERSHEY_SIMPLEX, fontScale, (0, 0, 0), thickness)


def yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, offset):
    for detection in out:
        if offset == 5 and detection[4] <= confThreshold and not (args.background_label_id >= 0):
            continue

        scores = detection[offset:]
        classId = np.argmax(scores)
        confidence = scores[classId]
        if confidence <= confThreshold:
            continue

        center_x = detection[0] * box_scale_w
        center_y = detection[1] * box_scale_h
        width = int(detection[2] * box_scale_w)
        height = int(detection[3] * box_scale_h)
        left = int(center_x - width / 2)
        top = int(center_y - height / 2)

        classIds.append(classId)
        confidences.append(float(confidence))
        boxes.append([left, top, width, height])

def postprocess(frame, imgScale, inpWidth, inpHeight, outs):
    layerNames = net.getLayerNames()
    lastLayerId = net.getLayerId(layerNames[-1])
    lastLayer = net.getLayer(lastLayerId)

    startTime = time.time()

    classIds = []
    confidences = []
    boxes = []

    if lastLayer.type == 'DetectionOutput':
        # Network produces output blob with a shape 1x1xNx7 where N is a number of
        # detections and an every detection is a vector of values
        # [batchId, classId, confidence, left, top, right, bottom]
        for out in outs:
            for detection in out[0, 0]:
                if detection[5] - detection[3] < 1:
                    # relative coordinates
                    box_scale_w = inpWidth * imgScale
                    box_scale_h = inpHeight * imgScale
                else:
                    # absolute coordinate
                    box_scale_w = imgScale
                    box_scale_h = imgScale

                confidence = detection[2]
                if confidence <= confThreshold:
                    continue

                left = int(detection[3] * box_scale_w)
                top = int(detection[4] * box_scale_h)
                width = int(detection[5] * box_scale_w - left + 1)
                height = int(detection[6] * box_scale_h - top + 1)

                classIds.append(int(detection[1] - 1 if 0 <= args.background_label_id <= detection[1] else detection[1]))
                confidences.append(float(confidence))
                boxes.append([left, top, width, height])
    elif lastLayer.type == 'Region':
        # yolo v3, v4

        # relative coordinates
        box_scale_w = inpWidth * imgScale
        box_scale_h = inpHeight * imgScale

        # Network produces output blob with a shape NxC where N is a number of
        # detected objects and C is a number of classes + 4 where the first 4
        # numbers are [center_x, center_y, width, height]
        for out in outs:
            yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, offset=5)
    elif lastLayer.type == "Identity":
        for out in outs:
            if len(out.shape) != 3:
                print(UNSUPPORTED_YOLO_VERSION + " len(out.shape) != 3")
                exit(1)

            if out.shape[0] != 1:
                print(UNSUPPORTED_YOLO_VERSION + " out.shape[0] != 1")
                exit(1)

            out = out[0]

            # absolute coordinate
            box_scale_w = imgScale
            box_scale_h = imgScale

            if (out.shape[1] == len(classes) + 5):
                # yolo v5, v6, v7 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
                offset = 5
            elif out.shape[0] == len(classes) + 4:
                # yolo v8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
                offset = 4
                out = out.transpose(1, 0)
            else:
                print(UNSUPPORTED_YOLO_VERSION + " out.shape[0] != len(classes) + 4 && out.shape[1] != len(classes) + 5")
                exit(1)

            yolo_object_detection_postprocess(box_scale_w, box_scale_h, out, classIds, confidences, boxes, offset=offset)
    else:
        print('Unknown output layer type: ' + lastLayer.type)
        exit(1)

    if predictionsQueue.counter == 1:
        print(f'postprocess : {time.time() - startTime} s')

    # Perform non maximum suppression to eliminate redundant overlapping bounding boxes with lower confidences.
    indices = cv.dnn.NMSBoxes(boxes, confidences, confThreshold, nmsThreshold)

    for i in indices:
        box = boxes[i]
        left = box[0]
        top = box[1]
        width = box[2]
        height = box[3]
        drawPred(frame, imgScale, classIds[i], confidences[i], left, top, left + width, top + height)

# Process inputs
winName = 'Deep learning object detection in OpenCV'
cv.namedWindow(winName, cv.WINDOW_AUTOSIZE)

def callback(pos):
    global confThreshold
    confThreshold = pos / 100.0

cv.createTrackbar('Confidence threshold, %', winName, int(confThreshold * 100), 99, callback)

cap = cv.VideoCapture(cv.samples.findFileOrKeep(args.input) if args.input else 0)

class QueueFPS(queue.Queue):
    def __init__(self):
        queue.Queue.__init__(self)
        self.startTime = 0
        self.counter = 0

    def put(self, v):
        queue.Queue.put(self, v)
        self.counter += 1
        if self.counter == 1:
            self.startTime = time.time()

    def getFPS(self):
        return self.counter / (time.time() - self.startTime)


process = True

#
# Frames capturing thread
#
framesQueue = QueueFPS()
inputFPS = args.fps
def framesThreadBody():
    global framesQueue, process, inputFPS, DESIRED_HEIGHT, DESIRED_WIDTH

    while process:
        hasFrame, frame = cap.read()
        if not hasFrame:
            break

        if framesQueue.counter == 0:
            if inputFPS == 0:
                inputFPS = cap.get(cv.CAP_PROP_FPS)
                if inputFPS == 0:
                    inputFPS = 30
                    cap.set(cv.CAP_PROP_FPS, inputFPS)

            if DESIRED_HEIGHT == 0:
                DESIRED_HEIGHT = int(cap.get(cv.CAP_PROP_FRAME_HEIGHT))
                if DESIRED_HEIGHT == 0:
                    DESIRED_HEIGHT = 640

            if DESIRED_WIDTH == 0:
                DESIRED_WIDTH = int(cap.get(cv.CAP_PROP_FRAME_WIDTH))
                if DESIRED_WIDTH == 0:
                    DESIRED_WIDTH = 640

            tickInit = cv.getTickCount()
        else:
            elapsed = (cv.getTickCount() - tickInit) / cv.getTickFrequency()
            expected = inputFPS * elapsed

            if expected < framesQueue.counter:
                time.sleep((framesQueue.counter - expected) / inputFPS)
                # elapsed = (cv.getTickCount() - tickInit) / cv.getTickFrequency()

        framesQueue.put(frame)


#
# Frames processing thread
#
processedFramesQueue = queue.Queue()
predictionsQueue = QueueFPS()
def processingThreadBody():
    global processedFramesQueue, predictionsQueue, args, process

    futureOutputs = []
    while process:
        # Get a next frame
        frame = None
        try:
            frame = framesQueue.get_nowait()

            if args.asyncN:
                if len(futureOutputs) == args.asyncN:
                    frame = None  # Skip the frame
            else:
                framesQueue.queue.clear()  # Skip the rest of frames
        except queue.Empty:
            pass


        if not frame is None:
            frameHeight = frame.shape[0]
            frameWidth = frame.shape[1]

            # Create a 4D blob from a frame.
            inpWidth = args.width if args.width else frameWidth
            inpHeight = args.height if args.height else frameHeight

            imgScale = frameWidth / inpWidth if frameWidth > frameHeight else frameHeight / inpHeight

            # The model expects images of size [ inpWidth x inpHeight ]
            # Performing a high quality shrinking, instead of the provided one in blobFromImage
            # improves detection
            image = np.zeros((inpHeight, inpWidth, 3), np.uint8)
            imgWidth = math.floor(frameWidth / imgScale)
            imgHeight = math.floor(frameHeight / imgScale)
            image[0:imgHeight, 0:imgWidth] = cv.resize(frame,
                dsize=(imgWidth, imgHeight),
                interpolation=cv.INTER_CUBIC if imgScale < 1 else cv.INTER_AREA
            )

            blob = cv.dnn.blobFromImage(image, size=(inpWidth, inpHeight), swapRB=args.rgb, ddepth=cv.CV_8U)
            processedFramesQueue.put((frame, imgScale, inpWidth, inpHeight))

            # Run a model
            net.setInput(blob, scalefactor=args.scale, mean=args.mean)
            if net.getLayer(0).outputNameToIndex('im_info') != -1:  # Faster-RCNN or R-FCN
                net.setInput(np.array([[inpHeight, inpWidth, 1.6]], dtype=np.float32), 'im_info')

            if args.asyncN and args.backend == cv.dnn.DNN_BACKEND_INFERENCE_ENGINE:
                futureOutputs.append(net.forwardAsync())
            else:
                outs = net.forward(outNames)
                predictionsQueue.put(copy.deepcopy(outs))

        while futureOutputs and futureOutputs[0].wait_for(0):
            out = futureOutputs[0].get()
            predictionsQueue.put(copy.deepcopy([out]))

            del futureOutputs[0]


framesThread = Thread(target=framesThreadBody)
framesThread.start()

processingThread = Thread(target=processingThreadBody)
processingThread.start()

#
# Postprocessing and rendering loop
#
while cv.waitKey(1) < 0:
    try:
        # Request prediction first because they put after frames
        outs = predictionsQueue.get_nowait()
        frame, imgScale, inpWidth, inpHeight = processedFramesQueue.get_nowait()

        postprocess(frame, imgScale, inpWidth, inpHeight, outs)

        # Put efficiency information.
        if predictionsQueue.counter > 1:
            label = 'Camera: %.2f FPS' % (framesQueue.getFPS())
            cv.putText(frame, label, (0, 15), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0))

            label = 'Network: %.2f FPS' % (predictionsQueue.getFPS())
            cv.putText(frame, label, (0, 30), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0))

            label = 'Skipped frames: %d' % (framesQueue.counter - predictionsQueue.counter)
            cv.putText(frame, label, (0, 45), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0))

        resize_and_show(winName, frame)
    except queue.Empty:
        pass


process = False
framesThread.join()
processingThread.join()
