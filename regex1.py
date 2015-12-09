import requests
import re
#patc = re.compile(r'"http://.*/dp/[^."]*"')
url = "http://www.amazon.com/Learning-Python-Second-Mark-Lutz/dp/0596002815" 
text=requests.get(url).text
print(text)
#for x in sorted(set(patc.findall(text))):
#	print(x.strip('"'))
