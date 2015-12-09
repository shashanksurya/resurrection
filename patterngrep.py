#!/usr/bin/env python3.4
import sys,os
import datetime as dt
import time

def file_walker(filetype):
	current_dir=os.path.dirname(os.path.realpath(__file__))
	filelist=[]
	for dirpath,dirname, filenames in os.walk(current_dir):
		for files in filenames:
			if files.endswith("."+filetype):
				temp=[]
				temp.append(os.path.abspath(dirpath)+"/"+str(files))
				temp.append(os.path.getmtime(temp[0]))
				filelist.append(temp)
	return filelist

def file_finder(pattern,filetype):
	iflist = file_walker(filetype)
	psum=0
	for f,tom in iflist:
		for i, line in enumerate(open(f)):
			if pattern in line:
				psum+=1
				print(time.ctime(tom) + " ..." + f[-20:] + " " + str(i) + line[:40])
	return psum

if __name__ == '__main__':
	if len(sys.argv)!= 3:
		print("Incorrect number of arguments!")
	else:
		pattern = sys.argv[1]
		filetype = sys.argv[2]
		file_finder(pattern,filetype)