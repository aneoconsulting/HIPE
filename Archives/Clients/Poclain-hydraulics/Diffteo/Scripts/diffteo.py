import cv2

import numpy as np
import cv2
from matplotlib import pyplot as plt

def main():
	print "Hello world"
	img = cv2.imread('../../Data/Images/OK.jpg',1)
	#img = cv2.imread('../../Data/Images/pair1.jpg')
	cv2.namedWindow('image', cv2.WINDOW_NORMAL)
	#img = 
	#Load the Image
	imgo = img #cv2.imread('koAl2.jpg')
	
	height, width = imgo.shape[:2]
	imgo = cv2.resize(img,(width/5, height/5), interpolation = cv2.INTER_CUBIC)
	#Create a mask holder
	mask = np.zeros(imgo.shape[:2],np.uint8)

	#Grab Cut the object
	bgdModel = np.zeros((1,65),np.float64)
	fgdModel = np.zeros((1,65),np.float64)

	#Hard Coding the Rect The object must lie within this rect.
	rect = (10,10,width-30,height-30)
	cv2.grabCut(imgo,mask,rect,bgdModel,fgdModel,5,cv2.GC_INIT_WITH_RECT)
	mask = np.where((mask==2)|(mask==0),0,1).astype('uint8')
	img1 = imgo*mask[:,:,np.newaxis]

	#Get the background
	background = imgo - img1

	#Change all pixels in the background that are not black to white
	background[np.where((background > [0,0,0]).all(axis = 2))] = [255,255,255]

	#Add the background and the image
	final = background + img1

	#To be done - Smoothening the edges

	cv2.imshow('image', img1 )

	k = cv2.waitKey(0)

	if k==27:
		cv2.destroyAllWindows()
	
main()
