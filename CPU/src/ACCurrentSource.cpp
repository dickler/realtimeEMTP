#include "ACCurrentSource.h"
#include "Circuit.h"
#define _USE_MATH_DEFINES
#include <math.h>



//Assign the input values to the class members
ACCurrentSource::ACCurrentSource(double Value, double Frequency, double Phase, int Node1, int Node2, Circuit& circuit) {
	this->value = Value;
	this->Frequency = Frequency;
	this->Phase = Phase;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
}


void ACCurrentSource::stamp(Circuit& circuit) {}



/*
Stamps the RightHandVector
*/
void ACCurrentSource::stampRightHand(Circuit& circuit) {

	/*
	Calculate the value of the wave function for circuit.time
	*/
	StampRightHand = (this->value)*sin((2 * double(M_PI)*(this->Frequency)*(circuit.time)) + this->Phase);


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

std::string ACCurrentSource::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double ACCurrentSource::getCurrent(Circuit& circuit){
    return (this->value)*sin((2 * double(M_PI)*(this->Frequency)*(circuit.time)) + this->Phase);
}


