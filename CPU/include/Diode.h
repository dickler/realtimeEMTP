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

    double getCurrent(Circuit& circuit) override;
    void stampConductance(Circuit& circuit) override;
    void stampPejovic(Circuit& circuit) override;
    void stampMANA(Circuit& circuit) override;

};

