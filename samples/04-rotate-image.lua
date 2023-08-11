require(string.gsub(arg[0], "[^/\\]+%.lua", "init"))

function example()
    local img = cv.imread(cv.samples.findFile("lena.jpg"))
    local angle = 20
    local scale = 1

    local size = { img.width, img.height }
    local center = { img.width / 2, img.height / 2 }
    local M = cv.getRotationMatrix2D(center, -angle, scale)
    local rotated = cv.warpAffine(img, M, size)

    cv.imshow("Rotation", rotated)
    cv.waitKey()
    cv.destroyAllWindows()
end

example()
