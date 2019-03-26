#pragma once
#include "DynamicElement.h"





class Capacitor : public DynamicElement {


private:
	double StampRightHand;
	int StampValueAdm;


public:
    Capacitor(double Value, int Node1, int Node2, double Value0, Circuit& circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    void trapStamp(Circuit& circuit) override;
	void initialize(Circuit& circuit) override;
    std::string classQuery() override {return "C";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;
};
