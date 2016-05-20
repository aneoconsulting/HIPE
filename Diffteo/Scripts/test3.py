import numpy as np
import cv2
from matplotlib import pyplot as plt


imgRaw = cv2.imread('../../Data/Images/pair1.jpg')
#imgRaw = cv2.imread('../../Data/Images/20160407_144307.jpg')

height, width = imgRaw.shape[:2]
img = cv2.resize(imgRaw,(width/2, height/2), interpolation = cv2.INTER_CUBIC)


imgRaw2 = cv2.imread('../../Data/Images/pair2.jpg')
img2 = cv2.resize(imgRaw2,(width/2, height/2), interpolation = cv2.INTER_CUBIC)

imgRaw3 = cv2.imread('../../Data/Images/pair3.jpg')
img3 = cv2.resize(imgRaw3,(width/2, height/2), interpolation = cv2.INTER_CUBIC)

kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3,3))
fgbg = cv2.createBackgroundSubtractorMOG2()



fgmask = fgbg.apply(img2)
fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

fgmask = fgbg.apply(img3)
fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)

fgmask = fgbg.apply(img)
fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)



plt.imshow(fgmask),plt.show()
