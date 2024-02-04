#!/usr/bin/python

import matplotlib as mpl                                                                                                               
mpl.use('Agg')                                                                                                                 
import matplotlib.pyplot as plt

import os
import plotData
import plotCrossData
import plotLegend
import sys
from optparse import OptionParser
from pylab import *

def column(matrix, i):
    return [row[i] for row in matrix]

parser = OptionParser()
parser.add_option("-d", "--dir_name", dest="dir_name", default="training_files_", help="Directory name")
parser.add_option("-l", "--show_legend", dest="show_legend", default=1, help="Show legend")
parser.add_option("-n", "--dir_name_to_ignore", dest="dir_name_to_ignore", default="", help="Directory name to ignore")
parser.add_option("-o", "--output_dir", dest="output_dir", default="", help="Name of the directory where the html file will be copied")
parser.add_option("-t", "--max_delta_time", dest="max_delta_time", default=0.0, help="max_delta_time")
parser.add_option("-y", "--coeff_y_lim", dest="coeff_y_lim", default=-1.0, help="Coefficient for ylim")

(options, args) = parser.parse_args()

max_delta_time = 0
if options.max_delta_time != 0:
    max_delta_time = 60*24*int(options.max_delta_time)

coeff_y_lim = float(options.coeff_y_lim)

show_legend = int(options.show_legend)

output_dir = os.environ['HOME'] + '/public_html/' + options.output_dir

if not os.access(output_dir, os.F_OK):
    print 'Creating output directory ' + output_dir
    os.mkdir(output_dir)

dir_names = options.dir_name.split(',')

for d in xrange(0,len(dir_names)):
    dir_names[d] = dir_names[d].replace('+', '.*')

# name, col_idx, use_multi_coeff
var_names_with_attributes = [
    ['scores0/training_score', 9, 0],
    ['scores0/test_score', 9, 0],
    ['dscore', 0, 0],
    ['a_dscore', 0, 0],
    ['obj', 0, 0],
    ['m', 0, 0],
    ['learning_rate', 0, 0],
    ['norm_w', 0, 0],
    ['norm_dfy', 0, 0],
    ['loss', 0, 0],
    ['constraint_set_card', 0, 0],
    ['constraint_set_card', 1, 0],
    ['autostep_learning_rate', 0, 0],
    ['autostep_learning_rate_all_0',1, 0],
    ['autostep_learning_rate_all_1',1, 0],
    ['autostep_learning_rate_all_2',1, 0],
    ['autostep_linear_min',1, 0],
    ['autostep_quadratic_min',1, 0],
    ['d_slack',0, 0