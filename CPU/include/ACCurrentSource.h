#pragma once
#include "ElectricElement.h"


/*
AC Current source has its inherited members, the usual AC parameters and the nodes it is connected to.
*/

class ACCurrentSource : public ElectricElement {


private:
	double StampRightHand;
	double Phase;
	double Frequency;


public:
	ACCurrentSource(double Value, double Frequency, double Phase, int Node1, int Node2, Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    std::string classQuery() override {return "ACC";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;



};
