import numpy as np
import ieee754 as ie

G = np.array([[2.0, -2.0, 0.0, 1.0],
               [-2, 2.05, -0.05, 0.0],
               [0.0, -0.05, 1.05, 0.0],
               [1.0, 0.0, 0.0, 0.0]], dtype = np.float64)
G = np.matrix(G)
Gi = np.linalg.inv(G)
#print(Gi)
#print('\n'*2)
'''
arq = open('matrix_teste1.txt','w')
for i in range(4):
    for j in range(4):
        s = ie.floatToIEEE754(Gi[i,j])+'\n'
        arq.write(s)
arq.close()
print('finish')
'''
I = np.matrix(np.array([[0.],[0.],[0.],[10.]]))
V = np.matrix(np.array([[0.],[0.],[0.],[0.]]))
Ih = np.matrix(np.array([[0.],[0.]]))
elements = [[1,0,1,0.5],[0,1,2,20.]]

def calcIH(i,c,k,m,req):
    result = Ih[i,0]+2/req*(V[k,0]-V[m,0])
    if c == 0:
        return result
    elif c == 1:
        return -result

if __name__ == '__main__':
    n = 50
else:
    n = 50
I1 =[]
V1 = []
V2 = []
V3 = []

def mult(A,b):
    x =[]
    #print('------------'*4)
    for i in range(len(A)):
        s = 0
        for j in range(len(A)):
            k = A[i,j]
            f = b[j,0]
            s += k*f
            #print (s)
        x.append([s])
        #print()
    return np.matrix(np.array(x))
        
for x in range(n):
    Ih[0,0] = -(V[0,0]-V[1,0])*4-Ih[0,0]
    Ih[1,0] = (V[1,0]-V[2,0])*0.1+Ih[1,0]
    I[0,0] = -Ih[0,0]
    I[1,0] = Ih[0,0] - Ih[1,0]
    I[2,0] = Ih[1,0]
    V = mult(Gi,I)

    V1.append(V[0,0])
    V2.append(V[1,0])
    V3.append(V[2,0])
    I1.append(V[3,0])
    '''
    print(Ih)
    print()
    print(Gi)
    print(I)
    print()
    print (V)
    '''
    '''
    print(x,'---',x+1)
    print(V)
    '''
print(V)
