# Extract
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

range_name = set()
range_low = set()
range_high = set()

def is_output_file(filename, name):
  if len(filename) >= 4 and filename.find(".lpc2") >= 0 and filename.find(name) >= 0 and filename.find(".mini") >= 0:
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

  N_decisions = 0

  output_file = open(path, "r")
  for line in output_file:
    dec_pos_1 = line.find("decisions")
    if dec_pos_1 >= 0:
      dec_pos_2 = line.find(":", dec_pos_1 + 10)
      dec_pos_3 = line.find(" ", dec_pos_2 + 2)
      decisions = line[dec_pos_2 + 2:dec_pos_3]

  N_decisions = decisions

  range_name.add(idx_name)
  range_low.add(idx_low)
  range_high.add(idx_high)

  if idx_name not in data_records:
    high_records = dict()
    high_records[idx_high] = N_decisions
    low_records = dict()
    low_records[idx_low] = high_records
    data_records[idx_name] = low_records
  else:
    if idx_low not in data_records[idx_name]:
      high_records = dict()
      high_records[idx_high] = N_decisions
      (data_records[idx_name])[idx_low] = high_records
    else:
      if idx_high not in (data_records[idx_name])[idx_low]:
        ((data_records[idx_name])[idx_low])[idx_high] = N_decisions
      else:
        print "Warning: Duplicated item."
  output_file.close()
      

def process_directory(directory, name):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_output_file(item, name):
        print "Processing: " + item + " ... ",
        sys.stdout.flush();
        record_data_from_output_file(path, name)
        print "OK"


def print_records():
  print "\t",
  for idx_low in range_low:
    for idx_high in range_high:
      print idx_low,
      print "/",
      print idx_high,
      print "\t",
  print "\n",
  
  for idx_name in range_name:
    print idx_name,
    print "\t",
    for idx_low in range_low:
      for idx_high in range_high:
        if idx_name not in data_records:
          print "-\t",
        else:
          if idx_low not in data_records[idx_name]:
            print "-\t",
          else:
            if idx_high not in (data_records[idx_name])[idx_low]:
              print "-\t",
            else:
              print ((data_records[idx_name])[idx_low])[idx_high],
              print "\t",
    print "\n",

print sys.argv[1]
print sys.argv[2]

process_directory(sys.argv[1], sys.argv[2])
print_records()
