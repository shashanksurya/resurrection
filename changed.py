#!/usr/bin/env python3.4
""""""
#This program is for fetching the weather of a given city
""""""
import requests

out=[]
def gettemperature(line):
   attr=["<temp_c>","<weather>","<wind_mph>"]
   for x in attr:
      if x in line:
         i1=line.find(">")
         i2=line.find("</")
         out.append(line[i1+1:i2])
   

getin=input()
getcity=input()
if getin=="web":
   if getcity=="tlh":
      readdata=requests.get("http://api.wunderground.com/auto/wui/geo/WXCurrentObXML/index.xml?query=TLH").text
      filex=open("file.xml","w")
      filex.write(readdata)
      filex.close()
      filex=open("file.xml","r")
      for line in filex.readlines():
         gettemperature(line)

   if getcity=="jax":
      readdata=requests.get("http://api.wunderground.com/auto/wui/geo/WXCurrentObXML/index.xml?query=JAX").text
      filex=open("file.xml","w")
      filex.write(readdata)
      filex.close()
      filex=open("file.xml","r")
      for line in filex.readlines():
         gettemperature(line)

if getin=="file":
   if getcity=="tlh":
      readdata=open("tlh.xml","r")
      for line in readdata.readlines():
         gettemperature(line)
   if getcity=="jax":
      readdata=open("jax.xml","r")
      for line in readdata.readlines():
         gettemperature(line)

print(out[1]+"\n"+out[0]+"\n"+out[2])   

