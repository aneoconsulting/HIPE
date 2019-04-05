import numpy as np
import copy
from sklearn.cluster import KMeans
from math import ceil
from PIL import Image
import cv2

#ANEO PACKAGE

# from pydata import shapeData
# from pydata import imageData
# from pydata import imageArrayData
# import pydata
#
#
#
#
# import sys
# sys.stdout = pydata.stdout()
# sys.stderr = pydata.stderr()

def contour_detection(threshold,mask,imgMask,original_img):
    numCells = np.max(mask) + 1
    for i in range(2, numCells):  # assign the value from 2 to numCell to i (except numCell)
        objArea = np.sum(mask == i)
        if objArea > threshold:
            imgMask[imgMask != i] = 0
            num_clusters = ceil(objArea / threshold)  # ceil back to a int which is larger or same with (objArea / threshold)
            # imgMask = im.segmentConnectedCells(imgMask, num_clusters) # input a binary image with black background, bright objects
            c = np.where(imgMask > 0)  # output the value img>0, light area
            e = np.array([c[1], c[0]])  # column and row of objects
            e = e.T  # transposition
            row_m, col_m = imgMask.shape
            kmeans = KMeans(n_clusters=num_clusters, n_init=8, max_iter=200).fit(e)
            labels = kmeans.labels_
            labels = labels + 1
            for i in range(len(e)):
                m = e[i][0]
                n = e[i][1]
                imgMask[n][m] = labels[i]

            imgMask = np.int8(imgMask)

            for i in range(len(e)):
                m = e[i][0]
                n = e[i][1]
                if (m + 1) < col_m and (m - 1) >= 0 and (n + 1) < row_m and (n - 1) >= 0:
                    # 4 - connected component
                    if imgMask[n][m] < imgMask[n][m + 1]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n][m - 1]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n + 1][m]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n - 1][m]:
                        imgMask[n][m] = -1
                    # 8 - connected component
                    if imgMask[n][m] < imgMask[n - 1][m - 1]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n - 1][m + 1]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n + 1][m - 1]:
                        imgMask[n][m] = -1
                    if imgMask[n][m] < imgMask[n + 1][m + 1]:
                        imgMask[n][m] = -1
            imgMask = imgMask + 1
            imgMask[imgMask > 0] = 1

            mask = mask * imgMask
            imgMask = []
            imgMask = copy.deepcopy(mask)

    original_img[mask == 0] = [255, 0, 0]
    contours = copy.copy(original_img)
    contours = np.uint8(contours)
    contours = Image.fromarray(contours)
    contours_img = copy.deepcopy(original_img)

    return contours_img