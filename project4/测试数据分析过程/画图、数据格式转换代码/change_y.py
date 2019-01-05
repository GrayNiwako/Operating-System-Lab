# -*- coding: utf8 -*-

import os

Path = os.path.dirname(os.path.realpath(__file__))

structure = "counter"

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

fp = open(Path + "\\" + structure + "_y.txt", 'w')

fp.write('thread_number')
for i in range(4):
    fp.write(' ' + lock[i])
fp.write('\n')
for i in range(20):
    fp.write(str(i + 1))
    for j in range(4):
        fp.write(' ' + Data[j][i])
    fp.write('\n')