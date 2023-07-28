cv = require("opencv_lua")
obj = cv.test.new(24)
print(obj.value)
print(cv.getBuildInformation())

function CreatePet(options)
  local name=options.name
  local species=options.species
  local breed=options.breed
  print('Created a '..breed..' '..species..' named '..name)
end

function fn(mat)
    print('dims: ' .. mat.dims .. ', rows: ' .. mat.rows)
end

CreatePet({name='Rex',species='Dog',breed='Irish Setter'})
--position does not matter here.

print(cv.multi_tuple())
ret = cv.multi_tuple()
x, y = cv.multi_tuple()
assert(x == 10 and y == 'goodbye')

if package.config:sub(1,1) == '\\' then
    filename = 'E:\\development\\git\\node-autoit-opencv-com\\opencv-4.8.0-windows\\opencv\\sources\\samples\\data\\lena.jpg'
else
    filename = '/mnt/e/development/git/node-autoit-opencv-com/opencv-4.8.0-windows/opencv/sources/samples/data/lena.jpg'
end

img = cv.imread(filename)
cv.imshow("Image", img)
cv.waitKey()

fn(img)
cv.proxy(fn, img)

img = cv.Mat(img)
cv.imshow("Image", img)
cv.waitKey()

img = cv.Mat(300, 300, cv.CV_8UC3, {255.0, 0.0, 255.0})
cv.imshow("Image", img)
cv.waitKey()

img = cv.Mat(500, 400, cv.kwargs({
    type=cv.CV_8UC3,
    s={127.0, 255.0, 255.0}
}))
cv.imshow("Image", img)
cv.waitKey()

print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
cv.IMREAD_COLOR = 2
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
