import re
import requests
patc = re.compile(r'"http://.*/dp/[^."]*"')
url = "http://www.facebook.com"
text = requests.get(url).text
for x in sorted(set(patc.findall(text))):
	print(x.strip('"'))