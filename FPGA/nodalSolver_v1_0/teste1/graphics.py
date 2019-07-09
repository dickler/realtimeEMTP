import matplotlib.pyplot as plt
import teste1 as t1

arq = open('psim_data', 'r')

t = []
v1 = []
v2 = []
v3 = []
If = []
for linha in arq:
    linha = linha.split()
    t.append(float(linha[0]))
    If.append(float(linha[1]))
    v1.append(float(linha[2]))
    v2.append(float(linha[3]))
    v3.append(float(linha[4]))
arq.close()
print(len(t))
plt.plot(t,v1,t,v2,t,v3,t,If)#,t,t1.V1,t,t1.V2,t,t1.V3,t,t1.I1,)
plt.legend(('V1_PSIM','V2_PSIM','V3_PSIM','I1_PSIM'))#,'V1_Py','V2_Py','V3_Py','I1_Py',))
plt.grid()
plt.show()
    
