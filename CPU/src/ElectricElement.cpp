#include "ElectricElement.h"



void ElectricElement::ObjectDissolver() {
	delete this;
};

void ElectricElement::linkCircuit(Circuit& circuit){
    for(int i:nodes){
        if(i==0) this->Voltage.push_back(&circuit.V0);
        else this->Voltage.push_back(&circuit.SolvedVector[i-1]);
    }
}

double ElectricElement::getVoltage(Circuit& circuit){
    return *Voltage[0]-*Voltage[1];
};
