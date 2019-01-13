# Runpreprocess
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


def process_directory_sigma(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        print command + " " + path + " > " + path[0:len(path) - len(target)] + suffix + " " + path
        os.system(command + " " + path + " > " + path[0:len(path) - len(target)] + suffix)
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_sigma(indent + "  ", suffix, command, path, target)

      
def process_directory_HyPre(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " -o " + path[0:len(path) - len(target)] + suffix + " " + path)
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_HyPre(indent + "  ", suffix, command, path, target)      


def process_directory_NiVer(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path);
        os.system("cp " + "./outfile.cnf " + path[0:len(path) - len(target)] + suffix)
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_NiVer(indent + "  ", suffix, command, path, target)


def process_directory_LiVer(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path + " 16");
        os.system("cp " + "./outfile.cnf " + path[0:len(path) - len(target)] + suffix)
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_LiVer(indent + "  ", suffix, command, path, target)


def process_directory_Shatter(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        sys.stdout.flush();
        os.system(command + " " + path);
        os.system("mv " + path + ".Sym.cnf " + path[0:len(path) - len(target)] + suffix)
        os.system("rm -f " + path + ".SymOnly.cnf")
        os.system("rm -f " + path + ".g")
        os.system("rm -f " + path + ".txt")
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_Shatter(indent + "  ", suffix, command, path, target)        


def process_directory_BCE(indent, suffix, command, directory, target):
  listing = os.listdir(directory)
  for item in listing:
    path = directory + "/" + item
    if os.path.isfile(path):
      if is_target(item, target):
        print indent + "Processing: " + item + " ... ",
        print command + " -k " + path + " > " +  path[0:len(path) - len(target)] + suffix,
        sys.stdout.flush();        
        os.system(command + " -k " + path + " > " +  path[0:len(path) - len(target)] + suffix)
        print "OK"
      elif os.path.isdir(path):
        print indent + "Subdir:" + item
        process_directory_BCE(indent + "  ", suffix, command, path, target)


print "Agruments:"
print "  " + sys.argv[1]
print "  " + sys.argv[2]
print "  " + sys.argv[3]
print "  " + sys.argv[4]
print "  " + sys.argv[5]

if sys.argv[1] == "-sigma" or sys.argv[1] == "-Sigma":
  print "Preprocessing sigma..."
  process_directory_sigma("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
elif sys.argv[1] == "-hypre" or sys.argv[1] == "-HyPre":
  print "Preprocessing HyPre..."
  process_directory_HyPre("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
elif sys.argv[1] == "-niver" or sys.argv[1] == "-NiVer":
  print "Preprocessing NiVer..."
  process_directory_NiVer("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
elif sys.argv[1] == "-liver" or sys.argv[1] == "-LiVer":
  print "Preprocessing LiVer..."
  process_directory_LiVer("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
elif sys.argv[1] == "-shatter" or sys.argv[1] == "-Shatter":
  print "Preprocessing Shatter..."
  process_directory_Shatter("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
elif sys.argv[1] == "-BCE" or sys.argv[1] == "-BCE":
  print "Preprocessing BCE..."
  process_directory_BCE("  ", sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])

