#include "VoltageControlledSource.h"
#include "Circuit.h"
#define _USE_MATH_DEFINES
#include <math.h>



//Assign the input values to the class members
VoltageControlledSource::VoltageControlledSource(double Value, int Node1, int Node2, std::string ControllerName, std::string type, Circuit& circuit) {
    this->value = Value;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
    this->type = type;
    this->elementName = ControllerName;
    circuit.SourceId++;
    ID = circuit.SourceId;
}


void VoltageControlledSource::stamp(Circuit& circuit){

    /*
    Check if the node is not ground
    */
    if (nodes[0] != 0) {

        /*
        Stamps +1 for the positive node
        */

        circuit.AdmittanceMatrix[
            circuit.numNodes + ID - 1 + circuit.totalSize*(this->nodes[0] - 1)
        ] += 1;


        circuit.AdmittanceMatrix[
            circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[0] - 1)
        ] += 1;


    };

    /*
    Repeat the ground check
    */
    if (nodes[1] != 0) {

        /*
        Stamps -1 for the negative node
        */
        circuit.AdmittanceMatrix[
            circuit.numNodes + circuit.totalSize*(this->nodes[1] - 1) + ID - 1
        ] -= 1;


        circuit.AdmittanceMatrix[
            circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[1] - 1)
        ] -= 1;


    };
};

void VoltageControlledSource::linkCircuit(Circuit& circuit){
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
void VoltageControlledSource::stampRightHand(Circuit& circuit) {

    /*
    Calculate the value of the wave function for circuit.time
    */

    if(this->type == "I") this->StampRightHand = ControlVar->getCurrent(circuit)*this->value;
    else if(this->type=="V") this->StampRightHand = ControlVar->getVoltage(circuit)*this->value;

    /*
    Stamp said value in the corresponding row
    */
    circuit.RightHandVector[circuit.numNodes + ID - 1] += StampRightHand;
};

std::string VoltageControlledSource::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double VoltageControlledSource::getCurrent(Circuit &circuit){
     return circuit.SolvedVector[circuit.numNodes + ID - 1];
};
