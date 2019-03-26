#include "CurrentControlledSource.h"
#include "Circuit.h"
#define _USE_MATH_DEFINES
#include <math.h>



//Assign the input values to the class members
CurrentControlledSource::CurrentControlledSource(double Value, int Node1, int Node2, std::string ControllerName, std::string type, Circuit& circuit) {
    this->value = Value;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
    this->type = type;
    this->elementName = ControllerName;
}


void CurrentControlledSource::stamp(Circuit& circuit) {}

void CurrentControlledSource::linkCircuit(Circuit& circuit){
    for(int i:this->nodes){
        if(i==0) this->Voltage.push_back(&circuit.V0);
        else this->Voltage.push_back(&circuit.SolvedVector[i-1]);
    }
//    std::cout << this->elementName;
    this->ControlVar = circuit.requestElement(this->elementName);

}

/*
Stamps the RightHandVector
*/
void CurrentControlledSource::stampRightHand(Circuit& circuit) {

    /*
    Calculate the value of the wave function for circuit.time
    */

    if(this->type == "I") this->StampRightHand = ControlVar->getCurrent(circuit)*this->value;
    else if(this->type=="V") this->StampRightHand = ControlVar->getVoltage(circuit)*this->value;
    //Checks if nodes[0] is not ground
    if (nodes[0] != 0) {

        //Stamps the RightHandVector in the index node[0]
        circuit.RightHandVector[nodes[0] - 1] += StampRightHand;


    };

    //Checks if nodes[1] is not ground
    if (nodes[1] != 0) {

        //Stamps the RightHandVector in the index node[1]
        circuit.RightHandVector[nodes[1] - 1] -= StampRightHand;


    };
};

std::string CurrentControlledSource::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double CurrentControlledSource::getCurrent(Circuit &circuit){
    return this->StampRightHand;
};
