# -*- coding: utf8 -*-

import matplotlib.pyplot as plt
import os

Path = os.path.dirname(os.path.realpath(__file__))

structure = "hash_bucket_7_single"

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
        temp2 = [float(temp2[j]) for j in range(len(temp2))]
        Data.append(temp2)
    i += 1

x = []
for i in range(7):
    x.append(i + 1)
plt.plot(x, Data[0], color='gold', marker='o', markersize=6, label=lock[0])
plt.plot(x, Data[1], color='skyblue', marker='o', markersize=6, label=lock[1])
plt.plot(x, Data[2], color='pink', marker='o', markersize=6, label=lock[2])
plt.plot(x, Data[3], color='palegreen', marker='o', markersize=6, label=lock[3])
# plt.plot(x, Data[4], color='mediumpurple', marker='o', markersize=6, label=lock[4])
# plt.plot(x, Data[5], color='lightcoral', marker='o', markersize=6, label=lock[5])
# plt.plot(x, Data[6], color='lightgrey', marker='o', markersize=6, label=lock[6])
# plt.plot(x, Data[7], color='lightsalmon', marker='o', markersize=6, label=lock[7])
# plt.plot(x, Data[8], color='violet', marker='o', markersize=6, label=lock[8])
# plt.plot(x, Data[9], color='aquamarine', marker='o', markersize=6, label=lock[9])
# plt.plot(x, Data[10], color='orange', marker='o', markersize=6, label=lock[10])
# plt.plot(x, Data[11], color='teal', marker='o', markersize=6, label=lock[11])
# plt.plot(x, Data[12], color='royalblue', marker='o', markersize=6, label=lock[12])
# plt.plot(x, Data[4], color='gold', marker='*', markersize=9, label=lock[4])
# plt.plot(x, Data[5], color='skyblue', marker='*', markersize=9, label=lock[5])
# plt.plot(x, Data[6], color='pink', marker='*', markersize=9, label=lock[6])
# plt.plot(x, Data[7], color='palegreen', marker='*', markersize=9, label=lock[7])
plt.legend(bbox_to_anchor=(0.6, 0.6))

y_max = plt.ylim()[1]
plt.ylim(0, y_max)

x_names = ['0', '10', '100', '1000', '10000', '10^5', '10^6', '10^7']
x_position = [0, 1, 2, 3, 4, 5, 6, 7]
plt.xticks(x_position, x_names, fontsize=12)
plt.yticks(fontsize=12)
plt.subplots_adjust(bottom=0.15, left=0.15)

font1 = {'fontsize': 15, 'verticalalignment': 'bottom', 'horizontalalignment': 'center'}
font2 = {'fontsize': 15, 'verticalalignment': 'top', 'horizontalalignment': 'center'}
plt.title(structure, fontdict=font1)
plt.xlabel('Buckets', fontdict=font2)
plt.ylabel('Time(seconds)', fontdict=font1)

plt.savefig(Path + "\\" + structure + '.png')
plt.show()