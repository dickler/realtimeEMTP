#pragma once
#include "ElectricElement.h"

class DynamicElement :
	public ElectricElement
{
public:
	double Ih;
    double Ihh;
	double Value0=0;
    virtual void trapStamp(Circuit& circuit)=0;
	virtual void initialize(Circuit& circuit) = 0;

};

