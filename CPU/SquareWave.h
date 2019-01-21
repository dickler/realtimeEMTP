#pragma once
#include "ElectricElement.h"
#include "Circuit.h"

/*
AC Voltage source has its inherited members, an unique ID, phase, period, offset and DutyCycle(DC) parameters and the nodes it is connected to.
*/
class SquareWave :
	public ElectricElement{

private:
	int ID;
	double StampRightHand;
	double Phase;
	double Frequency;
	int nodes[2];
	double OFFSET;
	double DC;


public:

	SquareWave(double Value, double Frequency, int Node1, int Node2, Circuit& circuit, double Phase = 0, double DC = 0.5, double OFFSET = 0);

	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;



};