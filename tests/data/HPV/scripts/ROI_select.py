import numpy as np
import cv2
import copy
from PIL import Image

# ANEO PACKAGE

# from pydata import shapeData
# from pydata import imageData
# from pydata import imageArrayData
# import pydata
#
# import sys
# sys.stdout = pydata.stdout()
# sys.stderr = pydata.stderr()

def ROI_select(img_original,original_img):
    resImg = np.uint8(original_img)
    row, col, dimen = img_original.shape
    fromCenter = False
    showCrosshair = False
    img = np.uint8(np.array(img_original))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    r = cv2.selectROI('Figure 0', img,showCrosshair, fromCenter)

    # construct a mask of ROI
    maskROI = np.zeros((row, col))
    maskROI[int(r[1]):int(r[1] + r[3] + 1), int(r[0]):int(r[0] + r[2] + 1)] = 1

    # the area of ROI
    areaROI = r[2] * r[3]
    imCrop = resImg[int(r[1]):int(r[1] + r[3]), int(r[0]):int(r[0] + r[2])]

    # display the region of interest
    imCrop = np.uint8(imCrop)
    cropROI = Image.fromarray(imCrop)

    ROIImg = []
    ROIImg = copy.deepcopy(cropROI)
    disResult = 3
    disReset = cropROI

    return disReset
