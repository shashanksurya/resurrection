import os
import sys
import feedparser
from bs4 import BeautifulStoneSoup
from nltk import clean_html
import urllib
#import HTMLParser


def cleanHtml(html):
    return BeautifulStoneSoup(clean_html(html),
                              convertEntities=BeautifulStoneSoup.HTML_ENTITIES).contents[0]

def get_feedparser_feed(FEED_URL):

    fp = feedparser.parse(FEED_URL)

    if fp and fp.entries and fp.entries[0]:
        print("Fetched %s entries from '%s'" % (len(fp.entries), fp.feed.title))
    else:
        print('No entries parseed!')
        sys.exit()
    return fp

def get_blog_posts(fp):
    global feed_dict, blog_posts

    blog_posts = []
    for e in fp.entries:
        try:
            content = e.content[0]
        except AttributeError:
            content = e.summary_detail
        feed_dict = {'title': e.title,
                     'content': content.value,
                     'link': e.links[0].href}
        blog_posts.append(feed_dict)
    return blog_posts

if __name__ == '__main__':
    FEED_URL = 'http://www.alldeaf.com/archive/index.php/t-7.html'
    fp = get_feedparser_feed(FEED_URL)
    blog_posts = get_blog_posts(fp)
    for each in blog_posts:
        print(each)