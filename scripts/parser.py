import sys
import os

f_in = open("data/sampleFICT.txt", "r")
f_out = open("data/texto-aux.txt", "w")

for line in f_in:
    for word in line.split():
        f_out.write(word+"\n")
    
f_in.close()
f_out.close()