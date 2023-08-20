package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/core/discrete_fourier_transform/discrete_fourier_transform.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local bit = bit or opencv_lua.bit
local round = opencv_lua.math.round

local function print_help()
    print([[
    This program demonstrated the use of the discrete Fourier transform (DFT).
    The dft of an image is taken and it's power spectrum is displayed.
    Usage:
    discrete_fourier_transform.lua [image_name -- default lena.jpg]
]])
end

local function main(argv)
    print_help()

    local filename = #argv > 2 and argv[1] or 'lena.jpg'

    I = cv.imread(cv.samples.findFile(filename), cv.IMREAD_GRAYSCALE)
    if I:empty() then
        error('Error opening image')
    end
    -- [expand]
    local rows, cols = I.rows, I.cols
    local m = cv.getOptimalDFTSize(rows)
    local n = cv.getOptimalDFTSize(cols)
    local padded = cv.copyMakeBorder(I, 0, m - rows, 0, n - cols, cv.BORDER_CONSTANT, kwargs({ value = { 0, 0, 0 } }))
    -- [expand]
    -- [complex_and_real]
    local planes = { padded:convertTo(cv.CV_32F), cv.Mat.zeros(padded.rows, padded.cols, cv.CV_32F) }
    local complexI = cv.merge(planes) -- Add to the expanded another plane with zeros
    -- [complex_and_real]
    -- [dft]
    cv.dft(complexI, complexI) -- this way the result may fit in the source matrix
    -- [dft]
    -- compute the magnitude and switch to logarithmic scale
    -- = > log(1 + sqrt(Re(DFT(I)) ^ 2 + Im(DFT(I)) ^ 2))
    -- [magnitude]
    cv.split(complexI, planes)                    -- planes[1] = Re(DFT(I), planes[2] = Im(DFT(I))
    cv.magnitude(planes[1], planes[2], planes[1]) -- planes[1] = magnitude
    local magI = planes[1]
    -- [magnitude]
    -- [log]
    local matOfOnes = cv.Mat.ones(magI.rows, magI.cols, magI:type())
    cv.add(matOfOnes, magI, magI) --  switch to logarithmic scale
    cv.log(magI, magI)
    -- [log]
    -- [crop_rearrange]
    local magI_rows, magI_cols = magI.rows, magI.cols
    -- crop the spectrum, if it has an odd number of rows or columns
    magI = magI:new({ 0, 0, bit.band(magI_cols, -2), bit.band(magI_rows, -2) })
    local cx = round(magI_rows / 2)
    local cy = round(magI_cols / 2)

    local q0 = magI:new({ 0, 0, cx, cy }) -- Top-Left - Create a ROI per quadrant
    local q1 = magI:new({ cx, 0, cx, cy }) -- Top-Right
    local q2 = magI:new({ 0, cy, cx, cy }) -- Bottom-Left
    local q3 = magI:new({ cx, cy, cx, cy }) -- Bottom-Right

    local tmp = q0:copy()                 -- swap quadrants (Top-Left with Bottom-Right)
    q3:copyTo(q0)
    tmp:copyTo(q3)

    tmp = q1:copy() -- swap quadrant (Top-Right with Bottom-Left)
    q2:copyTo(q1)
    tmp:copyTo(q2)
    -- [crop_rearrange]
    -- [normalize]
    cv.normalize(magI, magI, 0, 1, cv.NORM_MINMAX) -- Transform the matrix with float values into a
    -- viewable image form(float between values 0 and 1).
    -- [normalize]
    cv.imshow("Input Image", I)           -- Show the result
    cv.imshow("spectrum magnitude", magI)
    cv.waitKey()
end

main(arg)
