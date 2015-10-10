import httplib2
from bs4 import BeautifulSoup, SoupStrainer
import sys
import time
import sqlite3
import requests
import re
from lxml import html

conn = sqlite3.connect('crawl_data.db')
c = conn.cursor()
need_escapes = ['/','.','?']
http = httplib2.Http()
status, response = http.request('http://www.alldeaf.com/archive/index.php')

t = BeautifulSoup(response, parse_only=SoupStrainer('a',href=True))
count = 0
links_found =[]
filex = open('write_links.txt','w')
for link in t.find_all('a'):
	#print(link['href'])
	http2 = httplib2.Http()
	status2, response2 = http.request(link['href'])
	t2 = BeautifulSoup(response2, parse_only=SoupStrainer('a',href=True))
	count+=1
	for link2 in t2.find_all('a'):
		#print(link2['href'])
		reg = 'f-[0-9]*'
		match = re.findall(reg,link2['href'])
		if not match:
			if link2['href'] != link['href'] and \
			link2['href'] not in links_found:
					links_found.append([link['href'],link2['href']])
					filex.write(link2['href']+"		"+link['href'])
					filex.write("\n")
		count+=1
		#c.execute("insert into links")
print("Total Links found = ",count)
print("Data fetch links = ",len(links_found))
'''Fetch data now'''
escape = lambda s, escapechar, specialchars: "".\
join(escapechar + c if c in specialchars or \
 	c == escapechar else c for c in s)
#links_found = ['www.alldeaf.com/archive/index.php/t-43.html']
finished_section =''
for link in links_found:
	if link[1].find('alldeaf.com') != -1:
		try:
			section = ''
			topic = ''
			users =[]
			dates =[]
			forum =[]
			#print(link[1])
			data  = requests.get(link[1])
			#print(data.text)
			
			reg = r'<meta name="description" content="(.*?)".*>'
			match = re.findall(reg,data.text)
			if match:
				for each in match:
					section = each
			searcher = link[1][:-5]
			searcher = searcher.replace('archive/index.php/','showthread.php?')
			searcher = searcher.replace('t-','t=')
			url_pattern = escape(searcher,"\\",need_escapes)+ '">(.*?)<'
			#print(url_pattern)
			match = re.findall(url_pattern,data.text)
			if match:
				for each in match:
					topic = each
			reg = r'<div class="username">(.*?)<'
			match = re.findall(reg,data.text)
			if match:
				for each in match:
					users.append(each)
			reg = r'<div class="date">(.*?)<'
			match = re.findall(reg,data.text)
			if match:
				for each in match:
					dates.append(each)
			reg = r'class="posttext">(.*?)<\/div>'
			match = re.findall(reg,data.text,re.DOTALL)
			if match:
				for each in match:
					attach = each.replace("<br />","\n")
					#print(attach)
					forum.append(attach)
			if len(users) == len(dates) == len(forum) and len(users)>0:
				#print("testpass "+str(len(users)) + str(len(dates)) + str(len(forum)))
				section = section.replace(topic,'')
				section = section.replace('[Archive]   ','')
				if finished_section != section:
					print("Started Fetching "+section)
					finished_section = section
				for i in range(0,len(users)):
					c.execute("insert into data values(?,?,?,?,?,?)",(section,topic,link[1],users[i],forum[i],dates[i]))
			else:
				print("testfail")
		except (requests.exceptions.MissingSchema) as e:
			pass
conn.commit()
conn.close()