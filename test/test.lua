opencv_lua = require("opencv_lua")
cv = opencv_lua.cv
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

function display(mat)
    print('dims: ' .. mat.dims .. ', rows: ' .. mat.rows .. ', cols: ' .. mat.cols)
end

CreatePet({name='Rex',species='Dog',breed='Irish Setter'})
--position does not matter here.

-- print(cv2.multi_tuple())
-- ret = cv2.multi_tuple()
-- x, y = cv2.multi_tuple()
-- assert(x == 10 and y == 'goodbye')

if package.config:sub(1,1) == '\\' then
    filename = 'E:\\development\\git\\node-autoit-opencv-com\\opencv-4.8.0-windows\\opencv\\sources\\samples\\data\\lena.jpg'
else
    filename = '/mnt/e/development/git/node-autoit-opencv-com/opencv-4.8.0-windows/opencv/sources/samples/data/lena.jpg'
end

img = cv.imread(filename)
cv.imshow("Image 1", img)
cv.waitKey()

-- print('dims: ' .. img.dims .. ', rows: ' .. img.rows .. ', cols: ' .. img.cols)
display(img)
-- cv2.proxy(display, img)

img = cv.Mat(img)
cv.imshow("Image 2", img)
cv.waitKey()

img = cv.Mat(300, 300, cv.CV_8UC3, {255.0, 0.0, 255.0})
cv.imshow("Image 3", img)
cv.waitKey()

img = cv.Mat(500, 400, opencv_lua.kwargs({
    type=cv.CV_8UC3,
    s={127.0, 255.0, 255.0}
}))
cv.imshow("Image 4", img)
cv.waitKey()

print('IMREAD_COLOR = ' .. cv["IMREAD_COLOR"])
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
cv.IMREAD_COLOR = 2
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
