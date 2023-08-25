package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ml/introduction_to_svm/introduction_to_svm.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local INDEX_BASE = 1 -- lua is 1-based indexed

-- Set up training data
-- [setup1]
local labels = cv.Mat.createFromArray({ 1, -1, -1, -1 }, cv.CV_32SC1)
local trainingData = cv.Mat.createFromArray({ { 501, 10 }, { 255, 10 }, { 501, 255 }, { 10, 501 } }, cv.CV_32F)
-- [setup1]

-- Train the SVM
-- [init]
local svm = cv.ml.SVM.create()
svm:setType(cv.ml.SVM.C_SVC)
svm:setKernel(cv.ml.SVM.LINEAR)
svm:setTermCriteria(cv.TermCriteria(cv.TermCriteria.MAX_ITER, 100, 1e-6))
-- [init]
-- [train]
svm:train(trainingData, cv.ml.ROW_SAMPLE, labels)
-- [train]

-- Data for visual representation
local width = 512
local height = 512
local image = cv.Mat.zeros(height, width, cv.CV_8UC3)

-- Show the decision regions given by the SVM
-- [show]
local green = { 0, 255, 0 }
local blue = { 255, 0, 0 }
local sampleMat = cv.Mat.zeros(1, 2, cv.CV_32F)
local use_sweet_spots = true

local t = os.clock()

if use_sweet_spots then
    -- give channels a new dimension to be able to do image(y, x, c), which is a speed sweet spot
    image = image:reshape(1, { height, width, 3 })
end

for i = 0, height - 1 do
    for j = 0, width - 1 do
        sampleMat[0] = j
        sampleMat[1] = i
        local _, results = svm:predict(sampleMat)
        local response = results[0]

        if use_sweet_spots then
            if response == 1 then
                image:set(green[0 + INDEX_BASE], i, j, 0)
                image:set(green[1 + INDEX_BASE], i, j, 1)
                image:set(green[2 + INDEX_BASE], i, j, 2)
            elseif response == -1 then
                image:set(blue[0 + INDEX_BASE], i, j, 0)
                image:set(blue[1 + INDEX_BASE], i, j, 1)
                image:set(blue[2 + INDEX_BASE], i, j, 2)
            end
        else
            if response == 1 then
                image:Vec3b_set_at(i, j, green)
            elseif response == -1 then
                image:Vec3b_set_at(i, j, blue)
            end
        end
    end
end
-- [show]

if use_sweet_spots then
    -- restore channels
    image = image:reshape(3, { height, width })
end

print(string.format("Hand written function time passed in seconds: %.3f", os.clock() - t))

-- Show the training data
-- [show_data]
local thickness = -1
cv.circle(image, { 501, 10 }, 5, { 0, 0, 0 }, thickness)
cv.circle(image, { 255, 10 }, 5, { 255, 255, 255 }, thickness)
cv.circle(image, { 501, 255 }, 5, { 255, 255, 255 }, thickness)
cv.circle(image, { 10, 501 }, 5, { 255, 255, 255 }, thickness)
-- [show_data]

-- Show support vectors
-- [show_vectors]
thickness = 2
local sv = svm:getUncompressedSupportVectors()

for i = 0, #sv - 1 do
    cv.circle(image, { int(sv(i, 0)), int(sv(i, 1)) }, 6, { 128, 128, 128 }, thickness)
end
-- [show_vectors]

cv.imshow('SVM Simple Example', image) -- show it to the user
cv.waitKey()
