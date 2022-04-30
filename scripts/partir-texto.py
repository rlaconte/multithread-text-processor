import sys
import os

f_in = open("data/texto-aux.txt", "r")
nro_files = 20

for i in range(0, nro_files):
    f_out = open(F"data/texto-{i}.txt", "w")
    count = 0
    while count<50000:
        line = f_in.readline()
        for word in line.split():
            f_out.write(word+"\n")
            count = count+1
    f_out.close()
    
f_in.close()