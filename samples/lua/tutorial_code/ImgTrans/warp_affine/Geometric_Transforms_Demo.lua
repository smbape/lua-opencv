package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ImgTrans/warp_affine/Geometric_Transforms_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [Load the image]
-- parser = argparse.ArgumentParser(description='Code for Affine Transformations tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='lena.jpg')
-- args = parser.parse_args()

local args = {
    input = 'lena.jpg',
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
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end
-- [Load the image]

-- [Set your 3 points to calculate the  Affine Transform]
local srcTri = cv.Mat.createFromArray({ { 0, 0 }, { src.cols - 1, 0 }, { 0, src.rows - 1 } }, cv.CV_32F)
local dstTri = cv.Mat.createFromArray(
{ { 0, src.cols * 0.33 }, { src.cols * 0.85, src.rows * 0.25 }, { src.cols * 0.15, src.rows * 0.7 } }, cv.CV_32F)
-- [Set your 3 points to calculate the  Affine Transform]

-- [Get the Affine Transform]
local warp_mat = cv.getAffineTransform(srcTri, dstTri)
-- [Get the Affine Transform]

-- [Apply the Affine Transform just found to the src image]
local warp_dst = cv.warpAffine(src, warp_mat, { src.cols, src.rows })
-- [Apply the Affine Transform just found to the src image]

-- Rotating the image after Warp

-- [Compute a rotation matrix with respect to the center of the image]
local center = { math.floor(warp_dst.cols / 2), math.floor(warp_dst.rows / 2) }
local angle = -50
local scale = 0.6
-- [Compute a rotation matrix with respect to the center of the image]

-- [Get the rotation matrix with the specifications above]
local rot_mat = cv.getRotationMatrix2D(center, angle, scale)
-- [Get the rotation matrix with the specifications above]

-- [Rotate the warped image]
local warp_rotate_dst = cv.warpAffine(warp_dst, rot_mat, { warp_dst.cols, warp_dst.rows })
-- [Rotate the warped image]

-- [Show what you got]
cv.imshow('Source image', src)
cv.imshow('Warp', warp_dst)
cv.imshow('Warp + Rotate', warp_rotate_dst)
-- [Show what you got]

-- [Wait until user exits the program]
cv.waitKey()
-- [Wait until user exits the program]
