#!/usr/bin/env python3.4
''' This program does the amazon scraping for a given URL'''
import requests
import re

def test_processurl():
    '''To test the url scraping'''
    test_url = str(r'http://www.amazon.com/Introduction-Algorithms-3rd-Thomas-Cormen/dp/0262033844')
    assert len(process_url(test_url)) > 4, "Failed to fetch data"

def test_gettitle():
    '''To test the title'''
    test_doc = str(
        r'<span id="productTitle" class="a-size-large">Introduction to Algorithms, 3rd Edition</span>')
    assert gettitle(
        test_doc) == "Introduction to Algorithms, 3rd Edition", "Failed to fetch Title"


def test_getauthors():
    '''To test the authors'''
    test_doc = str(
        r'<a data-asin="B000AQ24AS" href="/Thomas-H.-Cormen/e/B000AQ24AS/ref=dp_byline_cont_book_1">Thomas H. Cormen</a>')
    assert len(getauthors(test_doc)) > 0, "Failed to fetch Author"


def test_geturl():
    '''To test the url'''
    assert len(geturl("0262033844")) > 0, "Invalid ISBN number"


def test_getprice():
    '''To test the price'''
    test_doc = str(
        r'<span class="a-size-medium a-color-price offer-price a-text-normal">$79.13</span>')
    assert getprice(test_doc) == '$79.13', "Failed to fetch Price"


def test_getreview():
    '''To test the review'''
    test_doc = str(
        r'<span id="acrPopover" class="reviewCountTextLinkedHistogram noUnderline" title="4.3 out of 5 stars">')
    assert getreview(
        test_doc) == '4.3 out of 5 stars', "Failed to fetch Review"


def test_getrevcount():
    '''To test the review count'''
    test_doc = str(r'<span id="acrCustomerReviewText" class="a-size-base">170 customer reviews</span>')
    assert getrevcount(test_doc) == '170 customer reviews', "Failed to fetch Review Count"


def test_getdimensions():
    '''To test the dimensions'''
    test_doc = str(r'Product Dimensions:   </b>    8 x 1.8 x 9 inches <')
    assert getdimensions(test_doc) != 0, "Failed to fetch Dimensions"


def start(filename):
    '''Everything starts from here

    >>> start("doc_list_isbn.txt")
    ISBN of the book: 0262033844
    Title of the book: Introduction to Algorithms, 3rd Edition
    Authors of the Book:  Thomas H. CormenCharles E. LeisersonRonald L. RivestClifford Stein
    Price of the book: $79.13
    Review of the book: 4.3 out of 5 stars
    No. of Reviews: 170 customer reviews
    Book Dimensions: 8 x 1.8 x 9 inches
    -----------------------------------
    '''
    file_read = open(filename, "r")
    for isbn in file_read.readlines():
        print("ISBN of the book: " + isbn)
        amazon_url = geturl(isbn.strip())
        process_url(amazon_url)
        print("-----------------------------------")


def geturl(isbn):
    '''This function is to search for a URL for a given ISBN

    >>> getURL("0262033844")
    'http://www.amazon.com/Introduction-Algorithms-3rd-Thomas-Cormen/dp/'
    '''
    data = requests.get("http://www.amazon.com/s/ref=nb_sb_noss?url=search-alias%3Dstripbooks&field-keywords=" + str(isbn), headers={'User-Agent': 'My User Agent 1.0',
                                                                                                                                     'From': 'youremail@domain.com'})
    re1 = r"http:\/\/www.amazon.com\/[a-zA-Z0-9-]*\/dp\/"
    urls = re.findall(re1 + isbn, data.text)
    return urls[0]


def gettitle(data):
    '''This function is for getting title of the book'''
    match = re.findall(r'<span id="(.*?)".*>(.*)<\/span>', data)
    if match:
        for each in match:
            if each[0] == "productTitle":
                print("Title of the book: " + each[1])
                return each[1]


def getauthors(data):
    '''This function is for getting authors of the book'''
    reg = r'<a data-asin.*?>(.*)<'
    match = re.findall(reg, data)
    if match:
        print('Authors of the Book: ', end=" ")
        for one in match:
            print(one, end=" ")
        return match


def getprice(data):
    '''This function is for getting price of the book'''
    reg = r'<span class="a-size-medium a-color-price offer-price a-text-normal">(.*)<\/span>'
    match = re.findall(reg, data)
    if match:
        print('\nPrice of the book: ', end="")
        print(match[0])
        return match[0]


def getreview(data):
    '''This function is for getting review of the book'''
    reg = r'<span id="acrPopover" class="reviewCountTextLinkedHistogram noUnderline" title="(.*?)"'
    match = re.findall(reg, data)
    if match:
        print('Review of the book: ', end="")
        print(match[0])
        return match[0]


def getrevcount(data):
    '''This function is for getting review count of the book'''
    reg = r'<span id="acrCustomerReviewText" class="a-size-base">(.*?)<'
    match = re.findall(reg, data)
    if match:
        print('No. of Reviews: ', end="")
        print(match[0])
        return match[0]


def getdimensions(data):
    '''This function is for getting dimensions of the book'''
    reg = r'Product Dimensions:     <\/b>(.*?)<'
    match = re.findall(reg, data.replace('\n', ''))
    if match:
        print('Book Dimensions: ', end="")
        print(match[0].strip())
        return match[0]


def process_url(amazon_url):
    '''This function is for scraping a given URL.'''
    data = requests.get(amazon_url, headers={'User-Agent': 'My User Agent 1.0',
                                             'From': 'youremail@domain.com'})
    out_list = []
    out_list.append(gettitle(data.text))
    out_list.append(getauthors(data.text))
    out_list.append(getprice(data.text))
    out_list.append(getreview(data.text))
    out_list.append(getrevcount(data.text))
    out_list.append(getdimensions(data.text))
    return out_list

if __name__ == '__main__':
    start("list_isbn.txt")
