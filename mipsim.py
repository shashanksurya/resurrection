#!/usr/bin/env python3.4
''' *******************************************
*	Name: Alekhya Gade						  *
*	Class: CDA 3101					 		  *
*	Assignment: Asg2(an assembler of a subeset*
*				of the MIPS ASSEMBLY LANGUAGE)*
*	Compile: python3 mipspi.py		 		  *
********************************************'''
import math as m

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
		 "$gp": "01000",
		 None: 0  };

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
fwref = {}


def parse_asm1(ins_list):
	iflag=0
	dflag=0
	wflag=0
	ins=0
	dat =0
	word =0
	lcount=0
	for each in ins_list:
		lcount+=1
		if '.text' in each:
			iflag =1
			continue
		if '.data' in each:
			dflag=1
			continue
		if '.word' in each:
			wflag=1
		if iflag is 1 and dflag is 0:
			ins+=1
		if iflag is 1 and dflag is 1:
			dat+=1
		if dflag is 1 and wflag is 1:
			word+=1
		x=each.split()
		if len(x)>1 and ':' in x[0]:	#storing fwd referneces
			fwref[x[0].strip(':')]=lcount
	print(str(ins)+' '+str(word))	
	#print(jref)

def parse_asm2(ins_list):
	lcount=0
	prevjmp=0
	for each in ins_list:
		lcount+=1
		machinecode=''
		x=[]
		if ':' not in each:
			x=each.split()
		else:
			x=each.split()
			x=x[1:]
		if '.' not in x[0]:
			if len(x)>1 and x[0] in rdict:
				machinecode+='000000'
				regsplit=x[1].split(",")
				machinecode+=regdict[regsplit[1]]
				machinecode+=regdict[regsplit[2]]
				machinecode+=regdict[regsplit[0]]
				machinecode+='00000'
				machinecode+=rdict[x[0]]
				#print(hex(int(machinecode,2)))
				print('{:08x}'.format(int(machinecode,2)))
			if len(x)>1 and x[0] in idict:
				machinecode+=idict[x[0]]
				regsplit=x[1].split(",")
				if 'addiu' in x[0]:
					machinecode+=regdict[regsplit[1]]
					machinecode+=regdict[regsplit[0]]
					machinecode+='{0:016b}'.format(int(regsplit[2]))
					print('{:08x}'.format(int(machinecode,2)))
				elif 'lw' in x[0]:
					tmp = regsplit[1].split('(')
					tmp = tmp[1].split(')')[0]
					#print(tmp)
					machinecode+=regdict[tmp]
					machinecode+=regdict[regsplit[0]]
					machinecode+='0000000000000000'
					#print(machinecode)
					print('{:08x}'.format(int(machinecode,2)))
				elif 'sw' in x[0]:
					tmp = regsplit[1].split('(')
					tmp = tmp[1].split(')')[0]
					#print(tmp)
					machinecode+=regdict[regsplit[0]]
					machinecode+=regdict[tmp]
					machinecode+='0000000000000000'
					print('{:08x}'.format(int(machinecode,2)))
				else:				#bne and #beq handling
					if len(regsplit)==3:
						prevjmp=lcount
						machinecode+=regdict[regsplit[0]]
						machinecode+=regdict[regsplit[1]]
						disp = abs(fwref[regsplit[2]]-lcount)
						machinecode+='{0:016b}'.format(disp)
						print('{:08x}'.format(int(machinecode,2)))
					else:
						machinecode+=regdict[regsplit[0]]
						machinecode+=regdict[regsplit[1]]
						machinecode+='0000000000000000'
						print('{:08x}'.format(int(machinecode,2)))
			if x[0]=='syscall':
				print('0000000c')
			if x[0]=='j':
				machinecode+=jdict[x[0]]
				machinecode+='{0:026b}'.format(abs(lcount-prevjmp))
				print('{:08x}'.format(int(machinecode,2)))

if __name__ == '__main__':
	raw_file = []

	while True:
		try:
			s=input()
			raw_file.append(s.split('#',1)[0])
		except EOFError:
			break
	parse_asm1(raw_file)
	parse_asm2(raw_file)