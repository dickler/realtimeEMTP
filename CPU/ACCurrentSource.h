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
	int nodes[2];


public:
	ACCurrentSource(double Value, double Frequency, double Phase, int Node1, int Node2, Circuit& circuit);


	//Stamp 1 in the AdmittanceMatrix
	//Stamp Value to the RightHandVector

	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;



};
