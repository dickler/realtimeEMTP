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
    void calculateState() override;
    std::string classQuery() override {return "D";}
    std::string nodesQuery() override;
    int ID;
    double getCurrent(Circuit& circuit) override;
};

