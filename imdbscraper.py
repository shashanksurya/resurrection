#!/usr/bin/env python3.4
import imdb_ratings as imdb
import re


class TableHandler(object):
    # Produce Table Header

    def header(self, hdata):
        for h in hdata:
            print("{0:>10s}".format(h), end="")
        print("\n" + "-" * 79)
    # Produce single row of output

    def row(self, rdata):
        for r in rdata:
            print("{0!s:>10s}".format(r), end="")
        print()
    # Produce Table Footer

    def footer(self, footer_data):
        print("\n" + "-" * 79)


class CSVTableHandler(TableHandler):

    def header(self, hdata):
        # print header for CSV
        for h in hdata:
            print(h + ",", end="")
        print()

    def row(self, rdata):
        for r in rdata:
            print(r + ",", end="")
        print()
    # print a singl row in CSV format

    def footer(self, fdata):
        pass
    # Will you be needing a footer for CSV?


class HTMLTableHandler(TableHandler):

    def header(self, hdata):
        # print header for HTML Table
        print('<html><head><title>IMDB Helper</title><head><body>')
        print('<table><tr>')
        for h in hdata:
            print('<td>' + h + '</td>', end="")
        print('</tr>')

    def row(self, rdata):
        # print a single row of the HTML table
        print('<tr>')
        for r in rdata:
            print('<td>' + r + '</td>', end="")
        print('</tr>')

    def footer(self, fdata):
        # HTML tags should be closed
        print('</table></body></html>')


class MoviePrinter(object):

    def __init__(self, *args):
        if len(args) == 2:
            self.handler = args[1]
            self.mlist = args[0]
        else:
            self.handler = CSVTableHandler()
            self.mlist = args[0]

    def print(self):
        # print header
        headers = ["Votes", "Rating", "Year", "Title"]
        self.handler.header(headers)

        # print movie data
        for m in self.mlist:
            self.handler.row([m.votes, m.rank,
                              m.year, "    " + m.title])
        self.handler.footer(headers)


def main():
    mlist = []
    count = 0
    freader = open("needed_movies.txt")
    titles = []
    for each_title in freader.readlines():
        titles.append(each_title.strip())
    freader.close()
    # print(titles)
    count = 0
    for line in open("ratings.list", encoding="ISO-8859-1"):
        count += 1
        if count > 295:
            record = imdb.GroupMovieByElements(line)
            if record:
                if imdb.condition(record, titles):
                    mlist.append(record)
        if count > 528797:
            break
    print(len(mlist))
    handler = HTMLTableHandler()
    printer = MoviePrinter(mlist, handler)
    printer.print()

if __name__ == '__main__':
    main()
