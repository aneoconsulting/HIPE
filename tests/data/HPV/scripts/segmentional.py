import numpy as np
import cv2
import color_decon as colde
import os
import copy
import math
from math import pi, ceil

from PIL import Image
from matplotlib import pyplot as plt
import matplotlib.pylab as plb
import skimage.measure as skp
from sklearn.cluster import KMeans

original_img = plt.imread('RNA1.tif')
img_original = copy.deepcopy(original_img)
original_img.setflags(write=1)
print(original_img.flags)

mRNA_channel = copy.copy(original_img)

# do color deconvolution
chann1, chann2, chann3 = colde.color_deconvolution(original_img)
chann1 = np.uint8(chann1)
chann2 = np.uint8(chann2)

# SelectIMageByLabel
ch1 = Image.fromarray(chann1)
ch2 = Image.fromarray(chann2)

########
# delete noise
row, col, dimen = original_img.shape # dimen =  dimension
ret1, bluish = cv2.threshold(chann1, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)  # bluish 带蓝点的 输出值

bluish = 255 - bluish
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (2, 2))  # Morphological operational element 椭圆
bluishOpen = cv2.morphologyEx(bluish, cv2.MORPH_OPEN, kernel, iterations=2)  # open operation, 2 iteration operations
# bluishOpen = im.delNoise(bluishOpen, 10)  # delete noise with open operation

######## im.delnoise
bluishOpen = plb.array(bluishOpen)
label_img = skp.label(bluishOpen,connectivity = 1)
props = skp.regionprops(label_img)

for i in range(len(props)):
    if props[i].area < 10: # 10 is an original value can be changed
        bluishOpen[label_img == i+1] = 0


# Start to do segmentation
# sure the background area
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

# use watershed to segment the cells
mask = cv2.watershed(original_img, markers)
# set the edge as red color
mask[mask == -1] = 0

imgMask = copy.deepcopy(mask)
# value = cellRadius.value()
value = 8 #accroding to gui interactive window, the original value of detect cell radiu is 8 pixels

threshold = value * value * pi

# contour detection
# img[mask == 0] = [255, 0, 0]
numCells = np.max(mask) + 1
for i in range(2, numCells): # assign the value from 2 to numCell to i (except numCell)
    objArea = np.sum(mask == i)
    if objArea > threshold:
        imgMask[imgMask != i] = 0
        num_clusters = ceil(objArea / threshold) # ceil back to a int which is larger or same with (objArea / threshold)
    #imgMask = im.segmentConnectedCells(imgMask, num_clusters) # input a binary image with black background, bright objects
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
labelRNA = skp.label(brownish,connectivity = 1)
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



contours_posi_img = contours_img
mRNA_channel = np.uint8(mRNA_channel)
mRNA_channel = Image.fromarray(mRNA_channel)

areaOfImg = row * col

resImg = np.uint8(original_img)
result = Image.fromarray(resImg)
disReset = copy.copy(result)
disResult = 0
ROIImg = []
ROIImg = result

fig1 = plt.figure()
plt.imshow(ROIImg)
plt.title('Processed Image')
plt.show()

fromCenter = False
showCrosshair = False
img = img_original
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

fig2 = plt.figure()
plt.imshow(disReset)
plt.title('ROI Image')
plt.show()

