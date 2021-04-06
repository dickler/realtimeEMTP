#include "stdio.h"

#define N_LC 2				 // defines the number of LC elements
#define I_SOURCES 0			 // defines the number of current sources in the circuit
#define V_SOURCES 1          // defines the number of voltage sources in the circuit
#define NODES 3				 // defines the number of nodes in the circuit (except node 0)
#define MATRICES_NUMBER 2    /* defines the number of different possibles matrices. Each switch
                                makes 2 new possibilities, so MATRICES_NUMBER = 2^SWITCHS_NUMBER
                             */
#define N 6	   				 // defines the matrix G order (N = NODES + V_SOURCES + SWITCHS_NUMBER)

#define PI 3.1415926

//it refers to the output address for the require information
#define A 2
#define B 0
#define C 1
#define D 3

typedef float MATRIX_G;
typedef float VECTOR_I;
typedef float VECTOR_V;

struct OUTPUT{
	int a;
	int b;
	int c;
	int d;
};

struct V_SOURCE{
	float dtheta;
	float amplitude;
	float phase;
	float DClevel;
};

struct I_SOURCE{
	unsigned char n_out;
	unsigned char n_in;
	float dtheta;
	float amplitude;
	float phase;
	float DClevel;

};

struct LC_ELEMENT{
	unsigned char type; //0 to inductor and 1 to capacitor
	unsigned char n_out;
	unsigned char n_in;
	float g;
	float Ih;
};
