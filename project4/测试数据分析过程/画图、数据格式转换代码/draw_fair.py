# -*- coding: utf8 -*-

import matplotlib.pyplot as plt
import os

Path = os.path.dirname(os.path.realpath(__file__))

structure = "fairmutex_fair"

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


x = []
for i in range(20):
    x.append(i + 1)
plt.plot(x, Data[0], color='gold', marker='o', markersize=6, label=lock[0])
plt.plot(x, Data[1], color='skyblue', marker='o', markersize=6, label=lock[1])
# plt.plot(x, Data[2], color='pink', marker='o', markersize=6, label=lock[2])
# plt.plot(x, Data[3], color='palegreen', marker='o', markersize=6, label=lock[3])
# plt.plot(x, Data[4], color='mediumpurple', marker='o', markersize=6, label=lock[4])
# plt.plot(x, Data[5], color='lightcoral', marker='o', markersize=6, label=lock[5])
plt.legend(bbox_to_anchor=(0.94, 0.6))

# y_max = plt.ylim()[1]
# plt.ylim(0, y_max)
plt.ylim(192, 194)

x_names = ['0', '4', '8', '12', '16', '20']
x_position = [0, 4, 8, 12, 16, 20]
plt.xticks(x_position, x_names, fontsize=12)
plt.yticks(fontsize=12)
plt.subplots_adjust(bottom=0.15, left=0.2)

font1 = {'fontsize': 15, 'verticalalignment': 'bottom', 'horizontalalignment': 'center'}
font2 = {'fontsize': 15, 'verticalalignment': 'top', 'horizontalalignment': 'center'}
plt.title(structure, fontdict=font1)
plt.xlabel('Thread_rank', fontdict=font2)
plt.ylabel('Time(seconds)', fontdict=font1)

plt.savefig(Path + "\\" + structure + '.png')
plt.show()