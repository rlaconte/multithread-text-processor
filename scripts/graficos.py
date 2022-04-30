import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import sys

f_tpos = open("exp1.txt", "r")

filas = []
#cantCores = [8,4]
#for i in cantCores:
for i in range(0,8):
    fila_i = []
    for j in range(0,20):    
        #fila_i.append(i+1)
        #fila_i.append(j)
        fila_i.append(float(f_tpos.readline()))
    filas.append(fila_i)

print(filas)

#df_carga = pd.DataFrame(filas, columns = ['cant-threads', 'tiempo'])

# filas = []
# for i in range(0,26):
#     fila_i = []
#     fila_i.append(i+1)
#     fila_i.append(float(f_tpos.readline()))
#     filas.append(fila_i)

#df_max = pd.DataFrame(filas, columns = ['cant-threads', 'tiempo'])

if sys.argv[1] == "carga":
    
    # fig = sns.lineplot(data=df_carga, x='cant-threads' , y='tiempo')
    # #fig = sns.lineplot(data=df_carga[df_carga['cant-cores']==8], x='cant-threads' , y='tiempo')

    # #fig.legend(labels=['cant-cores=4', 'cant-cores=8'])
    #plt.xticks(np.arange(1,9))

    fig1, ax1 = plt.subplots()
    ax1.boxplot(filas)
    ax1.set(xlabel='cant-threads', ylabel='tiempo (s)')
    fig1.savefig("exp-carga.png")


if sys.argv[1] == "max":
    
    fig = sns.lineplot(data=df_max, x='cant-threads' , y='tiempo')
    fig.set(xlabel='cant-threads', ylabel='tiempo (s)')
    plt.savefig("exp-max.png")

f_tpos.close()