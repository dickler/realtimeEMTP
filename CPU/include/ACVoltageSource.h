#pragma once
#include "ElectricElement.h"


/*
AC Voltage source has its inherited members, an unique ID, the usual AC parameters and the nodes it is connected to.
*/
class ACVoltageSource : public ElectricElement {


private:

	double StampRightHand;
	double Phase;
	double Frequency;


public:
	ACVoltageSource(double Value, double Frequency, double Phase, int Node1, int Node2, Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    std::string classQuery() override {return "ACV";}
    std::string nodesQuery() override;
    int ID;
    double getCurrent(Circuit& circuit) override;



};
