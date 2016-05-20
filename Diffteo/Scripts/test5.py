import cv2

import numpy as np
import cv2
from matplotlib import pyplot as plt

imgRaw = cv2.imread('../../Data/Images/OK.jpg')
height, width = imgRaw.shape[:2]
img = cv2.resize(imgRaw,(width/2, height/2), interpolation = cv2.INTER_CUBIC)

gray= cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

sift = cv2.xfeatures2d.SIFT_create()
kp = sift.detect(gray,None)

img=cv2.drawKeypoints(gray,kp, None)

plt.imshow(img),plt.show()