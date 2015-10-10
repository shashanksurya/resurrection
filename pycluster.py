#!/usr/bin/env python3.4
import sys,random,math,time,copy

def readFile(FILENAME):
	dlist=[]
	freader = open(FILENAME,"r")
	for each in freader.readlines():
		temp=[ float(x.strip('\t\n\r')) for x in each.split()]
		if len(temp) > 0:
			dlist.append(temp)
	return dlist

def nearest_distance(point,center):
	square_sum=0
	hdist=[]
	for each in center:
		square_sum=0
		for xy in range(0,len(each)):
			#print("length of each and point are "+str(len(each))+" "+str(len(point)))
			square_sum += ((each[xy]-point[xy]) ** 2)
		hdist.append(math.sqrt(square_sum))
	#print("Hamming distances are")
	#print(hdist)
	return hdist.index(min(hdist))
	


def find_convergence(previous_centers,DATASET):
	cluster_holder=[]
	for i in previous_centers:
		cluster_holder.append([])

	#print(previous_centers)
	for each in DATASET:
		closest_center=nearest_distance(each,previous_centers)
		cluster_holder[closest_center].append(each)
		#print(each,end=" ")
		#print("Closest center is "+str(closest_center))

	'''print("Cluster holder dump of length\n"+str(len(cluster_holder)))
	for each in cluster_holder:
		print("length of each="+str(len(each)))
		print(each)
	'''
	return cluster_holder	

def find_means(cluster_holder):
	centers=[]
	sums=[]
	means=[]
	for each in cluster_holder:
		sums.append(list(zip(*each)))
		means.append([])

	for each in range(0,len(sums)):
		for i in sums[each]:
			means[each].append(sum(i)/len(i))
	return means

def distance(point1,point2):
	sumxy=0
	for x in range(0,len(point1)):
		sumxy += (abs(point1[x]-point2[x] )) **2
	return sumxy

def find_list(element,list_element):
        try:
            index_element=list_element.index(element)
            return index_element
        except ValueError:
            return -1

def lloyd_covergence(DATASET,CLUSTERS):
	cent_locate=[]
	centers=[]
	prevcluster_holder=[]
	current_holder=[]
	min_cluster=[]
	count=0
	mincost=sys.maxsize #Sufficiently large
	#RUN IT 100 TIMES 
	for i in range(0,100):
	#PICKING UP k RANDOM POINTS
		cent_locate=random.sample(range(0,len(DATASET)-1), CLUSTERS)
		for j in cent_locate:
			centers.append(DATASET[j])
		#print("\n the centers are")
		#print(centers)
		#print("This is "+str(i)+"th time lloyd_covergence is called!")
		current_holder=find_convergence(centers,DATASET)
		count=1
		while prevcluster_holder != current_holder:
			count+=1
			prevcluster_holder=current_holder
			centers=find_means(prevcluster_holder)
			current_holder=find_convergence(centers,DATASET)
		#print("\n-------YAY! found the convergence point took "+str(count)+" iterations!------- \n")
		'''print("The clusters are:")
		for cluster in current_holder:
			print(cluster)
		'''
		sqsum=0
		for each in range(0,len(current_holder)):
			for point in current_holder[each]:
				sqsum += distance(point,centers[each])
		if(mincost>sqsum):
			mincost=sqsum
			min_cluster=copy.deepcopy(current_holder)
		#Clean up
		centers.clear()
		cent_locate.clear()
		prevcluster_holder.clear()
		current_holder.clear()
	print("\nThe optimum kmeans mincost is : "+str(mincost))
	for each in min_cluster:
		for each2 in each:
			for each3 in each2:
				print(each3,end=" ")
			print()
		print('next')
	print("The vector is [ ",end="")
	for each in DATASET:
			for cluster_i in range(0,len(min_cluster)):
				if find_list(each,min_cluster[cluster_i]) !=-1:
					print(str(cluster_i)+" ",end="")
					break
	print("]")

