import sys
import os
import pydata
cwd = os.getcwd()
import numpy
import pbcvt # your module, also the name of your compiled dynamic library file w/o the extension


def process(data):
	print "Hello world : " + cwd
	a = numpy.array([[1., 2., 3.]])
	b = numpy.array([[1.], [2.], [3.]])
	
	print(pbcvt.dot(a, b)) # should print [[14.]]
	print(pbcvt.dot2(a, b)) # should also print [[14.]]
	
	mat = data.get()
	#print("value : " + mat[0][0])

	data.assign(a)
	return 1337
	
