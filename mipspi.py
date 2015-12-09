#!/usr/bin/env python3.4
''' *******************************************
*	Name: Alekhya Gade						  *
*	Class: CDA 3101					 		  *
*	Assignment: Asg2(an assembler of a subeset*
*				of the MIPS ASSEMBLY LANGUAGE)*
*	Compile: python3 mipspi.py		 		  *
********************************************'''
import math as m
import sys

regdict = {
		 "$zero": "00000" ,
		 "$at": "00001" ,
		 "$v0": "00010" ,
		 "$v1": "00011" ,
		 "$a0": "00100" ,
		 "$a1": "00101" ,
		 "$a2": "00110" ,
		 "$a3": "00111" ,
		 "$t0": "01000" ,
		 "$t1": "01001" ,
		 "$t2": "01010" ,
		 "$t3": "01011" ,
		 "$t4": "01100" ,
		 "$t5": "01101" ,
		 "$t6": "01110" ,
		 "$t7": "01111" ,
		 "$s0": "10000" ,
		 "$s1": "10001" ,
		 "$s2": "10010" ,
		 "$s3": "10011" ,
		 "$s4": "10100" ,
		 "$s5": "10101" ,
		 "$s6": "10110" ,
		 "$s7": "10111" ,
		 "$t8": "11000" ,
		 "$t9": "11001" ,
		 "$k0": "11010" ,
		 "$k1": "11011" ,
		 "$gp": "11100" ,
		 "$sp": "11101" ,
		 "$fp": "11110" ,
		 "$ra": "11111" ,
		 "$li": "" ,
		 "$hi": "" 
		   };

idict  = { "lw":   "100011" ,
		 "sw":  "101011" ,
		 "bne":  "000101" ,
		 "beq":  "000100" ,
		 "addiu": "001001" ,
		 None: 0  };

rdict = {
		 "addu": "100001" ,
		 "subu": "100011" ,
		 "and": "100100" ,
		 "or": "100101" ,
		 "slt": "101010" ,
		 "div": "011010" ,
		 "mfhi": "010000",
		 "mflo": "010010",
		 "mult": "011000",
		  None: 0 } ;

jdict = {"j": "000010"}
tinst = ['addiu','beq','bne']
oset =[]
iset =[]

def simulator(raw_file):
	lcount=0
	for line in raw_file:
		flag = 0
		lcount+=1
		#print('{0:08x}'.format(int(line,2)))
		if line[0:6] == '000000':			
			for opcode,value in rdict.items():
				tmp=[]
				if value == line[26:32]:
					flag =1
					print(str(lcount)+': '+opcode,end='   ')
					oset.append(opcode)
					for reg,val in regdict.items():
						if val == line[16:21]:
							print(reg, end=',')
							tmp.append(reg)
							break
					for reg,val in regdict.items():
						if val == line[6:11]:
							print(reg,end=',')
							tmp.append(reg)
							break
					for reg,val in regdict.items():
						if val == line[11:16]:
							print(reg)
							tmp.append(reg)
							break
				if len(tmp)>1:
					iset.append(tmp)

		else:			
			for opcode,value in idict.items():
				tmp=[]
				if value == line[0:6]:
					flag =1
					print(str(lcount)+': '+opcode,end='  ')
					oset.append(opcode)
					for reg,val in regdict.items():
						if val == line[11:16]:
							tmp.append(reg)
							break
					for reg,val in regdict.items():
						if val == line[6:11]:
							tmp.append(reg)
							break
					if opcode in tinst:
						tmp.append(int(line[24:32],2))
					#print(opcode)
					if opcode=='lw':
						print('   '+tmp[0]+','+tmp[1])
						iset.append([tmp[0],tmp[1]])
					if opcode=='addiu':
						print(tmp[0]+','+tmp[1]+','+str(tmp[2]))
						iset.append([tmp[0],tmp[1],str(tmp[2])])
					if opcode=='sw':
						print('   '+tmp[0]+','+tmp[1])
						iset.append([tmp[0],tmp[1]])
					if opcode=='bne' or opcode=='beq':
						print('  '+tmp[1]+','+tmp[0]+','+str(tmp[2]))
						iset.append([tmp[1],tmp[0],str(tmp[2])])
					
							
			for opcode,value in jdict.items():
				if value == line[0:6]:
					flag =1
					print(str(lcount)+': '+opcode,end='   ')
					oset.append(opcode)
					print(int(line[24:32],2))
					iset.append([int(line[24:32],2)])
					break

		if '{0:08x}'.format(int(line,2)) == '0000000c':
			flag =1
			print(str(lcount)+': '+'syscall')
			oset.append('syscall')
			iset.append([])
		if flag == 0:
			print('Illegal instruction')
			lcount-=1
	return lcount-1


if __name__ == '__main__':
	raw_file = []
	simreg ={}
	fname=''
	if len(sys.argv) == 2:
		fname = sys.argv[1]
	else:
		print('Incorrect arguments! Specify only file name of obj file')
		sys.exit()
	fopen = open(fname,'r')
	for line in fopen.readlines():
		try:
			raw_file.append('{0:032b}'.format(int(line.strip(),16)))
		except ValueError:
			pass
	for op,reg in regdict.items():
		simreg[op]=0
	lcount =simulator(raw_file)
	print('\ndata:')
	print(str(lcount)+': 0')
	for i in range(1,lcount+1):
		print('PC:'+'#')
		print('inst:  '+oset[i-1],end='   ')
		for j in range(0,len(iset[i-1])):
			print(iset[i-1][j],end=',')
		print('\n\nregs:')
		lformatter=0
		for reg,val in simreg.items():
			if lformatter%4==0:
				print()
			print(reg+' =	'+str(val),end='  ')
			lformatter+=1
		print('\n data memory:')
		print()



	#print(simreg)
	
	#print(oset)
	#print(iset)
	