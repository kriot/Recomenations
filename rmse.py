import sys
import numpy

a1 = numpy.loadtxt(sys.argv[1])
a2 = numpy.loadtxt(sys.argv[2])
print sum(map(lambda x: x**2, (a1 - a2))) / a1.size
		

