#pragma once
#include "ElectricElement.h"

/*
AC Current source has its inherited members, the usual AC parameters and the nodes it is connected to.
*/

class CurrentControlledSource : public ElectricElement {


private:
    double StampRightHand;
    ElectricElement* ControlVar;
    std::string elementName;


public:
    CurrentControlledSource(double Value, int Node1, int Node2,std::string ControllerName, std::string type, Circuit& circuit);
    void stamp(Circuit& circuit) override;
    void stampRightHand(Circuit& circuit) override;
    void linkCircuit(Circuit& circuit) override;
    std::string classQuery() override {return "CCS";}
    std::string nodesQuery() override;
    double getCurrent(Circuit& circuit) override;
    std::string type;

};
