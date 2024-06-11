#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) ..
    arg[0]:gsub("[^/\\]+%.lua", '../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/digits.py
--]]

local __doc__ = [[
SVM and KNearest digit recognition.

Sample loads a dataset of handwritten digits from 'digits.png'.
Then it trains a SVM and KNearest classifiers on it and evaluates
their accuracy.

Following preprocessing is applied to the dataset:
 - Moment-based image deskew (see deskew())
 - Digit images are split into 4 10x10 cells and 16-bin
   histogram of oriented gradients is computed for each
   cell
 - Transform histograms to space with Hellinger metric (see [1] (RootSIFT))


[1] R. Arandjelovic, A. Zisserman
    "Three things everyone should know to improve object retrieval"
    http://www.robots.ox.ac.uk/~vgg/publications/2012/Arandjelovic12/arandjelovic12.pdf

Usage:
   digits.lua
]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed
local common = require("common")
local class, clock, mosaic = common.class, common.clock, common.mosaic
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local int = opencv_lua.math.int
local inspect = require("inspect")

local SZ = 20 -- size of each digit is SZ x SZ
local CLASS_N = 10
local DIGITS_FN = 'digits.png'

local function split2d(img, cell_size)
    local width, height = img.width, img.height
    local sx, sy = unpack(cell_size)

    local cells = {}
    for j = 1, height, sy do
        for i = 1, width, sx do
            cells[#cells + 1] = img:new({ i - 1, j - 1, sx, sy })
        end
    end
    return cells
end

local function load_digits(fn)
    local t = os.clock()
    fn = cv.samples.findFile(fn)
    print(string.format('loading "%s" ...', fn))
    local digits_img = cv.imread(fn, cv.IMREAD_GRAYSCALE)
    local digits = split2d(digits_img, { SZ, SZ })
    local labels = {}
    for i = 1, CLASS_N do
        for j = 1, #digits / CLASS_N do
            labels[#labels + 1] = i - 1
        end
    end
    print(string.format("load_digits took %.3f seconds", os.clock() - t))
    return digits, labels
end

local function deskew(img)
    local m = cv.moments(img)
    if math.abs(m['mu02']) < 1e-2 then
        return img.copy()
    end
    local skew = m['mu11'] / m['mu02']
    local M = cv.Mat.createFromArray({ { 1, skew, -0.5 * SZ * skew }, { 0, 1, 0 } }, cv.CV_32F)
    img = cv.warpAffine(img, M, { SZ, SZ }, opencv_lua.kwargs({ flags = bit.bor(cv.WARP_INVERSE_MAP, cv.INTER_LINEAR) }))
    return img
end

local KNearest = class({
    __init__ = function(self, ...)
        local args = { ... }
        local has_kwarg = opencv_lua.kwargs.isinstance(args[#args])
        local kwargs = has_kwarg and args[#args] or opencv_lua.kwargs()
        local usedkw = 0

        -- get argument k
        local k = 3
        local has_k = false
        if (not has_kwarg) or #args > 1 then
            -- positional parameter should not be a named parameter
            if has_kwarg and kwargs:has("k") then
                error('k was both specified as a Positional and NamedParameter')
            end
            has_k = #args >= 1
            if has_k then
                k = args[1]
            end
        elseif kwargs:has("k") then
            -- named parameter
            has_k = true
            k = kwargs:get("k")
            usedkw = usedkw + 1
        end

        if usedkw ~= kwargs:size() then
            error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
        end

        --- ====================== ---
        --- CODE LOGIC STARTS HERE ---
        --- ====================== ---
        self.k = k
        self.model = cv.ml.KNearest.create()
    end,

    train = function(self, samples, responses)
        local t = os.clock()
        if type(responses) == 'table' then
            responses = opencv_lua.VectorOfInt(responses)
        end
        self.model:train(samples, cv.ml.ROW_SAMPLE, responses)
        print(string.format("train took %.3f seconds", os.clock() - t))
    end,

    predict = function(self, samples)
        local _retval, results, _neigh_resp, _dists = self.model:findNearest(samples, self.k)
        return results:ravel():table()
    end,

    load = function(self, fn)
        self.model = cv.ml.KNearest.load(fn)
    end,

    save = function(self, fn)
        self.model:save(fn)
    end,
})

local SVM = class({
    __init__ = function(self, ...)
        local args = { ... }
        local has_kwarg = opencv_lua.kwargs.isinstance(args[#args])
        local kwargs = has_kwarg and args[#args] or opencv_lua.kwargs()
        local usedkw = 0

        -- get argument C
        local C = 1
        local has_C = false
        if (not has_kwarg) or #args > 1 then
            -- positional parameter should not be a named parameter
            if has_kwarg and kwargs:has("C") then
                error('C was both specified as a Positional and NamedParameter')
            end
            has_C = #args >= 1
            if has_C then
                C = args[1]
            end
        elseif kwargs:has("C") then
            -- named parameter
            has_C = true
            C = kwargs:get("C")
            usedkw = usedkw + 1
        end

        -- get argument gamma
        local gamma = 0.5
        local has_gamma = false
        if (not has_kwarg) or #args > 2 then
            -- positional parameter should not be a named parameter
            if has_kwarg and kwargs:has("gamma") then
                error('gamma was both specified as a Positional and NamedParameter')
            end
            has_gamma = #args >= 2
            if has_gamma then
                gamma = args[2]
            end
        elseif kwargs:has("gamma") then
            -- named parameter
            has_gamma = true
            gamma = kwargs:get("gamma")
            usedkw = usedkw + 1
        end

        if usedkw ~= kwargs:size() then
            error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
        end

        --- ====================== ---
        --- CODE LOGIC STARTS HERE ---
        --- ====================== ---
        self.model = cv.ml.SVM.create()
        self.model:setGamma(gamma)
        self.model:setC(C)
        self.model:setKernel(cv.ml.SVM.RBF)
        self.model:setType(cv.ml.SVM.C_SVC)
    end,

    train = function(self, samples, responses)
        local t = os.clock()
        if type(responses) == 'table' then
            responses = opencv_lua.VectorOfInt(responses)
        end
        self.model:train(samples, cv.ml.ROW_SAMPLE, responses)
        print(string.format("train took %.3f seconds", os.clock() - t))
    end,

    predict = function(self, samples)
        local _retval, results = self.model:predict(samples)
        return results:ravel():table()
    end,

    load = function(self, fn)
        self.model = cv.ml.SVM.load(fn)
    end,

    save = function(self, fn)
        self.model:save(fn)
    end,
})

local function evaluate_model(model, digits, samples, labels)
    local t = os.clock()
    local resp = model:predict(samples)

    local err = 0
    for i = 1, #resp do
        if resp[i] ~= labels[i] then
            err = err + 1
        end
    end
    err = err / #resp
    print(string.format('error: %.2f %%', err * 100))

    local confusion = cv.Mat.zeros({ 10, 10 }, cv.CV_32S):table()
    for k = 1, #resp do
        local i, j = labels[k] + INDEX_BASE, resp[k] + INDEX_BASE
        confusion[i][j] = confusion[i][j] + 1
    end
    confusion = cv.Mat.createFromArray(confusion, cv.CV_32S)
    print('confusion matrix:')
    print(confusion)
    print()

    local vis = {}
    for i = 1, #digits do
        local img = digits[i]
        img = cv.cvtColor(img, cv.COLOR_GRAY2BGR)
        if resp[i] ~= labels[i] then
            img[{ cv.Range.Ellipsis, { 0, 2 } }] = 0 -- img[...,:2] = 0
        end
        vis[#vis + 1] = img
    end
    vis = mosaic(25, vis)

    print(string.format("evaluate_model took %.3f seconds", os.clock() - t))
    return vis
end

local function preprocess_simple(digits)
    local img0 = digits[1]
    local channels, total = img0:channels(), img0:total() * #digits
    local rows = total / (SZ * SZ)
    return cv.Mat(digits):convertTo(cv.CV_32F):reshape(channels, rows) / 255.0
end

local function preprocess_hog(digits)
    local t_init = os.clock()
    local samples = {}
    -- local half_cell = SZ / 2 -- half_cell = 10

    local t_bin = 0
    local t_bin_cells = 0
    local t_hist = 0
    local t_transform = 0
    local t

    for i = 1, #digits do
        t = os.clock()
        local img = digits[i]
        local gx = cv.Sobel(img, cv.CV_32F, 1, 0)
        local gy = cv.Sobel(img, cv.CV_32F, 0, 1)
        local mag, ang = cv.cartToPolar(gx, gy)
        local bin_n = 16
        local bin = (bin_n * ang / (2 * math.pi) - 0.5):convertTo(cv.CV_32S) -- - 0.5 to prevent rounding, we want floor
        t_bin = t_bin + (os.clock() - t)

        t = os.clock()
        local bin_cells = {
            bin:new({ 0, 0, 10, 10 }),
            bin:new({ 10, 0, 10, 10 }),
            bin:new({ 0, 10, 10, 10 }),
            bin:new({ 10, 10, 10, 10 }),
        }

        local mag_cells = {
            mag:new({ 0, 0, 10, 10 }),
            mag:new({ 10, 0, 10, 10 }),
            mag:new({ 0, 10, 10, 10 }),
            mag:new({ 10, 10, 10, 10 }),
        }
        t_bin_cells = t_bin_cells + (os.clock() - t)

        t = os.clock()
        local hists = {}
        for i = 1, #bin_cells do
            -- hists[#hists + 1] = cv.bincount(bin_cells[i]:ravel(), nil, mag_cells[i]:ravel(), bin_n)
            hists[#hists + 1] = cv.bincount(bin_cells[i]:ravel(), opencv_lua.kwargs({
                weights = mag_cells[i]:ravel(),
                minlength = bin_n,
            }))
        end
        local hist = cv.hconcat(hists)
        t_hist = t_hist + (os.clock() - t)

        -- transform to Hellinger kernel
        t = os.clock()
        local eps = 1e-7
        hist = hist / (hist:sum()[1] + eps)
        hist = cv.sqrt(hist)
        hist = hist / (cv.norm(hist) + eps)
        t_transform = t_transform + (os.clock() - t)

        samples[#samples + 1] = hist
    end

    samples = cv.vconcat(samples):convertTo(cv.CV_32F)
    print(string.format("preprocess_hog %d took %.3f seconds", #digits, os.clock() - t_init))

    print(string.format("preprocess_hog t_bin took %.3f seconds", t_bin))
    print(string.format("preprocess_hog t_bin_cells took %.3f seconds", t_bin_cells))
    print(string.format("preprocess_hog t_hist took %.3f seconds", t_hist))
    print(string.format("preprocess_hog t_transform took %.3f seconds", t_transform))
    return samples
end

local function shuffle(digits, labels)
    local t = os.clock()
    local shuffled_indexes = {}
    for i = 1, #digits do
        shuffled_indexes[i] = i - INDEX_BASE;
    end
    shuffled_indexes = cv.randShuffle(opencv_lua.VectorOfInt(shuffled_indexes))

    local shuffled_digits = {}
    local shuffled_labels = {}

    for i = 1, #shuffled_indexes do
        shuffled_digits[shuffled_indexes[i] + INDEX_BASE] = digits[i];
        shuffled_labels[shuffled_indexes[i] + INDEX_BASE] = labels[i];
    end

    print(string.format("shuffle took %.3f seconds", os.clock() - t))
    return shuffled_digits, shuffled_labels
end

local function map(fn, tbl)
    local t = os.clock()
    local result = {}
    for i = 1, #tbl do
        result[i] = fn(tbl[i])
    end
    print(string.format("map took %.3f seconds", os.clock() - t))
    return result
end

local function slice(tbl, start)
    local result = {}
    for i = start, #tbl do
        result[#result + 1] = tbl[i]
    end
    return result
end

local function split(tbl, sep)
    local before = {}
    for i = 1, sep - 1 do
        before[#before + 1] = tbl[i]
    end

    local after = {}
    for i = sep, #tbl do
        after[#after + 1] = tbl[i]
    end

    return before, after
end

local function main()
    print(__doc__)

    local digits, labels = load_digits(DIGITS_FN)

    print('preprocessing...')
    -- shuffle digits
    digits, labels = shuffle(digits, labels)

    local digits2 = map(deskew, digits)
    local samples = preprocess_hog(digits2)

    local t = os.clock()
    local train_n = int(0.9 * #samples)
    cv.imshow('test set', mosaic(25, slice(digits, train_n + INDEX_BASE)))
    local digits_train, digits_test = split(digits2, train_n + INDEX_BASE)
    local samples_train = samples:new({ 0, 0, samples.width, train_n });
    local samples_test = samples:new({ 0, train_n, samples.width, samples.height - train_n });
    local labels_train, labels_test = split(labels, train_n + INDEX_BASE)
    print(string.format("preprocess samples, labels took %.3f seconds", os.clock() - t))

    local model, vis

    print('training KNearest...')
    model = KNearest(opencv_lua.kwargs({ k = 4 }))
    model:train(samples_train, labels_train)
    vis = evaluate_model(model, digits_test, samples_test, labels_test)
    cv.imshow('KNearest test', vis)
    -- print('saving KNearest as "digits_knearest.dat"...')
    -- model:save('digits_knearest.dat')

    print('training SVM...')
    model = SVM(opencv_lua.kwargs({ C = 2.67, gamma = 5.383 }))
    model:train(samples_train, labels_train)
    vis = evaluate_model(model, digits_test, samples_test, labels_test)
    cv.imshow('SVM test', vis)
    -- print('saving SVM as "digits_svm.dat"...')
    -- model:save('digits_svm.dat')

    cv.waitKey(0)
    cv.destroyAllWindows()
end

main()
