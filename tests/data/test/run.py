import sys
sys.path.append(r'C:\workspace\ANEO\Project\hipe-group\hipe\Hipe\tests\data\scriptPython')
sys.path.append(r'C:\workspace\ANEO\Project\hipe-group\hipe\Hipe\build\target\Debug')
import testhipe

def process1():
	print "Hello world test"
	testhipe.process()
	return 1337
	
	
if (__name__ == "__main__"):
	process1()