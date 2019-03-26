#include "SquareWave.h"
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

//Assign the input values to the class members, generates an ID and creates column for the current in the output file
SquareWave::SquareWave(double Value, double Frequency, int Node1, int Node2, Circuit& circuit, double Phase, double DC, double OFFSET) {
	this->value = Value;
	this->Frequency = Frequency;
	this->Phase = Phase;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
	this->OFFSET = OFFSET;
	this->DC = DC;
	circuit.SourceId++;
	ID = circuit.SourceId;
	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << 'I' << 'v' << ID << '\t';
}


//Stamp 1 in the AdmittanceMatrix
void SquareWave::stamp(Circuit& circuit) {

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
	Check if the node is not ground
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

//Stamp the RightHandVector
void SquareWave::stampRightHand(Circuit& circuit) {

	/*
	Calculate the angle of the wave it is in and point of transition
	*/

	double NormalizedAngle = std::fmod(circuit.time + this->Phase/360/(this->Frequency), 1/(this->Frequency));
	double TransitionSpot = this->DC/this->Frequency;

	/*
	Sets the value for the current state of the source
	*/
	StampRightHand = (this->value)*((NormalizedAngle<TransitionSpot) - (NormalizedAngle>TransitionSpot));


	/*
	Stamps said value in the RightHandVector
	*/
	circuit.RightHandVector[circuit.numNodes + ID - 1] += StampRightHand;
};

std::string SquareWave::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double SquareWave::getCurrent(Circuit &circuit){
    return  circuit.SolvedVector[circuit.numNodes + ID - 1];
}
