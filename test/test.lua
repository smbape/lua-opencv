local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

-- cv2 = opencv_lua.cv2
-- obj = cv.test.new(24)
-- print(obj.value)
print(cv.getBuildInformation())

function CreatePet(options)
  local name=options.name
  local species=options.species
  local breed=options.breed
  print('Created a '..breed..' '..species..' named '..name)
end

function Display(mat)
    print('dims: ' .. mat.dims .. ', rows: ' .. mat.rows .. ', cols: ' .. mat.cols)
end

CreatePet({name='Rex',species='Dog',breed='Irish Setter'})
--position does not matter here.

-- print(cv2.multi_tuple())
-- ret = cv2.multi_tuple()
-- x, y = cv2.multi_tuple()
-- assert(x == 10 and y == 'goodbye')

local sysPath = {
    dirname = function (file)
        local index;

        for i=#file,1,-1 do
            local c = file:sub(i,i)
            if c == "/" or c == "\\" then
                if i == 1 then return c end
                return file:sub(1, i - 1)
            end
        end

        if #file == 0 or (file:find(":") == nil and not (file:sub(1,1) == "/")) then
            return "."
        end

        return file
    end
}

local OPENCV_SAMPLES_DATA_PATH = sysPath.dirname(opencv_lua.fs_utils.findFile("samples/data/lena.jpg", opencv_lua.kwargs({
    hints={
        ".",
        "out/build/x64-Debug/opencv/opencv-src",
        "out/build/x64-Release/opencv/opencv-src",
        "out/build/Linux-GCC-Debug/opencv/opencv-src",
        "out/build/Linux-GCC-Release/opencv/opencv-src",
    }
})))
cv.samples.addSamplesDataSearchPath(OPENCV_SAMPLES_DATA_PATH)

local filename = cv.samples.findFile("lena.jpg")
local img = cv.imread(filename)
cv.imshow("Image 1", img)
cv.waitKey()

-- print('dims: ' .. img.dims .. ', rows: ' .. img.rows .. ', cols: ' .. img.cols)
Display(img)
-- cv2.proxy(display, img)

img = cv.Mat(img)
cv.imshow("Image 2", img)
cv.waitKey()

img = cv.Mat(300, 300, cv.CV_8UC3, {255.0, 0.0, 255.0})
cv.imshow("Image 3", img)
cv.waitKey()

img = cv.Mat(500, 400, kwargs({
    type=cv.CV_8UC3,
    s={127.0, 255.0, 255.0}
}))
cv.imshow("Image 4", img)
cv.waitKey()

print('IMREAD_COLOR = ' .. cv["IMREAD_COLOR"])
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
cv.IMREAD_COLOR = 2
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
