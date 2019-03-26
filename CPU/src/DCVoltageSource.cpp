#include "DCVoltageSource.h"
#include <fstream>


//
//The Voltage element is made of the nodes it connects to, the inheritance
//from the ElectricElement and its form of stamp.
//



//Assign the input values to the class members, generates an ID and creates column for the current in the output file
DCVoltageSource::DCVoltageSource(double Value, int Node1, int Node2, Circuit& circuit) {
	this->value = Value;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
	circuit.SourceId++;
	ID = circuit.SourceId;
	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << 'I' << 'v' << ID << '\t';
}


//Stamps the DCVoltageSource to the circuit
void DCVoltageSource::stamp(Circuit& circuit){

	//If nodes[0] is not ground
	if (nodes[0] != 0) {

		//Stamps +1 in the AdmittanceMatrix for the corrects index
		circuit.AdmittanceMatrix[
			circuit.numNodes + circuit.totalSize*(this->nodes[0] - 1) + ID - 1
		] += 1;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[0] - 1)
		] += 1;


	};

	//if nodes[1] is not ground
	if (nodes[1] != 0) {

		//Stamps -1 in the AdmittanceMatrix for the corrects index
		circuit.AdmittanceMatrix[
			circuit.numNodes + circuit.totalSize*(this->nodes[1] - 1) + ID - 1
		] -= 1;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID-1) + (this->nodes[1] - 1)
		] -= 1;


	};
};


void DCVoltageSource::stampRightHand(Circuit& circuit) {

	//Stamps the source matrix in the IDth term past the nodes rows
	circuit.RightHandVector[circuit.numNodes + ID - 1] += this->value;

}

std::string DCVoltageSource::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double DCVoltageSource::getCurrent(Circuit &circuit){
    return circuit.SolvedVector[circuit.numNodes + ID - 1];
}
