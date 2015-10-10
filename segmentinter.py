#!/usr/bin/env python3.4
import math
class Point(object):
	def __init__(self,co_x,co_y):
		self.x = co_x
		self.y = co_y

class Line(object):
	def __init__(self,point1,point2):
		self.point1 = point1
		self.point2 = point2


def format_input(raw_point):
	temp1 = raw_point.split(';')
	temp2 = []
	for each in temp1:
		temp2.append(each.split())
	return temp2

def get_delta(line,point):
	dx= line.point1.x*(line.point2.y-point.y)
	dy= line.point1.y*(line.point2.x-point.x)
	do= (line.point2.x*point.y)-(line.point2.y*point.x)
	return dx-dy+do

def get_distance(point1,point2):
	dist_x = point1.x-point2.x
	dist_y = point1.y- point2.y
	return math.sqrt(dist_x**2+dist_y**2)


def check_intersect(line1,line2):
	delta1 = get_delta(line1,line2.point1)
	delta2 = get_delta(line1,line2.point2)
	delta3 = get_delta(line2,line1.point1)
	delta4 = get_delta(line2,line1.point2)
	test1 = True
	test2 = True
	if delta1*delta2 >0:
		test1 = False
	elif delta1*delta2 < 0:
		test1 = True
	elif delta1*delta2 == 0:
		sim =0
		dist1 = get_distance(line1.point1, line2.point1)
		dist2 = get_distance(line1.point1, line2.point2)
		dist3 = get_distance(line1.point2, line2.point1)
		dist4 = get_distance(line1.point2, line2.point2)
		if dist1==0 or dist2==0 or dist3==0 or dist4==0:
			sim =1
		if sim==1:
			test1= True
		else:
			test1= False
	if delta3*delta4 >0:
		test2 = False
	elif delta3*delta4 < 0:
		test2 = True
	elif delta1*delta2 == 0:
		sim =0
		dist1 = get_distance(line1.point1, line2.point1)
		dist2 = get_distance(line1.point1, line2.point2)
		dist3 = get_distance(line1.point2, line2.point1)
		dist4 = get_distance(line1.point2, line2.point2)
		if dist1==0 or dist2==0 or dist3==0 or dist4==0:
			sim =1
		if sim==1:
			test2 = True
		else:
			test2= False
	if test1 and test2:
		return "True"
	else:
		return "False"


if __name__ == '__main__':
	raw_point1=format_input(input())
	raw_point2=format_input(input())
	point1 = Point(int(raw_point1[0][0]),int(raw_point1[0][1]))
	point2 = Point(int(raw_point1[1][0]),int(raw_point1[1][1]))
	point3 = Point(int(raw_point2[0][0]),int(raw_point2[0][1]))
	point4 = Point(int(raw_point2[1][0]),int(raw_point2[1][1]))
	line1 = Line(point1,point2)
	line2 = Line(point3,point4)
	print(check_intersect(line1,line2))