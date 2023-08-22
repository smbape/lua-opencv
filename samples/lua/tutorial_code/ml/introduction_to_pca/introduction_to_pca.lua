package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ml/introduction_to_pca/introduction_to_pca.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local bit = bit or opencv_lua.bit
local int = opencv_lua.math.int

local function drawAxis(img, p_, q_, colour, scale)
    local p = {p_[1], p_[2]}
    local q = {q_[1], q_[2]}

    -- [visualization1]
    local angle = math.atan2(p[2] - q[2], p[1] - q[1]) -- angle in radians
    local hypotenuse = math.sqrt((p[2] - q[2]) * (p[2] - q[2]) + (p[1] - q[1]) * (p[1] - q[1]))

    -- Here we lengthen the arrow by a factor of scale
    q[1] = p[1] - scale * hypotenuse * math.cos(angle)
    q[2] = p[2] - scale * hypotenuse * math.sin(angle)
    cv.line(img, {int(p[1]), int(p[2])}, {int(q[1]), int(q[2])}, colour, 1, cv.LINE_AA)

    -- create the arrow hooks
    p[1] = q[1] + 9 * math.cos(angle + math.pi / 4)
    p[2] = q[2] + 9 * math.sin(angle + math.pi / 4)
    cv.line(img, {int(p[1]), int(p[2])}, {int(q[1]), int(q[2])}, colour, 1, cv.LINE_AA)

    p[1] = q[1] + 9 * math.cos(angle - math.pi / 4)
    p[2] = q[2] + 9 * math.sin(angle - math.pi / 4)
    cv.line(img, {int(p[1]), int(p[2])}, {int(q[1]), int(q[2])}, colour, 1, cv.LINE_AA)
    -- [visualization1]
end

local function getOrientation(pts, img)
    -- [pca]
    -- Construct a buffer used by the pca analysis
    local sz = pts.rows
    local cols = pts.cols
    local channels = pts:channels()

    local data_pts = cv.Mat(sz, 2, cv.CV_64F)
    for i = 0, sz - 1 do
        local pt = pts:Point_at(i)
        data_pts:set(pt[1], i, 0)
        data_pts:set(pt[2], i, 1)
    end

    -- Perform PCA analysis
    local mean, eigenvectors, eigenvalues = cv.PCACompute2(data_pts, nil)

    -- Store the center of the object
    local cntr = {int(mean[0]), int(mean[1])}
    -- [pca]

    -- [visualization]
    -- Draw the principal components
    cv.circle(img, cntr, 3, {255, 0, 255}, 2)
    local p1 = {cntr[1] + 0.02 * eigenvectors(0,0) * eigenvalues(0,0), cntr[2] + 0.02 * eigenvectors(0,1) * eigenvalues(0,0)}
    local p2 = {cntr[1] - 0.02 * eigenvectors(1,0) * eigenvalues(1,0), cntr[2] - 0.02 * eigenvectors(1,1) * eigenvalues(1,0)}
    drawAxis(img, cntr, p1, {0, 255, 0}, 1)
    drawAxis(img, cntr, p2, {255, 255, 0}, 5)

    local angle = math.atan2(eigenvectors(0,1), eigenvectors(0,0)) -- orientation in radians
    -- [visualization]

    return angle
end

-- [pre-process]
-- Load image
-- parser = argparse.ArgumentParser(description='Code for Introduction to Principal Component Analysis (PCA) tutorial.\
--                                               This program demonstrates how to use OpenCV PCA to extract the orientation of an object.')
-- parser.add_argument('--input', help='Path to input image.', default='pca_test1.jpg')
-- args = parser.parse_args()

local args = {
    input = 'pca_test1.jpg',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local src = cv.imread(cv.samples.findFile(args.input))
-- Check if image is loaded successfully
if src:empty() then
    print('Could not open or find the image: ', args.input)
    os.exit(0)
end

cv.imshow('src', src)

-- Convert image to grayscale
local gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Convert image to binary
local _, bw = cv.threshold(gray, 50, 255, bit.bor(cv.THRESH_BINARY, cv.THRESH_OTSU))
-- [pre-process]

-- [contours]
-- Find all the contours in the thresholded image
local contours, _ = cv.findContours(bw, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)

for i, c in contours:pairs() do
    -- Calculate the area of each contour
    local area = cv.contourArea(c)
    -- Ignore contours that are too small or too large
    if area >= 1e2 and 1e5 >= area then
        -- Draw each contour only for visualisation purposes
        cv.drawContours(src, contours, i - 1, {0, 0, 255}, 2)
        -- Find the orientation of each shape
        getOrientation(c, src)
    end
end
-- [contours]

cv.imshow('output', src)
cv.waitKey()
