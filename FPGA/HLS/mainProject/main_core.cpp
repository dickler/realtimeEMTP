#include "hls_math.h"
#include "main_core.h"

static VECTOR_I G_vec_I[N];
static VECTOR_V G_vec_V[N];

void sinGen(void){
	#pragma HLS pipeline

	static I_SOURCE i_sources[I_SOURCES] = {};

	static V_SOURCE sources[V_SOURCES] = {{ 0.0,  0.0, 0.0,  20.0}};

	loop_1:for(unsigned char i = 0; i < V_SOURCES; i++){
		G_vec_I[i + NODES] = sources[i].DClevel + sources[i].amplitude*sin(sources[i].phase);

		if(sources[i].dtheta + sources[i].phase < 2*PI){
			sources[i].phase = sources[i].phase + sources[i].dtheta;
		}
		else{
			sources[i].phase = sources[i].phase + sources[i].dtheta - 2*PI;
		}
	}
	unsigned char k, m;
	loop_2:for(unsigned char j = 0; j < I_SOURCES; j++){
		k = i_sources[j].n_out;
		m = i_sources[j].n_in;

		if(k != 0){
			G_vec_I[k-1] -= i_sources[j].DClevel + i_sources[j].amplitude*sin(i_sources[j].phase);
		}
		if(m != 0){
			G_vec_I[m-1] += i_sources[j].DClevel + i_sources[j].amplitude*sin(i_sources[j].phase);
		}

		if(i_sources[j].dtheta + i_sources[j].phase < 2*PI){
			i_sources[j].phase = i_sources[j].phase + i_sources[j].dtheta;
		}
		else{
			i_sources[j].phase = i_sources[j].phase + i_sources[j].dtheta - 2*PI;
		}
	}
}

void vetmat(int s){
	#pragma HLS pipeline

	const MATRIX_G Gi[MATRICES_NUMBER][N][N] =
  {{{ 0.0, 					  0.0,					  0.0,					  1.0,		   		      0.0,  0.0},
	{ 0.0,  0.0004999373828427989,  0.0004999373828427989,  2.499686914213994e-07,     0.9999997500313086,  0.0},
	{ 0.0,  0.0004999373828427989,  0.0004999373828427989,  2.499686914213994e-07, -2.499686914213994e-07,  0.0},
	{ 1.0,  2.499686914213994e-07,  2.499686914213994e-07, -0.0004999998750156542,  0.0004999998750156542,  0.0},
	{ 0.0, 	   0.9999997500313086, -2.499686914213994e-07,  0.0004999998750156542, -0.0004999998750156542,  0.0},
	{ 0.0,					  0.0,  				  0.0,  				  0.0,  				  0.0,  1.0}},

   {{ 0.0,  0.0,  					0.0,  	 1.0,  0.0,  	0.0},
	{ 0.0,  0.0,  					0.0,  	 0.0,  0.0,  	1.0},
	{ 0.0,  0.0,  0.0004999375078115235, 	 0.0,  0.0,  	0.0},
	{ 1.0,  0.0,  					0.0, -0.0005,  0.0,  0.0005},
	{ 0.0,  0.0,  					0.0,  	 0.0,  1.0,	    0.0},
	{ 0.0,  1.0,  					0.0,  0.0005,  0.0, -0.0005}}};

	float acc;
	loop_1:for(unsigned char i = 0; i<N; i++){
		acc = 0;
		loop_2:for(unsigned char j = 0; j<N; j++){
			acc += Gi[s][i][j]*G_vec_I[j];
			if(i == N-1){
				G_vec_I[j] = 0.0;
			}
		}
		G_vec_V[i] = acc;
	}
}

void histvect(void){
	#pragma HLS pipeline
	static LC_ELEMENT elements[N_LC] = {{0, 1, 2, 0.001, 0.0},
									   {1, 3, 0, 4000.0, 0.0}};

	unsigned char k, m;
	float Vk, Vm;
	for(unsigned char i = 0; i < N_LC; i++){
		k = elements[i].n_out;
		m = elements[i].n_in;

		if(k == 0) Vk = 0.0;
		else Vk = G_vec_V[k-1];

		if(m == 0) Vm = 0.0;
		else Vm = G_vec_V[m-1];

		switch (elements[i].type){
		case 1: //capacitor case
			elements[i].Ih = -elements[i].Ih - elements[i].g*(Vk - Vm);
			break;
		case 0: //inductor case
			elements[i].Ih = elements[i].Ih + elements[i].g*(Vk - Vm);
			break;
		}

		if(k != 0) G_vec_I[k-1] -= elements[i].Ih;
		if(m != 0) G_vec_I[m-1] += elements[i].Ih;

	}
}

OUTPUT nodalSolver(int s){
	sinGen();
	vetmat(s);
	// export range: [-128.0, 128)
	OUTPUT out = {nearbyint(G_vec_V[A]*256 + 32768),
				  nearbyint(G_vec_V[B]*256 + 32768),
				  nearbyint(G_vec_V[C]*256 + 32768),
				  nearbyint(G_vec_V[D]*256 + 32768)};
	histvect();
	return out;
}


