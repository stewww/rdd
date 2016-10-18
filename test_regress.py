import scipy.optimize as optimize
import numpy as np
from numpy import genfromtxt
from array import array

A = genfromtxt('IDK.csv', delimiter=',')

def func(data, aa, a, bb, b, cc, c):
    return data[:,0] * data[:,0] * aa + data[:,0] * a + \
    	   data[:,1] * data[:,1] * bb + data[:,1] * b + \
    	   data[:,2] * data[:,2] * cc + data[:,2] * c
guess = (1,1,1,1,1,1)

thousand = np.empty(len(A))
compareConst = 1000
thousand.fill(compareConst)
params, pcov = optimize.curve_fit(func, A[:,:3], thousand, guess)

statistics = list()

maxDifference = 120

i = 0
for row in A:
	value = params[0] * row[0] * row[0] + params[1] * row[0] + \
			params[2] * row[1] * row[1] + params[3] * row[1] + \
			params[4] * row[2] * row[2] + params[5] * row[2]
	if abs(value - 1000) > maxDifference:
		print "Skipping %i: %f" %(i, value - 1000)
	else:
		statistics.append(value)
	i = i + 1

print "\tdouble hueConstSquared = %f;\n\tdouble hueConst = %f;" %(params[0], params[1])
print "\tdouble satConstSquared = %f;\n\tdouble satConst = %f;" %(params[2], params[3])
print "\tdouble valConstSquared = %f;\n\tdouble valConst = %f;" %(params[4], params[5])
print "\tdouble hsvStdev = %f;\n\tdouble hsvConst = %i;" %(np.std(statistics), compareConst)

print "double meanH = %f;" %np.mean(A[:,0])
print "double stdevH = %f;" %np.std(A[:,0])
print "double meanS = %f;" %np.mean(A[:,1])
print "double stdevS = %f;" %np.std(A[:,1])
print "double meanV = %f;" %np.mean(A[:,2])
print "double stdevV = %f;" %np.std(A[:,2])