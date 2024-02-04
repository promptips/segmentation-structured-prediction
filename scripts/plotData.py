import commands
import configIO
import os
import re
import numpy as np
from pylab import *
import itertools
import time

def isFloat(string):
    try:
        float(string)
        return True
    except ValueError:
        return False

def readColumnFromFile(col_idx, filename):
    f = open(filename)
    lines = f.readlines()

    if len(lines) == 0:
        return []

    # check if first line is a header
    first_idx = 0
    l = lines[0]
    l_s = l.split()
    if not isFloat(l_s[0]):
        #print l_s[0] + ' is not a digit'
        first_idx = 1

    objs = [0 for x in range(len(lines)-first_idx)]
    obj_idx = 0
    for li in range(first_idx, len(lines)):
        l = lines[li]
        l_s = l.split()
        if len(l_s) == 0:
            break
        if len(l_s) <= col_idx:
          