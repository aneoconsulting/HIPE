import cv2
import numpy as np
import sys
from matplotlib import pyplot as plt

# Align images with cv2
imgBack = cv2.imread("../../Data/Images/pair3.jpg")
imgTest = cv2.imread("../../Data/Images/pair1.jpg")
imgTest = cv2.cvtColor(imgTest, cv2.COLOR_RGB2BGR)
imgBack = cv2.cvtColor(imgBack, cv2.COLOR_RGB2BGR)
plt.imshow(imgTest),plt.show()
plt.imshow(imgBack),plt.show()

detector = cv2.xfeatures2d.SURF_create()
descriptor =  cv2.xfeatures2d.BriefDescriptorExtractor_create()
matcher = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)

# the matcher works on single channel images
grayBack = cv2.cvtColor(imgBack, cv2.COLOR_RGB2GRAY)
grayTest = cv2.cvtColor(imgTest, cv2.COLOR_RGB2GRAY)
plt.imshow(imgTest),plt.show()
plt.imshow(imgBack),plt.show()

# Compute magic (I found this code elsewhere 
kp1 = detector.detect(grayBack)
kp2 = detector.detect(grayTest)

k1, d1 = descriptor.compute(grayBack, kp1)
k2, d2 = descriptor.compute(grayTest, kp2)

matches = matcher.match(d1, d2)

dist = [m.distance for m in matches]
if len(dist) == 0:
	print "No Suitable matches found!"
	sys.exit(1)

mean_dist = sum(dist) / len(dist)
threshold_dist = mean_dist * 0.5

good_matches = [m for m in matches if m.distance < threshold_dist]

h1, w1 = grayBack.shape[:2]

matches = []
for m in good_matches:
	matches.append((m.queryIdx, m.trainIdx))

if len(good_matches) > 0:
	p1 = np.float32( [k1[i].pt for i, j in matches] )
	p0 = np.float32( [k2[j].pt for i, j in matches] )
	H, mask = cv2.findHomography(p0, p1, cv2.RANSAC)
	print str(len(good_matches))
	output = cv2.warpPerspective(imgTest, H, (w1, h1))
# End Magic
print str(w1) + " " + str(h1)
plt.imshow(output),plt.show()
#
#imgBack = cv2.imread("../../Data/Images/pair1.jpg")
#imgTest = cv2.imread("../../Data/Images/pair3.jpg")
#
#diff = imgBack.copy()
#grayDiff = np.zeros((imgBack.shape[0], imgBack.shape[1], 1), np.uint8)
#
#
## Perform simple substraction  
#  
#diff = cv2.absdiff(imgBack, imgTest)
#grayDiff = cv2.cvtColor(diff, cv2.COLOR_RGB2GRAY)
#plt.imshow(diff),plt.show()
#
## Get our threshold and expand it a little to clean the edges
#grayDiff = cv2.threshold(grayDiff, 40, 255, cv2.THRESH_BINARY)
#kernel = np.ones((5,5),np.uint8)
#grayDiff = cv2.dilate(grayDiff, kernel, 6)
#grayDiff = cv2.erode(grayDiff, None, 2)
#
## Copy the test image and mask with our threshold
#fore = np.zeros((imgBack.shape[0], imgBack.shape[1], 1), np.uint8)
#cv2.copy(imgTest, fore, grayDiff)
#
#cv2.imwrite("Threshold.png", grayDiff)
#cv2.imwrite("Foreground.png", fore)