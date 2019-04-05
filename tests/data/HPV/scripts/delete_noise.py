import colorsys
from timeit import default_timer as timer
import numpy as np
from matplotlib import pyplot as plt
import math
import matplotlib.pylab as plb
import skimage.measure as skp
# from keras import backend as K
# from keras.models import load_model
# from keras.layers import Input
from PIL import Image, ImageFont, ImageDraw

# from yolo3.model import yolo_eval, yolo_body, tiny_yolo_body
# from yolo3.utils import letterbox_image
import os
# from keras.utils import multi_gpu_model

# ANEO PACKAGE

# from pydata import shapeData
# from pydata import imageData
# from pydata import imageArrayData
# import pydata
import cv2




import sys
# sys.stdout = pydata.stdout()
# sys.stderr = pydata.stderr()
numOfNoise = 10

def delete_noise(original_img,chann1):
    row, col, dimen = original_img.shape  # dimen =  dimension
    ret1, bluish = cv2.threshold(chann1, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)  # bluish 带蓝点的 输出值

    bluish = 255 - bluish
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2, 2))  # Morphological operational element 椭圆
    bluishOpen = cv2.morphologyEx(bluish, cv2.MORPH_OPEN, kernel,iterations=2)  # open operation, 2 iteration operations
    # bluishOpen = im.delNoise(bluishOpen, 10)  # delete noise with open operation

    bluishOpen = plb.array(bluishOpen)
    label_img = skp.label(bluishOpen, connectivity=1)
    props = skp.regionprops(label_img)

    for i in range(len(props)):
        if props[i].area < numOfNoise:  # 10 is an original value can be changed
            bluishOpen[label_img == i + 1] = 0

    return bluishOpen
