package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/features2D/Homography/panorama_stitching_rotating_camera.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local function basicPanoramaStitching(img1Path, img2Path)
    local img1 = cv.imread(cv.samples.findFile(img1Path))
    local img2 = cv.imread(cv.samples.findFile(img2Path))

    -- [camera-pose-from-Blender-at-location-1]
    local c1Mo = cv.Mat.createFromArray({{0.9659258723258972, 0.2588190734386444, 0.0, 1.5529145002365112},
                     { 0.08852133899927139, -0.3303661346435547, -0.9396926164627075, -0.10281121730804443},
                     {-0.24321036040782928, 0.9076734185218811, -0.342020183801651, 6.130080699920654},
                     {0, 0, 0, 1}}, cv.CV_64F)
    -- [camera-pose-from-Blender-at-location-1]

    -- [camera-pose-from-Blender-at-location-2]
    local c2Mo = cv.Mat.createFromArray({{0.9659258723258972, -0.2588190734386444, 0.0, -1.5529145002365112},
                     {-0.08852133899927139, -0.3303661346435547, -0.9396926164627075, -0.10281121730804443},
                     {0.24321036040782928, 0.9076734185218811, -0.342020183801651, 6.130080699920654},
                     {0, 0, 0, 1}}, cv.CV_64F)
    -- [camera-pose-from-Blender-at-location-2]

    -- [camera-intrinsics-from-Blender]
    local cameraMatrix = cv.Mat.createFromArray({{700.0, 0.0, 320.0}, {0.0, 700.0, 240.0}, {0, 0, 1}}, cv.CV_64F)
    -- [camera-intrinsics-from-Blender]

    -- [extract-rotation]
    local R1 = c1Mo:new({ cv.Range(0, 3), cv.Range(0, 3) })
    local R2 = c2Mo:new({ cv.Range(0, 3), cv.Range(0, 3) })
    --[extract-rotation]

    -- [compute-rotation-displacement]
    local R_2to1 = R1 * R2:t()
    -- [compute-rotation-displacement]

    -- [compute-homography]
    local H = cameraMatrix * R_2to1 * cameraMatrix:inv()
    H = H / H(2, 2)
    -- [compute-homography]

    -- [stitch]
    local img_stitch = cv.warpPerspective(img2, H, { img2.width * 2, img2.height })
    img1:copyTo(img_stitch:new({ 0, 0, img1.width, img1.height }))
    -- [stitch]

    local img_space = cv.Mat.zeros({ 50, img1.height }, cv.CV_8UC3)
    local img_compare = cv.hconcat({ img1, img_space, img2 })

    cv.imshow("Final", img_compare)
    cv.imshow("Panorama", img_stitch)
    cv.waitKey(0)
end

local function main()
    -- parser = argparse.ArgumentParser(description="Code for homography tutorial. Example 5: basic panorama stitching from a rotating camera.")
    -- parser.add_argument("-I1","--image1", help = "path to first image", default="Blender_Suzanne1.jpg")
    -- parser.add_argument("-I2","--image2", help = "path to second image", default="Blender_Suzanne2.jpg")
    -- args = parser.parse_args()

    local args = {
        image1 = "Blender_Suzanne1.jpg",
        image2 = "Blender_Suzanne2.jpg",
    }

    local aliases = {}
    aliases["-I1"] = "image1"
    aliases["-I2"] = "image2"

    for i=1, #arg, 2 do
        local name = arg[i]
        if name:sub(1,2) == "--" then name = name:sub(3) end
        if aliases[name] ~= nil then name = aliases[name] end
        if args[name] == nil or i == #arg then
            error('unexpected argument ' .. name)
        end
        args[name] = arg[i + 1]
    end

    print("Panorama Stitching Started")
    basicPanoramaStitching(args.image1, args.image2)
    print("Panorama Stitching Completed Successfully")
end

main()
