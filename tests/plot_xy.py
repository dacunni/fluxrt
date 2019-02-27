#!/usr/bin/env python

import matplotlib.pyplot as plt
import sys

outfile = sys.argv[1]

#print(sys.argv)

for arg in sys.argv[2:]:
    infile = arg
    #print(infile)
    points = []
    with open(infile) as f:
        for line in f:
            values = [token.strip() for token in line.split(" ")]
            points.append(values)
    byaxis = zip(*points)
    plt.plot(*byaxis)

plt.savefig(outfile)

