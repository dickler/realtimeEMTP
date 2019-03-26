#include "DCCurrentSource.h"
#include <fstream>


//
//The Current element is made of the nodes it connects to, the inheritance
//from the ElectricElement and its form of stamps.
//



//Assign the input values to the class members
DCCurrentSource::DCCurrentSource(double Value, int Node1, int Node2) {
	this->value = Value;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
}



void DCCurrentSource::stamp(Circuit& circuit) {};


//Stamps the DCCurrentSource to the circuit
void DCCurrentSource::stampRightHand(Circuit& circuit) {

	//The stamp value for the Admittance matrix is set to 1 and to the Source matrix set to V


	//If nodes[0] is not ground
	if (nodes[0] != 0) {

		//Stamps the RightHandVector in the index node[0]
		circuit.RightHandVector[nodes[0]-1] += this->value;


	};

	//if nodes[1] is not ground
	if (nodes[1] != 0) {

		//Stamps the RightHandVector in the index node[1]
		circuit.RightHandVector[nodes[1]-1] -= this->value;

	};
};

std::string DCCurrentSource::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double DCCurrentSource::getCurrent(Circuit &circuit){
    return this->value;
}
