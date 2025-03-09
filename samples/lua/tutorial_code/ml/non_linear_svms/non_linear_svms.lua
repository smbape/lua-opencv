#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/ml/non_linear_svms/non_linear_svms.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local INDEX_BASE = 1          -- lua is 1-based indexed

local NTRAINING_SAMPLES = 100 -- Number of training samples per class
local FRAC_LINEAR_SEP = 0.9   -- Fraction of samples which compose the linear separable part

-- Data for visual representation
local WIDTH = 512
local HEIGHT = 512
local I = cv.Mat.zeros(HEIGHT, WIDTH, cv.CV_8UC3)

-- --------------------- 1. Set up training data randomly ---------------------------------------
local trainData = cv.Mat(2 * NTRAINING_SAMPLES, 2, cv.CV_32F)
local labels = cv.Mat(2 * NTRAINING_SAMPLES, 1, cv.CV_32S)

local rng = cv.RNG(100) -- Random value generation class

-- Set up the linearly separable part of the training data
local nLinearSamples = int(FRAC_LINEAR_SEP * NTRAINING_SAMPLES)

-- [setup1]
-- Generate random points for the class 1
local trainClass = trainData:rowRange(0, nLinearSamples)
-- The x coordinate of the points is in [0, 0.4)
local c = trainClass:colRange(0, 1)
rng:fill(c, cv.RNG.UNIFORM, 0, 0.4 * WIDTH)
-- The y coordinate of the points is in [0, 1)
c = trainClass:colRange(1, 2)
rng:fill(c, cv.RNG.UNIFORM, 0, HEIGHT)

-- Generate random points for the class 2
trainClass = trainData:rowRange(2 * NTRAINING_SAMPLES - nLinearSamples, 2 * NTRAINING_SAMPLES)
-- The x coordinate of the points is in [0.6, 1]
c = trainClass:colRange(0, 1)
rng:fill(c, cv.RNG.UNIFORM, 0.6 * WIDTH, WIDTH)
-- The y coordinate of the points is in [0, 1)
c = trainClass:colRange(1, 2)
rng:fill(c, cv.RNG.UNIFORM, 0, HEIGHT)
-- [setup1]

-------------------- Set up the non-linearly separable part of the training data ---------------
-- [setup2]
-- Generate random points for the classes 1 and 2
trainClass = trainData:rowRange(nLinearSamples, 2 * NTRAINING_SAMPLES - nLinearSamples)
-- The x coordinate of the points is in [0.4, 0.6)
c = trainClass:colRange(0, 1)
rng:fill(c, cv.RNG.UNIFORM, 0.4 * WIDTH, 0.6 * WIDTH)
-- The y coordinate of the points is in [0, 1)
c = trainClass:colRange(1, 2)
rng:fill(c, cv.RNG.UNIFORM, 0, HEIGHT)
-- [setup2]

--------------------------- Set up the labels for the classes ---------------------------------
labels:rowRange(0, NTRAINING_SAMPLES):setTo(1)                     -- Class 1
labels:rowRange(NTRAINING_SAMPLES, 2 * NTRAINING_SAMPLES):setTo(2) -- Class 2

-------------------------- 2. Set up the support vector machines parameters --------------------
print('Starting training process')
-- [init]
local svm = cv.ml.SVM.create()
svm:setType(cv.ml.SVM.C_SVC)
svm:setC(0.1)
svm:setKernel(cv.ml.SVM.LINEAR)
svm:setTermCriteria(cv.TermCriteria(cv.TermCriteria.MAX_ITER, int(1e7), 1e-6))
-- [init]

-------------------------- 3. Train the svm ----------------------------------------------------
-- [train]
svm:train(trainData, cv.ml.ROW_SAMPLE, labels)
-- [train]
print('Finished training process')

-------------------------- 4. Show the decision regions ----------------------------------------
-- [show]
local green = { 0, 100, 0 }
local blue = { 100, 0, 0 }
local rows, cols, type = I.rows, I.cols, I:type()
local sampleMat = cv.Mat.zeros(1, 2, cv.CV_32F)

local t = os.clock()

-- transform into an lua table for faster processing in lua
I = I:table()

for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        sampleMat[0] = j
        sampleMat[1] = i
        local _, results = svm:predict(sampleMat)
        local response = results[0]

        if response == 1 then
            I[i + INDEX_BASE][j + INDEX_BASE] = green
        elseif response == 2 then
            I[i + INDEX_BASE][j + INDEX_BASE] = blue
        end
    end
end

-- transform back to matrix
I = cv.Mat.createFromArray(I, type)

print(string.format('Show the decision regions took %s seconds', os.clock() - t))
-- [show]

------------------------- 5. Show the training data --------------------------------------------
-- [show_data]
local thick = -1
-- Class 1
for i = 0, NTRAINING_SAMPLES - 1 do
    local px = trainData(i, 0)
    local py = trainData(i, 1)
    cv.circle(I, { int(px), int(py) }, 3, { 0, 255, 0 }, thick)
end

-- Class 2
for i = NTRAINING_SAMPLES, 2 * NTRAINING_SAMPLES - 1 do
    local px = trainData(i, 0)
    local py = trainData(i, 1)
    cv.circle(I, { int(px), int(py) }, 3, { 255, 0, 0 }, thick)
end
-- [show_data]

--------------------------- 6. Show support vectors --------------------------------------------
-- [show_vectors]
thick = 2
local sv = svm:getUncompressedSupportVectors()

for i = 0, #sv - 1 do
    cv.circle(I, { int(sv(i, 0)), int(sv(i, 1)) }, 6, { 128, 128, 128 }, thick)
end
-- [show_vectors]

-- cv.imwrite('result.png', I)                      -- save the Image
cv.imshow('SVM for Non-Linear Training Data', I) -- show it to the user
cv.waitKey()
