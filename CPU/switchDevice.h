#pragma once
#include "ElectricElement.h"
class switchDevice :
	public ElectricElement
{
	public:
		virtual void idealStamp(Circuit& circuit) = 0;
		double* V1;
		double* V2;
		double Ih;
		double S = 1;
};

