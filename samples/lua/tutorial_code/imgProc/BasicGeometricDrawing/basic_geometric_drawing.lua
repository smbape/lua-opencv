#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/imgProc/BasicGeometricDrawing/basic_geometric_drawing.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

local W = 400
-- [my_ellipse]
local function my_ellipse(img, angle)
    local thickness = 2
    local line_type = 8

    cv.ellipse(img,
        { int(W / 2), int(W / 2) },
        { int(W / 4), int(W / 16) },
        angle,
        0,
        360,
        { 255, 0, 0 },
        thickness,
        line_type)
end
-- [my_ellipse]
-- [my_filled_circle]
local function my_filled_circle(img, center)
    local thickness = -1
    local line_type = 8

    cv.circle(img,
        center,
        int(W / 32),
        { 0, 0, 255 },
        thickness,
        line_type)
end
-- [my_filled_circle]
-- [my_polygon]
local function my_polygon(img)
    local line_type = 8

    -- Create some points
    local ppt = cv.Mat.createFromArray({ { W / 4, 7 * W / 8 }, { 3 * W / 4, 7 * W / 8 },
        { 3 * W / 4, 13 * W / 16 }, { 11 * W / 16, 13 * W / 16 },
        { 19 * W / 32, 3 * W / 8 }, { 3 * W / 4, 3 * W / 8 },
        { 3 * W / 4,   W / 8 }, { 26 * W / 40, W / 8 },
        { 26 * W / 40, W / 4 }, { 22 * W / 40, W / 4 },
        { 22 * W / 40, W / 8 }, { 18 * W / 40, W / 8 },
        { 18 * W / 40, W / 4 }, { 14 * W / 40, W / 4 },
        { 14 * W / 40, W / 8 }, { W / 4, W / 8 },
        { W / 4,      3 * W / 8 }, { 13 * W / 32, 3 * W / 8 },
        { 5 * W / 16, 13 * W / 16 }, { W / 4, 13 * W / 16 } }, cv.CV_32S)
    ppt = ppt:reshape(1, { int(ppt:total() / 2), 2 })
    cv.fillPoly(img, { ppt }, { 255, 255, 255 }, line_type)
    -- Only drawing the lines would be:
    -- cv.polylines(img, {ppt}, true, {255, 0, 255}, line_type)
end
-- [my_polygon]
-- [my_line]
local function my_line(img, pt1, pt2)
    local thickness = 2
    local line_type = 8

    cv.line(img,
        pt1,
        pt2,
        { 0, 0, 0 },
        thickness,
        line_type)
end
-- [my_line]
-- [create_images]
-- Windows names
local atom_window = "Drawing 1: Atom"
local rook_window = "Drawing 2: Rook"

-- Create black empty images
local atom_image = cv.Mat.zeros(W, W, cv.CV_8UC3)
local rook_image = cv.Mat.zeros(W, W, cv.CV_8UC3)
-- [create_images]
-- [draw_atom]
-- 1. Draw a simple atom:
-- -----------------------

-- 1.a. Creating ellipses
my_ellipse(atom_image, 90)
my_ellipse(atom_image, 0)
my_ellipse(atom_image, 45)
my_ellipse(atom_image, -45)

-- 1.b. Creating circles
my_filled_circle(atom_image, { int(W / 2), int(W / 2) })
-- [draw_atom]
-- [draw_rook]

-- 2. Draw a rook
-- ------------------
-- 2.a. Create a convex polygon
my_polygon(rook_image)
-- [rectangle]
-- 2.b. Creating rectangles
cv.rectangle(rook_image,
    { 0, int(7 * W / 8) },
    { W, W },
    { 0, 255, 255 },
    -1,
    8)
-- [rectangle]

--  2.c. Create a few lines
my_line(rook_image, { 0, int(15 * W / 16) }, { W, int(15 * W / 16) })
my_line(rook_image, { int(W / 4), int(7 * W / 8) }, { int(W / 4), W })
my_line(rook_image, { int(W / 2), int(7 * W / 8) }, { int(W / 2), W })
my_line(rook_image, { int(3 * W / 4), int(7 * W / 8) }, { int(3 * W / 4), W })
-- [draw_rook]
cv.imshow(atom_window, atom_image)
cv.moveWindow(atom_window, 0, 200)
cv.imshow(rook_window, rook_image)
cv.moveWindow(rook_window, W, 200)

cv.waitKey(0)
cv.destroyAllWindows()
