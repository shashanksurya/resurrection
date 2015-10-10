# Being Lazy using closures
import requests

def lazy_page(url):
    def get():
        return requests.get(url).text
    return get

fpdata = lazy_page("http://www.python.org")
fgdata = lazy_page("https://google.com")
