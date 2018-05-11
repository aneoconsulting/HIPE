import sys
import os
import pydata
cwd = os.getcwd()
import numpy
print "python binary " + sys.executable
import cv2
import numpy as np






def process(data):
	
	print "Hello world : " + cwd
	a = numpy.array([[1., 2., 3.]])
	b = numpy.array([[1.], [2.], [3.]])
	
	#print(pydata.dot(a, b)) # should print [[14.]]
	##print(pydata.dot2(a, b)) # should also print [[14.]]
	
	mat = data.get()
	#print("value : " + mat[0][0])
	
	edges = cv2.Canny(mat,100,200)
	
	edges = cv2.cvtColor(edges,cv2.COLOR_GRAY2RGB)
	
	# plt.subplot(121),plt.imshow(mat,cmap = 'gray')
	# plt.title('Original Image'), plt.xticks([]), plt.yticks([])
	# plt.subplot(122),plt.imshow(edges,cmap = 'gray')
	# plt.title('Edge Image'), plt.xticks([]), plt.yticks([])

	data.assign(edges)
	return 1337
	
