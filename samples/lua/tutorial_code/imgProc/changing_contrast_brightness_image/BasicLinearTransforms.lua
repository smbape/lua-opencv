package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/imgProc/changing_contrast_brightness_image/BasicLinearTransforms.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local function saturate_cast(value, minval, maxval)
    if value > maxval then return maxval end
    if value < minval then return minval end
    return value
end

-- Read image given by user
-- [basic-linear-transform-load]
-- parser = argparse.ArgumentParser(description='Code for Changing the contrast and brightness of an image! tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='lena.jpg')
-- args = parser.parse_args()

local args = {
    input = "lena.jpg",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local image = cv.imread(cv.samples.findFile(args.input))
if image:empty() then
    error('Could not open or find the image: ' .. args.input)
end
-- [basic-linear-transform-load]

-- [basic-linear-transform-output]
local new_image = cv.Mat.zeros(image:size(), image:type())
-- [basic-linear-transform-output]

-- [basic-linear-transform-parameters]
local alpha = 1.0 -- Simple contrast control
local beta = 0    -- Simple brightness control

-- Initialize values
print(' Basic Linear Transforms ')
print('-------------------------')

io.write('* Enter the alpha value [1.0-3.0]: ')
local alpha = io.read("*n") -- read a number

io.write('* Enter the beta value [0-100]: ')
local beta = io.read("*n") -- read a number
-- [basic-linear-transform-parameters]

local rows, cols, channels = image.rows, image.cols, image:channels()

-- make channels a new dimension to be able to do image(y, x, c)
image = image:reshape(1, { rows, cols, channels })
new_image = new_image:reshape(1, { rows, cols, channels })

-- Do the operation new_image(i,j) = alpha*image(i,j) + beta
-- Instead of these 'for' loops we could have used simply:
-- new_image = cv.convertScaleAbs(image, alpha=alpha, beta=beta)
-- but we wanted to show you how to access the pixels :)
-- [basic-linear-transform-operation]
for y = 0, rows - 1 do
    for x = 0, cols - 1 do
        for c = 0, channels - 1 do
            new_image:set(saturate_cast(alpha * image(y, x, c) + beta, 0, 255), y, x, c)
        end
    end
end

-- restore channels
image = image:reshape(channels, { rows, cols })
new_image = new_image:reshape(channels, { rows, cols })
-- [basic-linear-transform-operation]

-- [basic-linear-transform-display]
-- Show stuff
cv.imshow('Original Image', image)
cv.imshow('New Image', new_image)

-- Wait until user press some key
cv.waitKey()
-- [basic-linear-transform-display]
