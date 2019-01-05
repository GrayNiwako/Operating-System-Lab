# -*- coding: utf8 -*-

import os

Path = os.path.dirname(os.path.realpath(__file__))

structure = "hash_bucket_10"

f = open(Path + "\\" + structure + ".txt", 'r')
Lines = f.readlines()
f.close()
Data = []
lock = []
i = 0
for line in Lines:
    temp1 = line.strip('\n')
    temp2 = temp1.split(' ')
    if i % 2 == 0:
        temp3 = temp2[-1].split(':')
        lock.append(temp3[0])
    else:
        temp2 = temp2[:-1]
        Data.append(temp2)
    i += 1

fp = open(Path + "\\" + structure + "_x.txt", 'w')

fp.write('bucket_number')
for i in range(10):
    fp.write(' ' + str(10 * (i + 1)))
fp.write('\n')
for i in range(8):
    fp.write(lock[i])
    for j in range(10):
        fp.write(' ' + Data[i][j])
    fp.write('\n')