def average_distance(cluster1,cluster2):
	
	avg_sum=0
	count=0
	temp_f=[]
	if isinstance(cluster2[0],float):
		dist_c=0
		for cl1 in cluster1:
			for tt in range(0,len(cl1)):
				dist_c+=(cl1[tt]-cluster2[tt])**2
			avg_sum+=math.sqrt(dist_c)
			count+=1
	else:
		for cl1 in cluster1:
			for cl2 in cluster2:
				avg_sum+=math.sqrt(distance(cl1,cl2))
				count+=1
	#print("Average Sum is "+str(avg_sum/count))
	return avg_sum/len(cluster2)


def get_distances(point_list):
	point_distance = []
	for each in range(0,len(point_list)):
		point_distance.append([])
		for point in point_list:
			point_distance[each].append(math.sqrt(distance(point_list[each],point)))
				#point_distance[each].append(distance(each,point))
	#print(point_distance)
	return point_distance

def find_closest_clusters(point_distance):
	min_dist=sys.maxsize
	pX=[]
	pY=[]
	temp=[]
	for x in point_distance:
		for y in x:
			if y<min_dist and y!=0.0 and point_distance.index(x)!=x.index(y):
				min_dist=y
				pX=point_distance.index(x)
				pY=x.index(y)
	temp.append(pX)
	temp.append(pY)
	return temp


def average_linkage(DATASET,CLUSTERS):
	cluster_holder=copy.deepcopy(DATASET)
	point_distance = get_distances(cluster_holder)
	print(cluster_holder)
	while len(cluster_holder)>CLUSTERS:
		close_clusters_loc=find_closest_clusters(point_distance)
		close_clusters = []
		#print(close_clusters_loc)
		for x in close_clusters_loc:
			if isinstance(cluster_holder[x][0],float):
				close_clusters.append(cluster_holder[x])
			else:
				for y in cluster_holder[x]:
					close_clusters.append(y)
		#print(close_clusters)
		
		for x in sorted(close_clusters_loc,reverse=True):
			cluster_holder.pop(x)
			point_distance.pop(x)
		
		cluster_holder.append(close_clusters)
		
		for x in range(0,len(point_distance)):
			for y in sorted(close_clusters_loc,reverse=True):
				point_distance[x].pop(y)
			point_distance[x].append(average_distance(close_clusters,cluster_holder[x]))
		point_distance.append([])
		for x in cluster_holder:
			point_distance[len(point_distance)-1].append(average_distance(close_clusters,x))
		
		
		#print(len(point_distance[0]))
		#print(len(point_distance))
	print()
	print(cluster_holder)
	print("The vector is [ ",end="")
	for each in DATASET:
			#print(str(each))
			isfound=0
			for cluster_i in range(0,len(cluster_holder)):
				if find_list(each,cluster_holder[cluster_i]) !=-1:
					print(str(cluster_i)+" ",end="")
					isfound=1
					break
			if isfound == 0:
				try:
					if cluster_holder.index(each)>=0:
						print(str(cluster_holder.index(each))+" ",end="")
				except ValueError:
					pass

            
	print("]")

if __name__ == '__main__':
	FILENAME = ''
	ALGORITHM = ''
	CLUSTERS = 0
	DATASET=[]
	if len(sys.argv) < 4:
		print("Insufficient Arguments! \nUsage: python3.4 <program> <dataset> <clusters> <kmeans or average>")
	elif len(sys.argv) >5:
		print("Incorrect Arguments! \nUsage: python3.4 <program> <dataset> <clusters> <kmeans or average>")
	else:
		FILENAME = str(sys.argv[1])
		ALGORITHM = str(sys.argv[3])
		CLUSTERS = int(sys.argv[2])
		print("Starting with the dataset " + FILENAME + ", algorithm " + ALGORITHM + " and clusters = " + str(CLUSTERS))
		DATASET=readFile(FILENAME)
		#print(DATASET)
		start_time=time.time()
		if ALGORITHM == "kmeans":
			lloyd_covergence(DATASET,CLUSTERS)
		elif ALGORITHM == "average":
			average_linkage(DATASET,CLUSTERS)
		end_time=time.time()
		print("\nIt took "+str(end_time-start_time)+" seconds to complete this")
		