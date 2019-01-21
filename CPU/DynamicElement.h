#pragma once
#include "ElectricElement.h"

class DynamicElement :
	public ElectricElement
{
public:
	double* V1;
	double* V2;
	double Ih;
	double Value0=0;

	virtual void initialize(Circuit& circuit) = 0;
};

