#pragma once
#include "switchDevice.h"
class Switch :
	public switchDevice
{
public:
	Switch(double Value, int Node1, int Node2, int Gate,Circuit &circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
	void idealStamp(Circuit& circuit) override;

private:
	int nodes[3];
	int ID;
};

