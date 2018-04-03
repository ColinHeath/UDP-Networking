import matplotlib.pyplot as pyplot
import numpy as numpy

x = numpy.random.random_integers(1, 100, 5)
pyplot.hist(x, bins=20)
pyplot.ylabel('Frequency')
pyplot.show()