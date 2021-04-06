#include "main_core.h"

OUTPUT nodalSolver(int s);

int main(){
	OUTPUT outprint;
	int s = 0;
	for(int i = 0; i < 60; i++){
		outprint = nodalSolver(s);
		if (i%50 == 0 & i != 0){
			if (s==0) s = 1;
			else s = 0;
		}
		//printf("%d %d %d %d %d\n", s, outprint.a, outprint.b, outprint.c, outprint.d);
	}

	printf("V1 = %d\n", outprint.a);
	printf("V2 = %d\n", outprint.b);
	printf("V3 = %d\n", outprint.c);
	printf("V4 = %d\n", outprint.d);
	printf("\n");

	// teste boost
	if((outprint.a != 32775)||(outprint.b != 37888)||
	   (outprint.c != 32768)||(outprint.d != 32463)){
	    printf("deu ruim");
		return 1;
	}
	return 0;
}
