import numpy as np
import copy
import matplotlib.pylab as plb
from sklearn.cluster import KMeans
import math
from PIL import Image
import skimage.measure as skp
import cv2

# ANEO PACKAGE

# from pydata import shapeData
# from pydata import imageData
# from pydata import imageArrayData
# import pydata
#
# import sys
# sys.stdout = pydata.stdout()
# sys.stderr = pydata.stderr()

def labeled_mRNA(chann2,original_img,threshold,mask,imgMask):
    row, col, dimen = original_img.shape
    mRNA_channel = copy.copy(original_img)
    # in labeled image, 0 as edge,1 as background, change to 0 as background, 1...end as cells
    labelCells = copy.deepcopy(mask)
    labelCells[labelCells == 1] = 0
    labelCells[labelCells > 0] = 1
    labelCells = skp.label(labelCells, connectivity=1)
    numCells = np.max(labelCells)

    # calculate the area of cells
    cells1 = copy.deepcopy(labelCells)
    cells1[cells1 > 0] = 1
    areaCells = np.sum(cells1 == 1)

    # process the RNA molecules
    ret2, brownish = cv2.threshold(chann2, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)  # brownish 带棕点的
    # use Otsu algorithm usually set thresh is 0 and maxval is 255
    brownish = (255 - brownish) * (1 / 255)
    # brownish = (255 - brownish)
    # numRNA, areaRNA, labelRNA = im.countNum(brownish)
    brownish = plb.array(brownish)
    labelRNA = skp.label(brownish, connectivity=1)
    numRNA = np.max(labelRNA)
    props = skp.regionprops(labelRNA)
    areaRNA = 0

    for i in range(len(props)):
        areaRNA = areaRNA + props[i].area

    labelCellAndRNA = labelCells * brownish

    Positive_cells = np.int16(np.unique(labelCellAndRNA))
    Positive_cells = Positive_cells.tolist()
    Positive_cells.remove(0)

    # calculate the area of RNA in nuclei
    cells2 = copy.deepcopy(labelCellAndRNA)
    cells2[cells2 > 0] = 1
    areaRNAInside = np.sum(cells2 == 1)

    # number of RNA inside nuclei
    RNAInside = cells1 * labelRNA
    numRNAInside = len(np.unique(RNAInside)) - 1

    # the area of RNA in cytoplasm
    cells3 = brownish - cells2
    areaOfRNAInCyto = areaRNA - areaRNAInside

    # set RNA molecule inside nuclei as yellow,out of nuclei as green
    original_img[cells2 == 1] = [255, 255, 0]
    original_img[cells3 == 1] = [0, 255, 0]
    mRNA_channel[cells3 == 1] = [0, 255, 0]

    labelCellAndRNA = np.int16(labelCellAndRNA)

    # get Centroid
    # rowCenter, colCenter = im.getCentroid(labelCellAndRNA)
    props = skp.regionprops(labelCellAndRNA)

    rowCenter = [[] for i in range(len(props))]
    colCenter = [[] for i in range(len(props))]
    for i in range(len(props)):
        rowCenter[i].append(props[i].centroid[0])
        colCenter[i].append(props[i].centroid[1])

    rowCenter = np.int16(np.ceil(np.array(rowCenter)))
    colCenter = np.int16(np.ceil(np.array(colCenter)))

    numStain = len(rowCenter)
    img_circle = np.zeros((row, col))

    for i in range(numStain):
        rc = np.int(rowCenter[i])
        cc = np.int(colCenter[i])
        r = 5

        # setCirclecells
        #     im.setCircleCells(img_circle, rowCenter=rc, colCenter=cc, r=5, row=row, col=col)
        low_range_x = rc - r
        upper_range_x = rc + r + 1
        low_range_y = cc - r
        upper_range_y = cc + r + 1

        if low_range_x < 0:
            low_range_x = 0
        if low_range_y < 0:
            low_range_y = 0
        if upper_range_x >= row:
            upper_range_x = row
        if upper_range_y >= col:
            upper_range_y = col

        for i in range(low_range_x, upper_range_x):
            for j in range(low_range_y, upper_range_y):
                if math.pow(i - rc, 2) + math.pow(j - cc, 2) <= r * r:
                    imgMask[i, j] = 1

    mRNA_channel = np.uint8(mRNA_channel)
    mRNA_channel = Image.fromarray(mRNA_channel)

    areaOfImg = row * col

    resImg = np.uint8(original_img)
    result = Image.fromarray(resImg)
    ROIImg = result

    return ROIImg,resImg