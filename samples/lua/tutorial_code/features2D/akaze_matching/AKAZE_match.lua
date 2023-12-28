package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/features2D/akaze_matching/AKAZE_match.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local INDEX_BASE = 1 -- lua is 1-based indexed

-- [load]
-- parser = argparse.ArgumentParser(description='Code for AKAZE local features matching tutorial.')
-- parser.add_argument('--input1', help='Path to input image 1.', default='graf1.png')
-- parser.add_argument('--input2', help='Path to input image 2.', default='graf3.png')
-- parser.add_argument('--homography', help='Path to the homography matrix.', default='H1to3p.xml')
-- args = parser.parse_args()

local args = {
    input1 = "graf1.png",
    input2 = "graf3.png",
    homography = "H1to3p.xml",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local img1 = cv.imread(cv.samples.findFile(args.input1), cv.IMREAD_GRAYSCALE)
local img2 = cv.imread(cv.samples.findFile(args.input2), cv.IMREAD_GRAYSCALE)
if img1:empty() or img2:empty() then
    error('Could not open or find the images!')
end

local fs = cv.FileStorage(cv.samples.findFile(args.homography), cv.FileStorage.READ)
local homography = fs:getFirstTopLevelNode():mat()
-- [load]

-- [AKAZE]
local akaze = cv.AKAZE.create()
local kpts1, desc1 = akaze:detectAndCompute(img1, nil)
local kpts2, desc2 = akaze:detectAndCompute(img2, nil)
-- [AKAZE]

-- [2-nn matching]
local matcher = cv.DescriptorMatcher.create(cv.DescriptorMatcher.BRUTEFORCE_HAMMING)
local nn_matches = matcher:knnMatch(desc1, desc2, 2)
-- [2-nn matching]

-- [ratio test filtering]
local t = os.clock()
local matched1 = {}
local matched2 = {}
local nn_match_ratio = 0.8 -- Nearest neighbor matching ratio
for i, v in nn_matches:pairs() do
    local m, n = v[0 + INDEX_BASE], v[1 + INDEX_BASE]
    if m.distance < nn_match_ratio * n.distance then
        -- lua use 1-based index array
        -- and since kpts1 and kpts2 are arrays
        -- use +1 to match 1-based index array
        matched1[#matched1 + 1] = kpts1[m.queryIdx + 1]
        matched2[#matched2 + 1] = kpts2[m.trainIdx + 1]
    end
end
print(string.format("ratio test filtering: %.3f s", os.clock() - t))
-- [ratio test filtering]

-- [homography check]
local t = os.clock()
local inliers1 = {}
local inliers2 = {}
local good_matches = {}
local inlier_threshold = 2.5 -- Distance threshold to identify inliers with homography check
for i, m in ipairs(matched1) do
    local col = cv.Mat.ones(3, 1, cv.CV_64F)
    col[0] = m.pt[0 + INDEX_BASE]
    col[1] = m.pt[1 + INDEX_BASE]

    col = cv.gemm(homography, col, 1.0, nil, 0.0)
    col = col:convertTo(-1, kwargs({ alpha = 1 / col[2] }))
    local dist = math.sqrt(((col[0] - matched2[i].pt[0 + INDEX_BASE]) ^ 2) +
        ((col[1] - matched2[i].pt[1 + INDEX_BASE]) ^ 2))

    if dist < inlier_threshold then
        good_matches[#good_matches + 1] = cv.DMatch(#inliers1, #inliers2, 0)
        inliers1[#inliers1 + 1] = matched1[i]
        inliers2[#inliers2 + 1] = matched2[i]
    end
end
print(string.format("homography check: %.3f s", os.clock() - t))
-- [homography check]

-- [draw final matches]
local res = cv.Mat(math.max(img1.rows, img2.rows), img1.cols + img2.cols, cv.CV_8UC3)
cv.drawMatches(img1, inliers1, img2, inliers2, good_matches, res)
-- cv.imwrite("akaze_result.png", res)

local inlier_ratio = #inliers1 / #matched1
print('A-KAZE Matching Results')
print('*******************************')
print('-- Keypoints 1:                        \t', #kpts1)
print('-- Keypoints 2:                        \t', #kpts2)
print('-- Matches:                            \t', #matched1)
print('-- Inliers:                            \t', #inliers1)
print('-- Inliers Ratio:                      \t', inlier_ratio)

cv.imshow('result', res)
cv.waitKey()
-- [draw final matches]
