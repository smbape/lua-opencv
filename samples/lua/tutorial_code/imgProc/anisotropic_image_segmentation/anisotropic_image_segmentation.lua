#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://docs.opencv.org/4.x/d4/d70/tutorial_anisotropic_image_segmentation_by_a_gst.html
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/anisotropic_image_segmentation/anisotropic_image_segmentation.py
    https://github.com/opencv/opencv/blob/4.9.0/samples/cpp/tutorial_code/ImgProc/anisotropic_image_segmentation/anisotropic_image_segmentation.cpp
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

cv.samples.addSamplesDataSearchPath(opencv_lua.fs_utils.findFile(
    "doc/tutorials/imgproc/anisotropic_image_segmentation/images", opencv_lua.kwargs({
        hints = {
            "out/build/x64-Debug/opencv/opencv-src",
            "out/build/x64-Release/opencv/opencv-src",
            "out/build/Linux-GCC-Debug/opencv/opencv-src",
            "out/build/Linux-GCC-Release/opencv/opencv-src",
            "out/prepublish/luajit-2.1/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.4/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.3/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.2/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.1/lua-opencv/build.luarocks/opencv/opencv-src",
            "opencv",
        }
    })))

local W = 52       -- window size is WxW
local C_Thr = 0.43 -- threshold for coherency
local LowThr = 35  -- threshold1 for orientation, it ranges from 0 to 180
local HighThr = 57 -- threshold2 for orientation, it ranges from 0 to 180

-- [calcGST]
-- [calcJ_header]
-- [calcGST_proto]
local function calcGST(inputIMG, w)
    -- [calcGST_proto]
    local img = inputIMG:convertTo(cv.CV_32F)

    -- GST components calculation (start)
    -- J =  (J11 J12; J12 J22) - GST
    local imgDiffX = cv.Sobel(img, cv.CV_32F, 1, 0, nil, 3)
    local imgDiffY = cv.Sobel(img, cv.CV_32F, 0, 1, nil, 3)
    local imgDiffXY = cv.multiply(imgDiffX, imgDiffY)
    -- [calcJ_header]

    local imgDiffXX = cv.multiply(imgDiffX, imgDiffX)
    local imgDiffYY = cv.multiply(imgDiffY, imgDiffY)

    local J11 = cv.boxFilter(imgDiffXX, cv.CV_32F, { w, w })
    local J22 = cv.boxFilter(imgDiffYY, cv.CV_32F, { w, w })
    local J12 = cv.boxFilter(imgDiffXY, cv.CV_32F, { w, w })
    -- GST components calculations (stop)

    -- eigenvalue calculation (start)
    -- lambda1 = 0.5*(J11 + J22 + sqrt((J11-J22)^2 + 4*J12^2))
    -- lambda2 = 0.5*(J11 + J22 - sqrt((J11-J22)^2 + 4*J12^2))
    local tmp1 = J11 + J22
    local tmp2 = J11 - J22
    tmp2 = cv.multiply(tmp2, tmp2)
    local tmp3 = cv.multiply(J12, J12)
    local tmp4 = cv.sqrt(tmp2 + 4.0 * tmp3)

    local lambda1 = 0.5 * (tmp1 + tmp4) -- biggest eigenvalue
    local lambda2 = 0.5 * (tmp1 - tmp4) -- smallest eigenvalue
    -- eigenvalue calculation (stop)

    -- Coherency calculation (start)
    -- Coherency = (lambda1 - lambda2)/(lambda1 + lambda2)) - measure of anisotropism
    -- Coherency is anisotropy degree (consistency of local orientation)
    local imgCoherencyOut = cv.divide(lambda1 - lambda2, lambda1 + lambda2)
    -- Coherency calculation (stop)

    -- orientation angle calculation (start)
    -- tan(2*Alpha) = 2*J12/(J22 - J11)
    -- Alpha = 0.5 atan2(2*J12/(J22 - J11))
    local imgOrientationOut = cv.phase(J22 - J11, 2.0 * J12, kwargs({ angleInDegrees = true }))
    local imgOrientationOut = 0.5 * imgOrientationOut
    -- orientation angle calculation (stop)

    return imgCoherencyOut, imgOrientationOut
end
-- [calcGST]

-- parser = argparse.ArgumentParser(description='Code for Anisotropic image segmentation tutorial.')
-- parser.add_argument('-i', '--input', help='Path to input image.', required=True)
-- args = parser.parse_args()

local args = {
    input = "gst_input.jpg",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local imgIn = cv.imread(cv.samples.findFile(args.input), cv.IMREAD_GRAYSCALE)
if imgIn:empty() then
    error(string.format('Could not open or find the image: %s', args.input))
end

-- [main_extra]
-- [main]
local imgCoherency, imgOrientation = calcGST(imgIn, W)

-- [thresholding]
local imgCoherencyBin = cv.inRange(imgCoherency, C_Thr, 255)
local imgOrientationBin = cv.inRange(imgOrientation, LowThr, HighThr)
-- [thresholding]

-- [combining]
local imgBin = cv.bitwise_and(imgCoherencyBin, imgOrientationBin)
-- [combining]
-- [main]

imgCoherency = cv.normalize(imgCoherency, nil, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX, dtype = cv
.CV_32F }))
imgOrientation = cv.normalize(imgOrientation, nil, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX,
    dtype = cv.CV_32F }))

cv.imshow("Original", imgIn)
cv.imshow("Result", 0.5 * imgIn + 0.5 * imgBin)
cv.imshow("Coherency", imgCoherency);
cv.imshow("Orientation", imgOrientation);
cv.waitKey(0)
-- [main_extra]
