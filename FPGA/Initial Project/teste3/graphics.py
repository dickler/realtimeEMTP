import matplotlib.pyplot as plt
import teste3 as t3

arq = open('psim_data.txt', 'r')

t = []
v1 = []
v2 = []
If = []
for linha in arq:
    linha = linha.split()
    t.append(float(linha[0]))
    If.append(float(linha[1]))
    v1.append(float(linha[2]))
    v2.append(float(linha[3]))
arq.close()
print(len(t))
plt.plot(t,v1,t,v2,t,If,t,t3.V1,t,t3.V2,t,t3.I1,)
plt.legend(('V1_PSIM','V2_PSIM','I1_PSIM','V1_Py','V2_Py','I1_Py',))
plt.grid()
plt.show()
    
