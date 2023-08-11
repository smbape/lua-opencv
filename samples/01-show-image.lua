require(string.gsub(arg[0], "[^/\\]+%.lua", "init"))

function example()
    local img = cv.imread(cv.samples.findFile("lena.jpg"))
    cv.imshow("Image", img)
    cv.waitKey()
    cv.destroyAllWindows()
end

example()
