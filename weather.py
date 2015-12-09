''' This program is for fetching the weather of a given city '''
import requests

def gettemperature(data):
	out = []
	attr = ["<temp_c>", "<weather>", "<wind_mph>"]
	for line in data.readlines():		
		for x in attr:
			if x in line:
				i1 = line.find(">")
				i2 = line.find("</")
				out.append(x+" "+line[i1 + 1:i2])
	return out

if __name__ == '__main__':
	getin = raw_input("Enter the type of input web/file ")
	getcity = raw_input("Enter the city name ")
	if getin == "file":
	    readdata = open(getcity + ".xml", "r")
	    out = gettemperature(readdata)
	    if len(out)>0: 
	       	print("\n".join(out))
	       	#print(out[1] + "\n" + out[0] + "\n" + out[2])
	elif getin=="web":
		url = "http://api.wunderground.com/auto/wui/geo/WXCurrentObXML/index.xml?query="
		readdata=requests.get(url+getcity.lower()).text
		filex=open("file.xml","w")
		filex.write(readdata)
		filex.close()
		readdata=open("file.xml","r")
		out = gettemperature(readdata)
		if len(out)>0: 
			print("\n".join(out))