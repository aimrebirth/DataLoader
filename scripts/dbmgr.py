#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import json
import re
import sqlite3

values_key = 'values'
fks_key = 'fks'
sql_key = 'sql'

def main():    
    parser = argparse.ArgumentParser(description='Polygon-4 DB Manager')
    parser.add_argument('--db', dest='db', help='database name: db.sqlite')
    parser.add_argument('--json', dest='json', help='json name: db.json')
    parser.add_argument('--dump', dest='dump', action='store_true', help='save to json')
    parser.add_argument('--load', dest='load', action='store_true', help='load from json')
    parser.add_argument('--merge', dest='merge', nargs='+', help='merge json files into single: db1.json db2.json ... dbN.json')
    parser.add_argument('--tables', dest='tables', nargs='*', help='load only these tables: table1.json table2.json ... tableN.json')
    parser.add_argument('--clear', dest='clear', action='store_true', help='clear the database first')
    parser.add_argument('--clearfield', dest='clearfield', nargs=2, help='clear table field: table field')
    pargs = parser.parse_args()
    
    if pargs.tables:
        for table in pargs.tables:
            table = table.lower()

    if pargs.clear:
        clear(pargs.db)

    if pargs.dump:
        dump(pargs.db, pargs.json, pargs.tables)

    if pargs.load:
        load(pargs.db, pargs.json)

    if pargs.merge:
        merge(pargs.json, pargs.merge)

    if pargs.clearfield:
        clearfield(pargs.json, pargs.clearfield[0], pargs.clearfield[1])

def clearfield(fn, table, field):
    data = json.load(open(fn))
    for t in sorted(data):
        if t.lower() != table.lower():
            continue
        d = []
        for row in data[t][values_key]:
            r = dict()
            for k in row:
                if k.lower() != field.lower():
                    r[k] = row[k]
            d.append(r)
        data[t][values_key] = d
    json.dump(data, open(fn, 'w'), sort_keys = True, indent = 2)

def clear(db):
    conn = sqlite3.connect(db)
    c = conn.cursor()
    c.execute('PRAGMA foreign_keys = OFF;')
    c.execute('''select * from sqlite_master WHERE type = 'table';''')
    for table in c.fetchall():
        name = table[2]
        if name == 'Tables':
            continue
        c.execute('delete from ' + name + ';')
    conn.commit()
    conn.close()

def dict_factory(cursor, row):
    d = {}
    for idx, col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d

def dump(db, fn, tables):
    conn = sqlite3.connect(db)
    conn.row_factory = dict_factory
    c = conn.cursor()
    data = {}
    c.execute('''select * from sqlite_master WHERE type = 'table';''')
    for table in c.fetchall():
        name = table['name']
        if name == 'Tables':
            continue

        if tables:
            found = False
            for t in tables:
                if re.match(t.lower(), name.lower()):
                    found = True
                    break
            if not found:
                continue

        c.execute('select * from ' + name)
        rows = c.fetchall()
        if len(rows) == 0:
            continue

        tdata = []
        for row in rows:
            tdata.append(row)
        
        print('dumping: ' + name)

        fks = []
        c.execute('PRAGMA foreign_key_list(' + name + ');')
        for key in c.fetchall():
            fks.append(key)

        d = dict()
        d[values_key] = tdata
        d[sql_key] = table['sql']
        d[fks_key] = fks
        data[name] = d
    conn.close()
    json.dump(data, open(fn, 'w'), sort_keys = True, indent = 2)

def load(db, fn):
    data = json.load(open(fn))
    conn = sqlite3.connect(db)
    c = conn.cursor()
    c.execute('PRAGMA foreign_keys = OFF;')
    for table in data:
        if len(data[table][values_key]) == 0:
            continue
        print('loading: ' + table)
        c.execute('delete from ' + table + ';')
        s = 'insert into ' + table + ' ({0}) values '
        for row in data[table][values_key]:
            f = ''
            for field in row.keys():
                f += "'" + field + "'" + ', '
            f = f[:-2]
            sql = s.format(f)
            f = ''
            for field in row.keys():
                f += "'" + str(row[field]) + "'" + ', '
            f = f[:-2]
            sql = sql + '(' + f + ');'
            c.execute(sql)
    conn.commit()
    conn.close()

def merge(fn, files):
    if len(files) < 2:
        print('Supply two or more input files')
        return

    first = json.load(open(files[0]))
    for i in range(1, len(files)):
        print('merging: ' + files[i])
        second = json.load(open(files[i]))
        do_merge(first, second)

    json.dump(first, open(fn, 'w'), sort_keys = True, indent = 2)
    
def do_merge(first, second):
    maxId = dict()
    maxIdSecond = dict()
    maxId['tables'] = 0
    maxIdSecond['tables'] = 0

    # find max ids on first table
    for table in sorted(first):
        maxId[table.lower()] = 0
        hasId = False
        if len(first[table][values_key]) > 0:
            hasId = 'id' in first[table][values_key][0]
        if hasId:
            for row in first[table][values_key]:
                maxId[table.lower()] = max(maxId[table.lower()], row['id'])

    # find max ids on second table
    for table in sorted(second):
        maxIdSecond[table.lower()] = 0
        hasId = False
        if len(second[table][values_key]) > 0:
            hasId = 'id' in second[table][values_key][0]
        if hasId:
            for row in second[table][values_key]:
                maxIdSecond[table.lower()] = max(maxIdSecond[table.lower()], row['id'])

    # apply max ids
    for table in sorted(second):
        hasId = False
        if len(second[table][values_key]) > 0:
            hasId = 'id' in second[table][values_key][0]
        for row in second[table][values_key]:
            # increase id in the same subsequent tables
            if hasId:
                if row['id'] != 0:
                    row['id'] += maxId[table.lower()]
            # increase foreign keys only if the current (second) table
            #  brings new keys to the target table
            for field in row:
                for fk in second[table][fks_key]:
                    if fk['from'] == field:
                        if row[field] != 0:
                            if fk['table'].lower() in maxIdSecond and maxIdSecond[fk['table'].lower()] != 0:
                                row[field] += maxId[fk['table'].lower()]

    # write second to first
    for table in sorted(second):
        for row in second[table][values_key]:
            first[table][values_key].append(row)

if __name__ == '__main__':
    main()
