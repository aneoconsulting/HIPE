# -*- coding: utf-8 -*-
"""
Class definition of color deconvolution
"""

import colorsys
from timeit import default_timer as timer
import numpy as np
from matplotlib import pyplot as plt
import math

# from keras import backend as K
# from keras.models import load_model
# from keras.layers import Input
from PIL import Image, ImageFont, ImageDraw

# from yolo3.model import yolo_eval, yolo_body, tiny_yolo_body
# from yolo3.utils import letterbox_image
import os
# from keras.utils import multi_gpu_model

# ANEO PACKAGE
from pydata import shapeData
from pydata import imageData
from pydata import imageArrayData
import pydata

import sys

sys.stdout = pydata.stdout()
sys.stderr = pydata.stderr()


def init_deconv():
    None


def deconv(frame):
    # image = Image.fromarray(frame.get())
    # shape = yolo.detect_image(image)
    # frame = plt.imread('RNA1.tif')
    frame = np.double(frame.get())
    row_frame, col_frame, di = frame.shape

    colorMap = np.array([[0.650, 0.704, 0.286], [0.268, 0.570, 0.776], [0, 0, 0]])

    row, col = colorMap.shape

    # judge if the image is RGB image
    dim = frame.ndim
    if dim != 3:
        os._exit(0)

    colorMapNor = np.zeros((row, col))

    for i in range(row - 1):
        div = np.sqrt(
            np.power(colorMap[i, 0], 2) + np.power(colorMap[i, 1], 2) + np.power(colorMap[i, 2], 2))  # equation(5) row1
        colorMapNor[i, :] = colorMap[i, :] / div  # array([0.26814752, 0.57031376, 0.77642715]) (3*1)

    if np.power(colorMapNor[0, 0], 2) + np.power(colorMapNor[1, 0], 2) > 1:
        colorMapNor[2, 0] = 0
    else:
        colorMapNor[2, 0] = np.sqrt(
            1 - np.power(colorMapNor[0, 0], 2) - np.power(colorMapNor[1, 0], 2))  # equation(5) row3

    if np.power(colorMapNor[0, 1], 2) + np.power(colorMapNor[1, 1], 2) > 1:
        colorMapNor[2, 1] = 0
    else:
        colorMapNor[2, 1] = np.sqrt(1 - np.power(colorMapNor[0, 1], 2) - np.power(colorMapNor[1, 1], 2))

    if np.power(colorMapNor[0, 2], 2) + np.power(colorMapNor[1, 2], 2) > 1:
        colorMapNor[2, 2] = 0
    else:
        colorMapNor[2, 2] = np.sqrt(1 - np.power(colorMapNor[0, 2], 2) - np.power(colorMapNor[1, 2], 2))

    div = np.sqrt(np.power(colorMapNor[2, 0], 2) + np.power(colorMapNor[2, 1], 2) + np.power(colorMapNor[2, 2], 2))
    colorMapNor[2, :] = colorMapNor[2, :] / div

    A = colorMapNor[1, 1] - colorMapNor[1, 0] * colorMapNor[0, 1] / colorMapNor[0, 0];
    V = colorMapNor[1, 2] - colorMapNor[1, 0] * colorMapNor[0, 2] / colorMapNor[0, 0];
    C = colorMapNor[2, 2] - colorMapNor[2, 1] * V / A + colorMapNor[2, 0] * \
        (V / A * colorMapNor[0, 1] / colorMapNor[0, 0] - colorMapNor[0, 2] / colorMapNor[0, 0])

    q = np.zeros((9, 1))
    q[2] = (-colorMapNor[2, 0] / colorMapNor[0, 0] - colorMapNor[2, 0] / A * \
            colorMapNor[1, 0] / colorMapNor[0, 0] * colorMapNor[0, 1] / colorMapNor[0, 0] + \
            colorMapNor[1, 1] / A * colorMapNor[1, 0] / colorMapNor[0, 0]) / C
    q[1] = -q[2] * V / A - colorMapNor[1, 0] / (colorMapNor[0, 0] * A)
    q[0] = 1.0 / colorMapNor[0, 0] - q[1] * colorMapNor[0, 1] / colorMapNor[0, 0] - q[2] * \
           colorMapNor[0, 2] / colorMapNor[0, 0]
    q[5] = (-colorMapNor[2, 1] / A + colorMapNor[2, 0] / A * colorMapNor[0, 1] / colorMapNor[0, 0]) / C
    q[4] = -q[5] * V / A + 1.0 / A
    q[3] = -q[4] * colorMapNor[0, 1] / colorMapNor[0, 0] - q[5] * colorMapNor[0, 2] / colorMapNor[0, 0]
    q[8] = 1.0 / C;
    q[7] = -q[8] * V / A;
    q[6] = -q[7] * colorMapNor[0, 1] / colorMapNor[0, 0] - q[8] * colorMapNor[0, 2] / colorMapNor[0, 0];

    frameR = frame[:, :, 0].T.flatten()
    frameG = frame[:, :, 1].T.flatten()
    frameB = frame[:, :, 2].T.flatten()

    # calculate and construct new image
    length = len(frameR)
    newImg = np.zeros((length, 3))

    for i in range(length):
        transR = -((255.0 * math.log((np.double(frameR[i] + 1)) / 255.0)) / math.log(255.0));
        transG = -((255.0 * math.log((np.double(frameG[i] + 1)) / 255.0)) / math.log(255.0));
        transB = -((255.0 * math.log((np.double(frameB[i] + 1)) / 255.0)) / math.log(255.0))

        for j in range(3):
            Rscaled = transR * q[j * 3];
            Gscaled = transG * q[j * 3 + 1];
            Bscaled = transB * q[j * 3 + 2];

            output = math.exp(-((Rscaled + Gscaled + Bscaled) - 255.0) * math.log(255.0) / 255.0);

            if output > 255:
                output = 255

            newImg[i, j] = output

    imgVR = np.reshape(newImg[:, 0], (row_frame, col_frame), order='F')
    imgVG = np.reshape(newImg[:, 1], (row_frame, col_frame), order='F')
    imgVB = np.reshape(newImg[:, 2], (row_frame, col_frame), order='F')

    red_channel_image = Image.fromarray(imgVR)

    # fig3 = plt.figure(1)
    # plt.imshow(red_channel_image)
    # plt.title('Red Channel Image')
    # red_channel_image.show()
    arrayImage = imageArrayData()
    arrayImage.add("imgVR", imgVR.astype("uint8"))
    arrayImage.add("imgVG", imgVG.astype("uint8"))
    arrayImage.add("imgVB", imgVB.astype("uint8"))
    return arrayImage


def test(img):
    # img = np.double(img.get())
    red_channel_image = Image.fromarray(img.get())

    # print(red_channel_image.getpixel((0, 0)))
    # help(red_channel_image)
    # print("size : " + str(img.get().shape[0]) + " x " + str(img.get().shape[1]))
    # fig3 = plt.figure(3)
    # red_channel_image.show()

    # plt.imshow(red_channel_image)
    # plt.title('Red Channel Image')
    # plt.show()

    return img.get()