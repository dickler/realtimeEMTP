#pragma once
#include "DynamicElement.h"





class Capacitor : public DynamicElement {


private:

	double StampRightHand;
	int StampValueAdm;
	int nodes[2];
	int ID = 0;



public:
	Capacitor(double Value, int Node1, int Node2, double Value0, Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
	void initialize(Circuit& circuit) override;
};
