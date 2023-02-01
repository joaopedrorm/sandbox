#!/usr/bin/env python

from __future__ import print_function
import glob
import sys
from sys import argv
import re

def processUpsSql(content):
  # GET INDEX FOR UPS AND DOWNS
  upsRegex = re.compile("#.*--.*!Ups.*");
  downsRegex = re.compile("#.*--.*!Downs.*");
  upsIndex = [i for i,val in enumerate(content) if upsRegex.search(val)];
  downsIndex = [i for i,val in enumerate(content) if downsRegex.search(val)];
  if len(upsIndex) != 1:
    if len(upsIndex) < 1:
      print ("[ERROR] Ups regex didn't match anything, returning empty string");
      return ["-- empty"];
    else:
      print ("[WARNING] Multiple matchs for Ups regex, taking first only");
  if len(downsIndex) != 1:
    if len(downsIndex) < 1:
      print ("[WARNING] Dows regex didn't match anything, returning remaining lines");
      downsIndex.append(len(content));
    else:
      print ("[WARNING] Multiple matchs for Downs regex, taking first only");
  # RETURN LINES BETWEEN UPS AND DOWNS
  if upsIndex[0] < downsIndex[0]:
    return content[upsIndex[0]+1:downsIndex[0]];
  else:
    return content[upsIndex[0]+1:len(content)];

def processDowsSql(content):
  # GET INDEX FOR UPS AND DOWNS
  upsRegex = re.compile("#.*--.*!Ups.*");
  downsRegex = re.compile("#.*--.*!Downs.*");
  upsIndex = [i for i,val in enumerate(content) if upsRegex.search(val)];
  downsIndex = [i for i,val in enumerate(content) if downsRegex.search(val)];
  if len(downsIndex) != 1:
    if len(downsIndex) < 1:
      print ("[ERROR] Downs regex didn't match anything, returning empty string");
      return ["-- empty"];
    else:
      print ("[WARNING] Multiple matchs for Downs regex, taking first only");
  if len(upsIndex) != 1:
    if len(upsIndex) < 1:
      print ("[WARNING] Ups regex didn't match anything, returning remaining lines");
      upsIndex.append(len(content));
    else:
      print ("[WARNING] Multiple matchs for Ups regex, taking first only");
  # RETURN LINES BETWEEN DOWNS AND UPS
  if downsIndex[0] < upsIndex[0]:
    return content[downsIndex[0]+1:upsIndex[0]];
  else:
    return content[downsIndex[0]+1:len(content)];

def processUps(fileNameList):
  print ("[Processing] Creating ups.sql");
  ups = open("ups.sql", "w");
  ups.write("-- SQL UPS\n");
  # OPEN AND READ FILES
  for filename in fileNameList:
    file = open(filename);
    print ("[Processing] " + filename);
    statementList = processUpsSql(file.read().splitlines());
    file.close();
    ups.write("-- " + filename + "\n");
    for l in statementList:
      ups.write(l + "\n");
  ups.close();
  return;

def processDowns(fileNameList):
  print ("[Processing] Creating downs.sql");
  downs = open("downs.sql", "w");
  downs.write("-- SQL DOWNS\n");
  # OPEN AND READ FILES IN REVERSE ORDER
  list.reverse(fileNameList);
  for filename in fileNameList:
    file = open(filename);
    print ("[Processing] " + filename);
    statementList = processDowsSql(file.read().splitlines());
    file.close();
    downs.write("-- " + filename + "\n");
    for l in statementList:
      downs.write(l + "\n");
  downs.close();
  return;

def getFileNames(filepaths):
  fileNameList = [];
  # GET FILE LIST FOR EACH PATH
  for filepath in filepaths:
    # GET NUMBERED SQL FILES IN ASC ORDER
    tempFileNameList = [];
    tempFileNameList += sorted(glob.glob(filepath + "[0-9].sql"));
    tempFileNameList += sorted(glob.glob(filepath + "[0-9][0-9].sql"));
    tempFileNameList += sorted(glob.glob(filepath + "[0-9][0-9][0-9].sql"));
    tempFileNameList += sorted(glob.glob(filepath + "[0-9][0-9][0-9][0-9].sql"));
    if len(tempFileNameList) < 1:
      print ("[PATH] No .sql file detected in " + filepath);
    else:
      print ("[PATH] " + str(len(tempFileNameList)) + " .sql files detected in " + filepath);
      fileNameList += tempFileNameList;
  # EXIT IF NO SQL FILES WERE DETECTED
  if len(fileNameList) < 1:
    print ("[PATH] No .sql files detected");
    sys.exit(1);
  else:
    print ("[PATH] " + str(len(fileNameList)) + " .sql files detected");
  return fileNameList;

def main():
  filepaths = ["/home/swex/nsw/arquitetura/conf/evolutions/mysql/", "/home/swex/nsw/aplicacao/conf/evolutions/mysql/"];
  filenamelist = getFileNames(filepaths);
  processUps(filenamelist);
  processDowns(filenamelist);
  print("[Done]");
  return 0;

if __name__ == "__main__":
  sys.exit(int(main() or 0))
