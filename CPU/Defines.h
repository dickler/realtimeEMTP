#pragma once
/*Defines go here*/
//#define OptimizeG
//#define EXPANDED_SWITCH
//#define MNA_CAPACITOR
//#define MNA_INDUCTOR
#define IdealSwitch


/*Defines go up to here*/

#ifdef IdealSwitch
	#ifdef EXPANDED_SWITCH
		#undef EXPANDED_SWITCH
	#endif
#endif

#ifdef OptimizeG
	#define EXPANDED_SWITCH
#endif


