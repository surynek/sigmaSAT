# Extract Optimize
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

def is_output_file(filename):
  if len(filename) >= 4 and filename.find(".lpc2") >= 0:
    return True
  else:
    return False


def record_data_from_output_file(path, name):
  pos_tag_0 = path.find(name)
  pos_tag_1 = path.find(".lpc2")

  idx_name = path[pos_tag_0 : pos_tag_1]
  
  pos_tag_2 = path.find("-", pos_tag_1 + 6);
  pos_tag_3 = path.find("mini", pos_tag_2 + 1);

  idx_low  = path[pos_tag_1 + 6 : pos_tag_2]
  idx_high = path[pos_tag_2 + 1 : pos_tag_3 - 1]

  print idx_name
  print idx_low
  print idx_high

  data_records[idx_name] = 1.0


def process_directory(directory, name):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_output_file(item):
        print "Processing: " + item + " ... ",
        sys.stdout.flush();
        record_data_from_output_file(path, name)
        print "OK"


print sys.argv[1]
print sys.argv[2]

process_directory(sys.argv[1], sys.argv[2])
