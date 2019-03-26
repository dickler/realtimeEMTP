#pragma once
#include "ElectricElement.h"
#include "Circuit.h"

/*
AC Voltage source has its inherited members, an unique ID, phase, period, offset and DutyCycle(DC) parameters and the nodes it is connected to.
*/
class SquareWave :
	public ElectricElement{

private:

	double StampRightHand;
	double Phase;
	double Frequency;
	double OFFSET;
	double DC;


public:

	SquareWave(double Value, double Frequency, int Node1, int Node2, Circuit& circuit, double Phase = 0, double DC = 0.5, double OFFSET = 0);
    int ID;
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    std::string classQuery() override {return "SQV";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;
};
