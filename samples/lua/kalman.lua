#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/kalman.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local __doc__ = [[
   Tracking of rotating point.
   Point moves in a circle and is characterized by a 1D state.
   state_k+1 = state_k + speed + process_noise N(0, 1e-5)
   The speed is constant.
   Both state and measurements vectors are 1D (a point angle),
   Measurement is the real state + gaussian noise N(0, 1e-1).
   The real and the measured points are connected with red line segment,
   the real and the estimated points are connected with yellow line segment,
   the real and the corrected estimated points are connected with green line segment.
   (if Kalman filter works correctly,
    the yellow segment should be shorter than the red one and
    the green segment should be shorter than the yellow one).
   Pressing any key (except ESC) will reset the tracking.
   Pressing ESC will stop the program.
]]

local cos = math.cos
local sin = math.sin
local sqrt = math.sqrt
local pi = math.pi

local int = opencv_lua.math.int
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated

local function main()
   local img_height = 500
   local img_width = 500
   local kalman = cv.KalmanFilter(2, 1, 0)

   local code = -1
   local num_circle_steps = 12
   while true do
      local img = cv.Mat.zeros(img_height, img_width, cv.CV_8UC3)
      local state = cv.Mat.createFromArray({ { 0.0 }, { (2 * pi) / num_circle_steps } }, cv.CV_32F) -- start state
      kalman.transitionMatrix = cv.Mat.createFromArray({ { 1., 1. }, { 0., 1. } }, cv.CV_32F)       -- F. input
      kalman.measurementMatrix = 1. * cv.Mat.eye(1, 2, cv.CV_32F)                                   -- H. input
      kalman.processNoiseCov = 1e-5 * cv.Mat.eye(2, cv.CV_32F)                                      -- Q. input
      kalman.measurementNoiseCov = 1e-1 * cv.Mat.ones({ 1, 1 }, cv.CV_32F)                          -- R. input
      kalman.errorCovPost = 1. * cv.Mat.eye(2, 2, cv.CV_32F)                                        -- P._k|k  KF state var
      kalman.statePost = 0.1 * cv.randn(2, 1)                                                       -- x^_k|k  KF state var

      while true do
         local function calc_point(angle)
            return { int(img_width / 2. + img_width / 3.0 * cos(angle)),
               int(img_height / 2. - img_width / 3.0 * sin(angle)) }
         end
         img = img * 0.2
         local state_angle = state:at(0, 0)
         local state_pt = calc_point(state_angle)
         -- advance Kalman filter to next timestep
         -- updates statePre, statePost, errorCovPre, errorCovPost
         -- k-> k+1, x'(k) = A*x(k)
         -- P'(k) = temp1*At + Q
         local prediction = kalman:predict()

         local predict_pt = calc_point(prediction:at(0, 0)) -- equivalent to calc_point(kalman.statePre:at(0, 0))
         -- generate measurement
         local measurement = kalman.measurementNoiseCov * cv.randn(1, 1)
         measurement = cv.dot(kalman.measurementMatrix, state) + measurement

         local measurement_angle = measurement:at(0, 0)
         local measurement_pt = calc_point(measurement_angle)

         -- correct the state estimates based on measurements
         -- updates statePost & errorCovPost
         kalman:correct(measurement)
         local improved_pt = calc_point(kalman.statePost:at(0, 0))

         -- plot points
         cv.drawMarker(img, measurement_pt, { 0, 0, 255 }, cv.MARKER_SQUARE, 5, 2)
         cv.drawMarker(img, predict_pt, { 0, 255, 255 }, cv.MARKER_SQUARE, 5, 2)
         cv.drawMarker(img, improved_pt, { 0, 255, 0 }, cv.MARKER_SQUARE, 5, 2)
         cv.drawMarker(img, state_pt, { 255, 255, 255 }, cv.MARKER_STAR, 10, 1)
         -- forecast one step
         cv.drawMarker(img, calc_point(cv.dot(kalman.transitionMatrix, kalman.statePost):at(0, 0)),
            { 255, 255, 0 }, cv.MARKER_SQUARE, 12, 1)

         cv.line(img, state_pt, measurement_pt, { 0, 0, 255 }, 1, cv.LINE_AA, 0) -- red measurement error
         cv.line(img, state_pt, predict_pt, { 0, 255, 255 }, 1, cv.LINE_AA, 0)   -- yellow pre-meas error
         cv.line(img, state_pt, improved_pt, { 0, 255, 0 }, 1, cv.LINE_AA, 0)    -- green post-meas error

         -- update the real process
         local process_noise = sqrt(kalman.processNoiseCov:at(0, 0)) * cv.randn(2, 1)
         state = cv.dot(kalman.transitionMatrix, state) + process_noise -- x_k+1 = F x_k + w_k

         cv.imshow("Kalman", img)
         code = cv.waitKey(1000)
         if code ~= -1 then
            break
         end
      end

      if code == 0x1b or code == string.byte("q") or code == string.byte("Q") then
         break
      end
   end

   print('Done')
end

print(__doc__)
main()
cv.destroyAllWindows()
