import numpy as np
import cv2
from matplotlib import pyplot as plt

imgRaw = cv2.imread('../../Data/Images/OK.jpg')
#imgRaw = cv2.imread('../../Data/Images/20160407_144307.jpg')

height, width = imgRaw.shape[:2]
img = cv2.resize(imgRaw,(width/2, height/2), interpolation = cv2.INTER_CUBIC)


img_rgb = imgRaw #cv2.imread('mario.png')
img_gray = cv2.cvtColor(img_rgb, cv2.COLOR_BGR2GRAY)


template = cv2.imread('../../Data/Images/template/vpurge.png',0)


w, h = template.shape[::-1]

res = cv2.matchTemplate(img_gray,template,cv2.TM_CCOEFF_NORMED)
threshold = 0.8
loc = np.where( res >= threshold)
for pt in zip(*loc[::-1]):
    cv2.rectangle(img_rgb, pt, (pt[0] + w, pt[1] + h), (255,0,0), 2)

#cv2.imwrite('res.png',img_rgb)

plt.imshow(img_rgb),plt.show()