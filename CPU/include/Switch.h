#pragma once
#include "switchDevice.h"
class Switch :
	public switchDevice
{
public:
    Switch(double Value, int Node1, int Node2, Circuit &circuit);
	void stamp(Circuit& circuit) override;
	void stampRightHand(Circuit& circuit) override;
    void idealStamp(Circuit& circuit) override;
    void calculateState() override;
    std::string classQuery() override {return "S";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;

    bool *gate;
    void stampConductance(Circuit& circuit) override;
    void stampPejovic(Circuit& circuit) override;
    void stampMANA(Circuit& circuit) override;


private:

};

