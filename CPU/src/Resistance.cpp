#include "Resistance.h"



//
//The Resistance element is made of the nodes it connects to, the inheritance
//from the ElectricElement and its form of stamp.
//


//Assign its input values
Resistance::Resistance(double Value, int Node1, int Node2) {
		this->value = Value;
        this->nodes.push_back(Node1);
        this->nodes.push_back(Node2);
}


//Overrides the stamp fucntion to the Resistor stamp
void Resistance::stamp(Circuit& circuit){

	//Creates temporary variables for improved readability
	double StampValue;
	double Stmp;
	int n = 0, nn = 0;
	StampValue = 1 / (this->value);

	//for each node adds to the diagonal a 1/R value
	//and substract 1/R from the (i,j) and (j,i) of the AdmittanceMatrix
	//The ground node does not appear in the matrix as it is the reference
	
	for (int i = 0; i<2; i++) {


		//Assign the value of the node with a decrement so node 1 is considered the 0th index
		n = nodes[i] - 1;

		//if node[i] is not ground
		if (n != -1) {
			for (int ii = 0; ii<2; ii++) {

				//Assign the value of the node with a decrement so node 1 is considered the 0th index 
				nn = nodes[ii] - 1;

				//if node[ii] is not ground
				if (nn != -1) {

					//Establish the Stmp as the 1/R or -1/R depending if it is in the main diagonal or not
					Stmp = (2*(n == nn)-1)*StampValue;


					//Stamps the term A[i][ii] in the AdmittanceMatrix
					circuit.AdmittanceMatrix[n*circuit.totalSize + nn] += Stmp;

				};
			};
		};
	};
};

//Resistance has no RHS stamp.
void Resistance::stampRightHand(Circuit& circuit) {}


std::string Resistance::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

double Resistance::getCurrent(Circuit &circuit){
    return (*(this->Voltage[0]) - *(this->Voltage[1]))/this->value;
}
