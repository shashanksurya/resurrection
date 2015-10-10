'''/***********************************************************
* *
* Name: Alekhya Gade *
* Class: CDA3101 *
* Assignment: Implementing a Data Cache Simulator *
* Compile: "python3 datacache.py" *
* *
***********************************************************/'''
import math
import re
import sys

#few constants
BIT_SIZE = 32
BLOCK_SIZE = 8

#Index, offset and tag are stored in this
cache_config = []
traces = []
cache_table = []
cache_simulator = []
hits = 0
refcount  = 1
memrefcount = {}

def init():
	for i in range(0,BLOCK_SIZE):
		attr_dict = {}
		attr_dict['valid'] = 0
		cache_simulator.append(attr_dict)

def read_traces():
	file_read = open('trace.dat',"r")
	for line in file_read.readlines():
		temp_list =[]
		for each in line.strip().split(":"):
			temp_list.append(each)
		if(temp_list[0] == 'R' or temp_list[0] =='W'):
			if(int(temp_list[1])%2 ==0 and int(temp_list[1])<=8):
				if(int(temp_list[2],16)%int(temp_list[1])==0):
					traces.append(temp_list)
				else:
					print("Invalid Alignment! Ignoring this trace")
			else:
				print("Invalid Address Size! Ignoring this trace")
		else:
			print("Invalid Operation than Read or Write! Ignoring this trace")
	#print(len(traces))

def check_cache(trace_attr):
	index = trace_attr['index']
	#print(index)
	tag = trace_attr['tag']
	addr = trace_attr['Address']
	#Empty Cache
	if(cache_simulator[index]['valid']==0):
		attr_dict = {}
		attr_dict['Address'] = addr
		attr_dict['tag'] = tag
		attr_dict['valid'] = 1
		cache_simulator[index] = attr_dict
		return False

	elif(cache_simulator[index]['valid']==1 and cache_simulator[index]['tag']!=tag):
		attr_dict = {}
		attr_dict['Address'] = addr
		attr_dict['tag'] = tag
		attr_dict['valid'] = 1
		cache_simulator[index] = attr_dict
		return False		
	elif(cache_simulator[index]['valid']==1 and cache_simulator[index]['tag']==tag):
		global hits
		hits+=1
		return True

def write_cache(trace_attr):
	global hits
	index = trace_attr['index']
	tag = trace_attr['tag']
	addr = trace_attr['Address']
	checker = ''
	#What we want to write is already there in Cache
	if(cache_simulator[index]['valid']==1 and cache_simulator[index]['tag']==tag):
		hits+=1
		trace_attr['memref'] = 0
		checker = 'hit'
	#CAche miss and we will replace the tag
	else:
		attr_dict['Address'] = addr
		attr_dict['tag'] = tag
		attr_dict['valid'] = 1
		try:
			#fetched from memory and doing a write back
			memrefcount[addr] +=1
		except KeyError:
			memrefcount[addr] = 1
		cache_simulator[index] = attr_dict
		trace_attr['memref'] = memrefcount[tempdict['Address']]
		checker = 'miss'
	trace_attr['result'] = checker
	cache_table.append(trace_attr)

def parse_traces():
	global refcount
	for trace in traces:
		tempdict = {}
		try:
			tempdict['Ref'] = refcount
			tempdict['tag'] = (int(trace[2],16) >> 6) & 0x1F
			tempdict['Address'] = trace[2]
			tempdict['index'] = (int(trace[2],16) >> 3) & 0x7
			tempdict['offset'] = 0
			if(trace[0]=='R'):
				tempdict['Access'] = 'read'
			else:
				tempdict['Access'] = 'write'
				write_cache(tempdict)
				refcount+=1
				#print('Got this kid')
				continue
			#print(bin(int(trace[2],16)))
			checker = ''
			if(check_cache(tempdict)):
				checker = 'hit'
				tempdict['memref'] = 0
			else:
				checker = 'miss'
				memrefcount[trace[2]] += 1
				tempdict['memref'] = memrefcount[tempdict['Address']]
			tempdict['result'] = checker
			refcount+=1
		except KeyError:
			memrefcount[trace[2]] = 1
			tempdict['result'] = checker
			refcount+=1
			tempdict['memref'] = memrefcount[tempdict['Address']]
		cache_table.append(tempdict)
	#print(cache_table)
	#print(cache_simulator)
	#print(len(memrefcount))

def print_results():
	global refcount
	refcount-=1
	for tempdict in cache_table:
		mrfs = tempdict['memref']
		print('{0:5d} {1:5} {2:5} {3:5d} {4:5d} {5:10d} {6:5} {7:5d}'\
				.format(tempdict['Ref'],tempdict['Access'],tempdict['Address'],\
					tempdict['tag'],tempdict['index'],tempdict['offset'],tempdict['result'],\
					mrfs))
	print()
	print('Simulation Summary Statistics')
	print('-----------------------------')
	print('{0:10} :{1:10d}'.format('Total hits',hits))
	print('{0:10}:{1:9d}'.format('Total misses',refcount - hits))
	print('{0:10}:{1:7d}'.format('Total accesses',refcount))
	print('{0:10}:{1:18f}'.format('Hits Ratio',hits/refcount))
	print('{0:10}:{1:18f}'.format('Miss Ratio',(refcount - hits)/refcount))


def read_config(setcount,setsize,linesize):
	global BLOCK_SIZE
	temp =[]
	temp.append(math.log2(setcount))
	temp.append(math.log2(linesize))
	temp.append(BIT_SIZE - (temp[0] + temp [1]))
	BLOCK_SIZE = int(setcount*8/linesize)
	cache_config.append(temp)

def start_simulator():
	file_read = open('trace.config', "r")
	temp_list = file_read.readlines()
	setcount = int(re.search(r'\d+', temp_list[0]).group())
	setsize = int(re.search(r'\d+', temp_list[1]).group())
	linesize = int(re.search(r'\d+', temp_list[2]).group())
	if(setcount>8192 or setsize > 8 or linesize < 8):
		print("Cannot process with this configuration")
		sys.exit()
	else:
		read_config(setcount,setsize,linesize)
		init()
	print('Cache Configuration')
	print(str(setcount)+' '+str(setsize)+'-way set associative entries')
	print('of line size '+str(linesize)+' bytes')
	read_traces()
	print('Results for Each Reference')
	print('{0:5} {1:5} {2:5} {3:5} {4:5} {5:5} {6:5} {7:5}'.format('Ref', 'Access', \
		'Address','Tag','Index','Offset','Result','Memrefs'))
	print('{0:5} {1:5} {1:5} {0:5} {1:5} {1:5} {1:5} {1:5}'.format('-----','------'))
	parse_traces()
	print_results()


if __name__ == '__main__':
	start_simulator()