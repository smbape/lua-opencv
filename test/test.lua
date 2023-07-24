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

CreatePet({name='Rex',species='Dog',breed='Irish Setter'})
--position does not matter here.

if package.config:sub(1,1) == '\\' then
    filename = 'E:\\development\\git\\node-autoit-opencv-com\\opencv-4.8.0-windows\\opencv\\sources\\samples\\data\\lena.jpg'
else
    filename = '/mnt/e/development/git/node-autoit-opencv-com/opencv-4.8.0-windows/opencv/sources/samples/data/lena.jpg'
end

img = cv.imread(filename)
cv.imshow("Image", img)
cv.waitKey()

img = cv.Mat(img)
cv.imshow("Image", img)
cv.waitKey()

print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
cv.IMREAD_COLOR = 2
print('IMREAD_COLOR = ' .. cv.IMREAD_COLOR)
