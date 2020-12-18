import numpy, scipy
from scipy.signal import correlate
import sys

def main(filePath) :
    whole = numpy.loadtxt(filePath, delimiter=' ')
    t = whole.T[0]
    A = whole.T[1]
    B = whole.T[2]

    print "t = ", t
    print "A = ", A
    print "B = ", B
    
    # Put in an artificial time shift between the two datasets
    nsamples = len(A)
    # regularize datasets by subtracting mean and dividing by s.d.
    A -= A.mean(); A /= A.std()
    B -= B.mean(); B /= B.std()

    # Find cross-correlation
    xcorr = correlate(A, B)

    # delta time array to match xcorr
    dt = numpy.arange(1-nsamples, nsamples)

    recovered_time_shift = dt[xcorr.argmax()]
    print("xcorr = ", xcorr)
    print("argmax = ", xcorr.argmax())

    print "Recovered time shift: %d" % (recovered_time_shift)

if ( __name__ == "__main__"):
    print("A new day dawns")
    main(sys.argv[1])
    print("Done !")
