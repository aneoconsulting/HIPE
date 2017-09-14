import cv2

import numpy as np
import cv2
from matplotlib import pyplot as plt

def main():
	img1 = cv2.imread('../../Data/Images/pair2.jpg')
	img2 = cv2.imread('../../Data/Images/pair1.jpg')
	
	img3 = cv2.absdiff(img1, img2)
	
	#kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(3,3))
	fgbg = cv2.createBackgroundSubtractorMOG2()
	
	fgmask = fgbg.apply(img2)
	fgmask = fgbg.apply(img1)
	
	#fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, kernel)
	
	plt.imshow(img3),plt.show()
		
main()