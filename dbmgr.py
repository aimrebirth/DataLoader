#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import json
import sqlite3

import dbmgr_data

def main():    
    parser = argparse.ArgumentParser(description='Polygon-4 DB Manager')
    parser.add_argument('--dump', dest='dump', nargs=2, help='dump database: db.sqlite db.json')
    parser.add_argument('--load', dest='load', nargs=2, help='load database: db.json db.sqlite')
    parser.add_argument('--merge', dest='merge', nargs=2, help='merge database: db1.json db2.json')
    pargs = parser.parse_args()

    if pargs.dump:
        dump(pargs.dump)

    if pargs.load:
        load(pargs.load)

    if pargs.merge:
        merge(pargs.merge)

def dict_factory(cursor, row):
    d = {}
    for idx, col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d

def dump(fn):
    conn = sqlite3.connect(fn[0])
    conn.row_factory = dict_factory
    c = conn.cursor()
    data = {}
    for table in dbmgr_data.tables:
        tdata = []
        for row in c.execute('select * from ' + table):
            tdata.append(row)
        data[table] = tdata
    conn.close()
    json.dump(data, open(fn[1], 'w'), sort_keys = True, indent = 2)

def load(fn):
    data = json.load(open(fn[0]))
    conn = sqlite3.connect(fn[1])
    c = conn.cursor()
    c.execute('PRAGMA foreign_keys = OFF;')
    for table in data:
        c.execute('delete from ' + table + ';')
        if len(data[table]) == 0:
            continue
        s = 'insert into ' + table + ' ({0}) values '
        f = ''
        for row in data[table][0]:
            f += row + ', '
        f = f[:-2]
        s = s.format(f)
        for row in data[table]:
            f = ''
            for field in row.keys():
                f += "'" + str(row[field]) + "'" + ', '
            f = f[:-2]
            s += '(' + f + '),'
        s = s[:-1] + ';'
        c.executemany(s, [])
    conn.close()

def merge(fn):
    data = [json.load(open(fn[0])), json.load(open(fn[1]))]

if __name__ == '__main__':
    main()
