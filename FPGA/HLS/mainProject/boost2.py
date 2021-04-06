import numpy as np
import scipy.signal
import matplotlib.pyplot as plt

dt = 1e-6
fChave = 10e3
Tchave = 1/fChave
Vin = 20
L = 1e-3
C = 1e-3
R = 4.0

ReqL = 2*L/dt
ReqC = dt/(2*C)

tmax = 0.06
t = np.arange(0, tmax, dt)
Vout = np.arange(0, tmax, dt)
squareWave = np.abs((scipy.signal.square(2*np.pi*fChave*(t-Tchave/4))-1)/2)
s = squareWave == 1

#### MNA ####
G_on = np.matrix([[ 1/ReqL,           0.0, 1.0],
                  [    0.0,  1/ReqC + 1/R, 0.0],
                  [    1.0,           0.0, 0.0]])
G_on_inv = np.linalg.inv(G_on)

G_off = np.matrix([[  1/ReqL,       -1/ReqL, 1.0],
                   [ -1/ReqL,  1/ReqC + 1/R, 0.0],
                   [     1.0,           0.0, 0.0]])
G_off_inv = np.linalg.inv(G_off)

iL = 0.0
iC = 0.0
I = np.matrix([[0.0],
               [0.0],
               [Vin]])
V = np.matrix([[0.0] for i in range(3)])

for i in range(len(t)):
    on = s[i]
    off = not on
    if on:
        I[0] = -iL
        I[1] = -iC
        V = G_on_inv*I
        
        vL = V[0]
        iL = iL + vL*2/ReqL
        
        vC = V[1]
        iC = -iC - vC*2/ReqC
        
    elif off:
        I[0] = -iL
        I[1] = iL - iC
        V = G_off_inv*I
        
        vL = V[0] - V[1]
        iL = iL + vL*2/ReqL
        
        vC = V[1]
        iC = -iC - vC*2/ReqC
    Vout[i] = V[1]

#### MANA ####
G_off = np.matrix([[  1/ReqL,  -1/ReqL,           0.0, 1.0,  0.0, 0.0],
                   [ -1/ReqL,   1/ReqL,           0.0, 0.0,  1.0, 0.0],
                   [     0.0,      0.0,  1/ReqC + 1/R, 0.0, -1.0, 0.0],
                   [     1.0,      0.0,           0.0, 0.0,  0.0, 0.0],
                   [     0.0,      1.0,          -1.0, 0.0,  0.0, 0.0],
                   [     0.0,      0.0,           0.0, 0.0,  0.0, 1.0]])
G_off_inv = np.linalg.inv(G_off)

G_on =  np.matrix([[  1/ReqL,  -1/ReqL,           0.0, 1.0,  0.0, 0.0],
                   [ -1/ReqL,   1/ReqL,           0.0, 0.0,  0.0, 1.0],
                   [     0.0,      0.0,  1/ReqC + 1/R, 0.0,  0.0, 0.0],
                   [     1.0,      0.0,           0.0, 0.0,  0.0, 0.0],
                   [     0.0,      0.0,           0.0, 0.0,  1.0, 0.0],
                   [     0.0,      1.0,           0.0, 0.0,  0.0, 0.0]])
G_on_inv = np.linalg.inv(G_on)

Vout2 = np.arange(0, tmax, dt)
Vout2 = Vout2.reshape((len(Vout2),1))
Vout2 = np.hstack((Vout2, Vout2, Vout2, Vout2))

iL = 0.0
iC = 0.0
I = np.matrix([[0.0],
               [0.0],
               [0.0],
               [Vin],
               [0.0],
               [0.0]])
V = np.matrix([[0.0] for i in range(6)])

for i in range(len(t)):
    on = s[i]
    off = not on
    
    I[0] = -iL
    I[1] = iL
    I[2] = -iC

    #print(on, I[0], I[1], I[2], I[3])
    if on:
        V = G_on_inv*I
    elif off:
        V = G_off_inv*I

    #print(on, V[0], V[1], V[2], V[3])
    vL = V[0] - V[1]
    iL = iL + vL*2/ReqL
    
    vC = V[2]
    iC = -iC - vC*2/ReqC
    
    Vout2[i,0] = V[0]
    Vout2[i,1] = V[1]
    Vout2[i,2] = V[2]
    Vout2[i,3] = V[3]

data = np.loadtxt('boost2.txt', dtype=int)
Vout3 = data[:,1]
Vout3 = (Vout3-2**15)/256

plt.plot(t, Vout, label = 'MNA')
plt.plot(t, Vout2[:,2], label = 'MANA')
plt.plot(t, Vout3, label = 'MANA HLS/C++')
plt.grid()
plt.legend()
plt.show()

#plt.plot(t, Vout2[:,0], label = 'V fonte')
#plt.plot(t, Vout2[:,1], label = 'V pré diodo')
plt.plot(t, Vout2[:,2], label = 'V saída')
plt.plot(t, -Vout2[:,3], label = 'corrente na fonte')
plt.grid()
plt.legend()
plt.show()


