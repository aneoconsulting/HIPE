
import numpy as np
import cv2

# from keras import backend as K
# from keras.models import load_model
# from keras.layers import Input

# from yolo3.model import yolo_eval, yolo_body, tiny_yolo_body
# from yolo3.utils import letterbox_image
import os
# from keras.utils import multi_gpu_model

# ANEO PACKAGE

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

def back_foreground(bluishOpen):
    # Start to do segmentation
    # sure the background area
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2, 2))
    sure_bg = cv2.dilate(bluishOpen, kernel, iterations=3)  # 在bluishon的基础上膨胀操作 3次迭代

    # finding sure foreground area
    dist_transform = cv2.distanceTransform(bluishOpen, cv2.DIST_L2, 5)
    thres3, sure_fg = cv2.threshold(dist_transform, 0.25 * dist_transform.max(), 255, 0)

    # finding unknown region
    sure_fg = np.uint8(sure_fg)
    unknown = cv2.subtract(sure_bg, sure_fg)  # background image - forhead image can get unknown region

    # marker labelling
    ret, markers = cv2.connectedComponents(sure_fg)  # define unknown region with 0
    # label the background as 1, so that all markers should be added one, to make sure background not 0 is one
    markers = markers + 1

    # label the unknown region as 0, so that we can change its value to segment the cells in the next step
    markers[unknown == 255] = 0

    return markers