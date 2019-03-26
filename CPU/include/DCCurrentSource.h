#pragma once
#include "ElectricElement.h"
#include "Circuit.h"

//
//The Current element is made of the nodes it connects to, the inheritance
//from the ElectricElement and its form of stamps.
//

class DCCurrentSource :
	public ElectricElement
{
private:



public:
	DCCurrentSource(double Value, int Node1, int Node2);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    std::string classQuery() override {return "DCC";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;
};
