#pragma once
#include "switchDevice.h"



class Diode :
	public switchDevice
{
public:
	Diode(double Value, int Node1, int Node2,Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
	void idealStamp(Circuit& circuit) override;

private:
	int nodes[2];
	int ID;
};

