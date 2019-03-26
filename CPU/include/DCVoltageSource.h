#pragma once
#include "ElectricElement.h"
#include "Circuit.h"


//
//The Voltage element is made of the nodes it connects to, the inheritance
//from the ElectricElement, its form of stamps and an unique ID.
//

class DCVoltageSource : public ElectricElement {

public:
	DCVoltageSource(double Value, int Node1, int Node2, Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    std::string classQuery() override {return "DCV";}
    std::string nodesQuery() override;
    int ID;
    double getCurrent(Circuit& circuit) override;
};
