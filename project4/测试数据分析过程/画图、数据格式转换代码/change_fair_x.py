# -*- coding: utf8 -*-

import os

Path = os.path.dirname(os.path.realpath(__file__))

structure = "fairlock_fair"

f = open(Path + "\\" + structure + ".txt", 'r')
Lines = f.readlines()
f.close()
Data = []
lock = []
i = 0
j = 0
temp5 = []
for line in Lines:
    temp1 = line.strip('\n')
    temp2 = temp1.split(' ')
    if i % 21 == 0:
        temp3 = temp2[-1].split(':')
        lock.append(temp3[0])
    else:
        temp4 = float(temp2[-1])
        temp5.append(temp4)
        j += 1
    if j == 20:
        Data.append(temp5)
        j = 0
        temp5 = []
    i += 1

fp = open(Path + "\\" + structure + "_x.txt", 'w')

fp.write('thread_rank')
for i in range(20):
    fp.write(' ' + str(i + 1))
fp.write('\n')
for i in range(2):
    fp.write(lock[i])
    for j in range(20):
        fp.write(' ' + str(Data[i][j]))
    fp.write('\n')