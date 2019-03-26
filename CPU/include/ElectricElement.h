#pragma once
#include "Circuit.h"

/*
ElectricElement is an abstract class capable of polymorphism so any element can be
stamped with no problem

Has only the stamp functions, a value which the stamp functions will handle,
a dissolver function that will delete object and a Name.
*/

class ElectricElement {

protected:

public:

	double value;
	void ObjectDissolver();
    virtual void stamp(Circuit& circuit) = 0;
	virtual void stampRightHand(Circuit& circuit) = 0;
    virtual std::string classQuery() = 0;
    virtual std::string nodesQuery() = 0;
    virtual double getVoltage(Circuit& circuit);
    virtual double getCurrent(Circuit& circuit) = 0;
	std::string Name;

    std::vector<double*> Voltage;
    virtual void linkCircuit(Circuit& circuit);
    std::vector<int> nodes;

};



