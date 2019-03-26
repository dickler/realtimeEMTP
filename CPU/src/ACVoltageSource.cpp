#include "ACVoltageSource.h"
#include <fstream>
#include "Circuit.h"
#define _USE_MATH_DEFINES
#include <math.h>



//Assign the input values to the class members, generates an ID and creates column for the current in the output file
ACVoltageSource::ACVoltageSource(double Value, double Frequency, double Phase, int Node1, int Node2, Circuit& circuit) {
		this->value = Value;
		this->Frequency = Frequency;
		this->Phase = Phase;
        this->nodes.push_back(Node1);
        this->nodes.push_back(Node2);
		circuit.SourceId++;
		ID = circuit.SourceId;
	}


	//Stamps the AdmittanceMatrix
	void ACVoltageSource::stamp(Circuit& circuit){

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


	void ACVoltageSource::stampRightHand(Circuit& circuit) {
		/*
		Calculate the value of wave function for the circuit.time
		*/
		StampRightHand = (this->value)*sin((2 * double(M_PI)*(this->Frequency)*(circuit.time)) + double(M_PI)/180*this->Phase);

		/*
		Stamp said value in the corresponding row
		*/
		circuit.RightHandVector[circuit.numNodes + ID - 1] += StampRightHand;
	};

    std::string ACVoltageSource::nodesQuery() {
        std::string nodesStr;
        for(int i: nodes){
            nodesStr+= std::to_string(i) + "\n";
        }
        return nodesStr;
    }

    double ACVoltageSource::getCurrent(Circuit& circuit){
        return circuit.SolvedVector[circuit.numNodes + ID - 1];
    }
