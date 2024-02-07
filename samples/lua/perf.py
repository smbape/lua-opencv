# PYTHONPATH="$(realpath opencv-4.9.0-windows/opencv/sources/samples/dnn)" samples/.venv/Scripts/python.exe

import numpy as np
import cv2 as cv


rows = 512
cols = 512
channels = 3
mat = (np.random.rand(rows, cols, channels) * 256).astype(np.uint8)
times = 5
total = mat.size * times

print("Expected Sum = %i" % (mat.sum() * times, ))

t = cv.getTickCount()
bin = np.zeros(( 20, 20 ), np.int32)
mag = np.zeros(( 20, 20 ), np.int32)
for i in range(1, 50000):
    bin_cells = bin[:10,:10], bin[10:,:10], bin[:10,10:], bin[10:,10:]
    mag_cells = mag[:10,:10], mag[10:,:10], mag[:10,10:], mag[10:,10:]
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
print("calling new %d times on a %ix%i matrix took %.6f seconds" % (50000 * 4 * 2, bin.shape[0], bin.shape[1], elapsed))

t = cv.getTickCount()
sum_ = 0
for i in range(0, rows):
    for j in range(0, cols):
        for k in range(0, channels):
            for l in range(0, times):
                sum_ = sum_ + mat[ i, j, k ]
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
print("mat[i, j, k] %i times took %.6f seconds. Sum = %i" % (total, elapsed, sum_))
