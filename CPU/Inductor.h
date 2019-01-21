#pragma once
#include "DynamicElement.h"
#include <fstream>
class Inductor : public DynamicElement {


private:
	int ID;
	double StampRightHand;
	int StampValueAdm;
	int nodes[2];


public:
	Inductor(double Value, int Node1, int Node2, double Value0, Circuit& circuit);


	//Stamp 1 in the AdmittanceMatrix
	//Stamp Value to the RightHandVector

	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
	void initialize(Circuit& circuit) override;
};
