#pragma once
#include "DynamicElement.h"
#include <fstream>
class Inductor : public DynamicElement {


private:
	int ID;
	double StampRightHand;
	int StampValueAdm;

public:
	Inductor(double Value, int Node1, int Node2, double Value0, Circuit& circuit);


	//Stamp 1 in the AdmittanceMatrix
	//Stamp Value to the RightHandVector

	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    void trapStamp(Circuit& circuit) override;
	void initialize(Circuit& circuit) override;
    std::string classQuery() override {return "I";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;
};
