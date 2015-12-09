from imdb_ratings import condition, GroupMovieByElements


class TableHandler(object):
    # Produce Table Header
    def header(self, hdata):
        pass

    # Produce single row of output
    def row(self, rdata):
        pass

class TextTableHandler(TableHandler):
    def header(self, hdata):
        for h in hdata:
            print("{0:>10s}".format(h), end="")
        print("\n" + "-"*79)

    def row(self, rdata):
        for r in rdata:
            print("{0!s:>10s}".format(r), end="")
        print()

class MoviePrinter(object):
    def __init__(self, handler):
        self.handler = handler

    def print_movie_table(self, mlist):
        # print header
        headers = ["Votes", "Rating", "Year", "Title"]
        self.handler.header(headers)

        # print movie data
        for m in mlist:
            self.handler.row([m.votes, m.rating,
                              m.year, "    "+m.title])


def main():
    mlist = []
    for line in open("ratings.list"):
        record = GroupMovieByElements(line)
        if condition(record):
            mlist.append(record)

    handler = TextTableHandler()
    printer = MoviePrinter(handler)
    printer.print_movie_table(mlist)


if __name__ == "__main__":
    main()

