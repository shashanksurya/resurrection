'''Program to demonstrate files'''
import requests

def openwebsite(url):
	request_data = requests.get(url)
	print(request_data.text)

if __name__ == '__main__':
	url = raw_input("Enter a url, example: http://www.fsu.edu ")
	openwebsite(url)