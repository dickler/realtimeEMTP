#pragma once
#include "ElectricElement.h"
class switchDevice :
	public ElectricElement
{
	public:
        int flag;
		virtual void idealStamp(Circuit& circuit) = 0;
        virtual void calculateState() = 0;
		double Ih;
		double S = 1;
};

