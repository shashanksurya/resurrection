#!/usr/bin/env python3.4
import levenshtein as lnsh
import re


class Movie(object):

    def __init__(self, votes, rank, year, title):
        self.votes = votes
        self.rank = rank
        self.year = year
        self.title = title


def condition(movie, titles):
    if float(movie.rank) >= 8.0:
        if int(movie.votes) >= 1000:
            for needed in titles:
                if lnsh.levenshtein(movie.title, needed) <= 3:
                    #print('is less than 3')
                    return True
    return False


def GroupMovieByElements(line):
    reg = r'( ){6}[0-9.]*.*'
    match = re.findall(reg, line)
    movie_attr = []
    # omit lines which are episodes by placing }
    if match and line.find("}") == -1:
        # print(line)
        reg = r'[0-9.]{10}[ ]*\d*'
        match = re.findall(reg, line)
        if match:
            votes = str(match[0])
            if len(votes.split()) == 2:
                movie_attr.append(votes.split()[1])
        reg = r'[A-Za-z:ÁÂàâçéèêëîïôûùüÿñæœ].*[(\d)]{4}'
        match = re.findall(reg, line)
        if match:
            movie = str(match[0])
            movie_attr.append(movie[:-6])
            movie_attr.append(movie[-5:-1])
        reg = r'\d\.\d'
        match = re.findall(reg, line)
        if match:
            movie_attr.append(match[0])
        # print(movie_attr)
    if len(movie_attr) == 4:
        m = Movie(movie_attr[0], movie_attr[3], movie_attr[2], movie_attr[1])
        #print('returning m '+m.title)
        return m
