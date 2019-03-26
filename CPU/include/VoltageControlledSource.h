#pragma once
#include "ElectricElement.h"


/*
AC Voltage source has its inherited members, an unique ID, the usual AC parameters and the nodes it is connected to.
*/
class VoltageControlledSource : public ElectricElement {


private:

    double StampRightHand;
    ElectricElement* ControlVar;
    std::string elementName;


public:
    VoltageControlledSource(double Value, int Node1, int Node2,std::string ControllerName, std::string type, Circuit& circuit);
    void stamp(Circuit& circuit) override;
    void stampRightHand(Circuit& circuit) override;
    void linkCircuit(Circuit& circuit) override;
    std::string classQuery() override {return "VCS";}
    std::string nodesQuery() override;
    int ID;
    double getCurrent(Circuit& circuit) override;
    std::string type;



};
