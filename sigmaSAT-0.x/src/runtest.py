# Runtest
# Version: 1.0
# (C) Copyright 2010-2011 Pavel Surynek
# http://www.surynek.com
# pavel.surynek@mff.cuni.cz

import fileinput
import string
import shutil
import os
import sys


def is_formula(filename):
  if len(filename) >= 4 and string.find(filename, ".cnf") == len(filename) - 4:
    return True
  else:
    return False


def is_target(filename, substring):
  if len(filename) >= len(substring) and string.find(filename, substring) >= 0:
    return True
  else:
    return False
  

def process_directory(indent, command, directory):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_formula(item):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path);
        print "OK"
    elif os.path.isdir(path):
      print indent + "Subdir:" + item
      process_directory(indent + "  ", command, path)


def process_directory_output(indent, command, directory):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_formula(item):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path + " > " + path + ".out")
        print "OK"
    elif os.path.isdir(path):
      print indent + "Subdir:" + item
      process_directory_output(indent + "  ", command, path)


def process_directory_output_suffix(indent, command, directory, suffix, redirection, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path + redirection + path[0:len(path) - len(target)] + suffix)
        print "OK"
    elif os.path.isdir(path):
      print indent + "Subdir:" + item
      process_directory_output_suffix(indent + "  ", command, path, suffix, redirection, target)


print "Agruments:"
print "  " + sys.argv[1]
print "  " + sys.argv[2]
print "  " + sys.argv[3]
print "  " + sys.argv[4]
print "  " + sys.argv[5]

if sys.argv[1] == "-o" or sys.argv[1] == "-output":
  process_directory_output("  ", sys.argv[2], sys.argv[3])
elif sys.argv[1] == "-s" or sys.argv[1] == "-suffix":
  process_directory_output_suffix("  ", sys.argv[3], sys.argv[4], sys.argv[2], " > ", ".cnf")
elif sys.argv[1] == "-st" or sys.argv[1] == "-suffix-target":
  process_directory_output_suffix("  ", sys.argv[3], sys.argv[4], sys.argv[2], " > ", sys.argv[5])
elif sys.argv[1] == "-se" or sys.argv[1] == "-suffix-error":
  process_directory_output_suffix("  ", sys.argv[3], sys.argv[4], sys.argv[2], " 2> ", ".cnf")
elif sys.argv[1] == "-set" or sys.argv[1] == "-suffix-error-target":
  process_directory_output_suffix("  ", sys.argv[3], sys.argv[4], sys.argv[2], " 2> ", sys.argv[5])
else:
  process_directory("  ", sys.argv[1], sys.argv[2])
