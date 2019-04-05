import sys
import os
import delete_noise
from delete_noise import delete_noise
from color_decon import color_deconvolution
from back_foreground import back_foreground
from watershed_cell import watershed_cell
from contour_detection import contour_detection
from labeled_mRNA import labeled_mRNA
from ROI_select import ROI_select

from PIL import Image
from matplotlib import pyplot as plt
import matplotlib.pylab as plb
import skimage.measure as skp
from sklearn.cluster import KMeans
import copy
import numpy as np
import math

def main():
    original_img = plt.imread('RNA1.tif')
    original_img.setflags(write=1)
    print(original_img.flags)
    img_original = copy.deepcopy(original_img)

    (imgVR,imgVG,imgVB) = color_deconvolution(original_img)
    chann1 = np.uint8(imgVR)
    chann2 = np.uint8(imgVG)

    bluishOpen = delete_noise(original_img, chann1)
    markers = back_foreground(bluishOpen)
    mask, threshold, imgMask = watershed_cell(original_img, markers)
    contours_img = contour_detection(threshold,mask,imgMask,original_img)
    fig0 = plt.figure()
    plt.imshow(contours_img)
    plt.title('Contours Image')
    plt.show()

    ROIImg,resImg = labeled_mRNA(chann2, original_img, threshold, mask, imgMask)
    fig1 = plt.figure()
    plt.imshow(ROIImg)
    plt.title('Processed Image')
    plt.show()

    disReset = ROI_select(img_original,original_img)
    fig2 = plt.figure()
    plt.imshow(disReset)
    plt.title('ROI Image')
    plt.show()


    # print("hello")

if __name__ == "__main__":
    main()
