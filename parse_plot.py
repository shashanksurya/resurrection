#!/usr/bin/env python
''' This program is used to parse Blender output and plot using plotly'''
import sys
import ast
import math
import plotly.plotly as py
from plotly.graph_objs import *

py.sign_in('shashanksurya','80srccpnil')

def plot(xydict):
	'''Create a plot based on x and y co-ordinates dictionary'''
	trace0 = Scatter(
	    x=xydict['x'],
	    y=xydict['y'],
	    mode='markers',
	    name='NEw Plot Massive',
	    marker=Marker(
	        color = 'orange',
	        size=5,
	        symbol = 'circle'
	    )
	)

	data = Data([trace0])
	layout = Layout(

	    xaxis=XAxis(
	    	title = "Distance between ideal center of mass(COM) and COM after packing",
	        autorange=True,
	    ),

	    yaxis=YAxis(
	    	title = "Volume after packing (l*b*h)",
	        autorange=True,
	    ),
	)
	fig = Figure(data=data, layout=layout)
	plot_url = py.plot(fig, filename='text-hover')

def calc_distance(pdict1,pdict2):
	'''Calculate distance between two points in \
	space represented as dictionaries'''
	dist = 0
	p1 = ast.literal_eval(pdict1)
	p2 = ast.literal_eval(pdict2)
	tempx = (p1['x'] - p2['x'])**2
	tempy = (p1['y'] - p2['y'])**2
	tempz = (p1['z'] - p2['z'])**2
	return math.sqrt(tempx+tempy+tempz)

def parse(filename):
	'''Parses the simulation results file'''
	vol_b = []
	com_b = []
	vol_a = []
	com_a = []
	fp = open(filename,"r")
	i = -1
	for eachline in fp.readlines():
		if eachline.find("Detected GL_ARB_depth")!=-1:
			i+=1
		if eachline.find("::") != -1:
			#if eachline.find("VOL_before") != -1:
			#	vol_b.append(eachline.strip().split("::")[1])
			if eachline.find("VOL_after") != -1:
				if len(vol_a)==i:
					vol_a.append(eachline.strip().split("::")[1])
			if eachline.find("COM_before") != -1:
				if len(com_b)==i:
					com_b.append(eachline.strip().split("::")[1])
			if eachline.find("COM_after") != -1:
				if len(vol_a)==i:
					com_a.append(eachline.strip().split("::")[1])
	print("COM before: ",len(com_b))
	print("COM after: ",len(com_a))
	print("VOL after: ",len(vol_a))
	com_distances = []
	vol_diff = []
	for i in range(0,len(com_a)):
		dist = calc_distance(com_a[i],com_b[i])
		com_distances.append(dist)
	for i in range(0,len(vol_a)):
		dist = abs(float(vol_a[i])) #l/2,b/2,h/2
		vol_diff.append(dist)
	ret_dict = {}
	ret_dict['x'] = com_distances
	ret_dict['y'] = vol_diff
	return ret_dict

def main():
	'''Main function of parse-plot program'''
	com_vol = parse(sys.argv[1])
	#print(com_vol)
	plot(com_vol)

if __name__ == '__main__':
	main()