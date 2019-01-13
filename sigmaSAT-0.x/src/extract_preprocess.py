# Extract Preprocess
# Version: 1.0
# (C) Copyright 2010-2011 Pavel Surynek
# http://www.surynek.com
# pavel.surynek@mff.cuni.cz

import fileinput
import string
import shutil
import os
import sys


data_records = dict()

def is_output_file(filename, ext):
  if len(filename) >= 4 and filename.find(ext) >= 0:
    return True
  else:
    return False


def record_data_from_output_file(path, ext, category):
  pos_tag_00 = path.rfind("/")
  if pos_tag_00 < 0:
    pos_tag_00 = -1
  pos_tag_01 = path.find(ext)
  idx_name = path[pos_tag_00 + 1 : pos_tag_01]
  
  # print idx_name
  file = open(path, "r")
  
  for line in file:
    if line.find(category) >= 0:
      pos_tag_1 = line.find(":")
      pos_tag_2 = line.find(" ", pos_tag_1 + 2)
      N_conflicts = line[pos_tag_1 + 2 : pos_tag_2]
      # print N_conflicts
      data_records[idx_name] = N_conflicts
    
      
def print_recorded_data():
  for name, value in data_records.iteritems():
    print name,
    print "\t",
    print value


def process_directory(directory, ext, category):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_output_file(item, ext):
        # print "Processing: " + item + " ... ",
        sys.stdout.flush();
        record_data_from_output_file(path, ext, category)
        # print "OK"


# print sys.argv[1]
# print sys.argv[2]
# print sys.argv[3]

process_directory(sys.argv[1], sys.argv[2] + ".mini", sys.argv[3])
print_recorded_data()