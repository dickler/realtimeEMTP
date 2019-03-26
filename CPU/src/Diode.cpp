#include "Diode.h"
#include <iostream>
#include <fstream>


#ifdef EXPANDED_SWITCH
Diode::Diode(double Value, int Node1, int Node2,Circuit& circuit) {
	/*
	Sets the internal values of the variables as passed by the function
	*/
	this->value = Value;
	this->nodes[0] = Node1;
	this->nodes[1] = Node2;
	this->Ih = 0;

	/*
	Sets the pointers V1 and V2 to the corresponding node in the SolvedVector, for the ground node
	exception sets the pointer to a member 0 in the Circuit.
	*/
	if (nodes[0] == 0) {
		V1 = &(circuit.V0);
	}
	else {
		V1 = &(circuit.SolvedVector[nodes[0] - 1]);
	}
	if (nodes[1] == 0) {
		V2 = &(circuit.V0);
	}
	else {
		V2 = &(circuit.SolvedVector[nodes[1] - 1]);
	}
	circuit.SourceId++;
	ID = circuit.SourceId;
	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << 'I' << 'd' << ID << '\t';
}


//Overrides the stamp fucntion to the Resistor stamp for MNA
void Diode::stamp(Circuit& circuit) {

	//Creates temporary variables for improved readability
	double StampValue;
	double Stmp;
	int n = 0, nn = 0;
	StampValue = (this->value);

	//for each node adds to the diagonal a G value
	//and substract G from the (i,j) and (j,i) of the AdmittanceMatrix
	//The ground node does not appear in the matrix as it is the reference

	for (int i = 0; i < 2; i++) {


		//Assign the value of the node with a decrement so node 1 is considered the 0th index
		n = nodes[i] - 1;

		//if node[i] is not ground
		if (n != -1) {
			for (int ii = 0; ii < 2; ii++) {

				//Assign the value of the node with a decrement so node 1 is considered the 0th index 
				nn = nodes[ii] - 1;

				//if node[ii] is not ground
				if (nn != -1) {

					//Establish the Stmp as the Value G
					Stmp = StampValue;

					//If not in the diagonal Stmp is assigned as -G
					if (n != nn) {
						Stmp = -StampValue;
					};

					//Stamps the term A[i][ii] in the AdmittanceMatrix
					circuit.AdmittanceMatrix[n*circuit.totalSize + nn] += Stmp;

				};
			};
		};
	};

	circuit.AdmittanceMatrix[circuit.totalSize*(circuit.numNodes + ID - 1) + circuit.numNodes + ID - 1] += 1;

	if (nodes[0] != 0) {

		circuit.AdmittanceMatrix[
			circuit.totalSize*(this->nodes[0] - 1) + (circuit.numNodes + ID - 1)
		] += 1;

	};


	if (nodes[1] != 0) {

		circuit.AdmittanceMatrix[
			circuit.totalSize*(this->nodes[1] - 1) + (circuit.numNodes + ID - 1)
		] -= 1;

	}
};


void Diode::stampRightHand(Circuit& circuit) {
	double G;

	G = this->value;


	S = S * (Ih - G * (*V1 - *V2) < 0) + (1 - S)*(*V1 > *V2);

	Ih = S * Ih - (2 * S - 1) * G*(*V1 - *V2);


	circuit.RightHandVector[circuit.numNodes + ID - 1] = -Ih;

};


void Diode::idealStamp(Circuit& circuit) {

	circuit.AdmittanceMatrix[circuit.totalSize*(circuit.numNodes + ID - 1) + circuit.numNodes + ID - 1] += S;

	if (nodes[0] != 0) {

		circuit.AdmittanceMatrix[
			circuit.totalSize*(this->nodes[0] - 1) + (circuit.numNodes + ID - 1)
		] += 1;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[0] - 1)
		] += 1 - S;


	};


	if (nodes[1] != 0) {

		circuit.AdmittanceMatrix[
			circuit.totalSize*(this->nodes[1] - 1) + (circuit.numNodes + ID - 1)
		] -= 1;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[1] - 1)
		] -= 1 - S;
	};


}

#else


/*
Constructor of the Diode object
*/
Diode::Diode(double Value, int Node1, int Node2, Circuit &circuit) {
	/*
	Sets the internal values of the variables as passed by the function
	*/
	this->value = Value;
    this->nodes.push_back(Node1);
    this->nodes.push_back(Node2);
	this->Ih = 0;
}




//Overrides the stamp fucntion to the Diode stamp for FAMNM
void Diode::stamp(Circuit& circuit) {

	//Creates temporary variables for improved readability
	double StampValue;
	double Stmp;
	int n = 0, nn = 0;
    StampValue = pow((this->value),(2*S-1));

	//for each node adds to the diagonal a G value
	//and substract G from the (i,j) and (j,i) of the AdmittanceMatrix
	//The ground node does not appear in the matrix as it is the reference

	for (int i = 0; i < 2; i++) {


		//Assign the value of the node with a decrement so node 1 is considered the 0th index
		n = nodes[i] - 1;

		//if node[i] is not ground
		if (n != -1) {
			for (int ii = 0; ii < 2; ii++) {

				//Assign the value of the node with a decrement so node 1 is considered the 0th index 
				nn = nodes[ii] - 1;

				//if node[ii] is not ground
				if (nn != -1) {

					//Establish the Stmp as the Value G
					Stmp = StampValue;

					//If not in the diagonal Stmp is assigned as -G
					if (n != nn) {
						Stmp = -StampValue;
					};

					//Stamps the term A[i][ii] in the AdmittanceMatrix
					circuit.AdmittanceMatrix[n*circuit.totalSize + nn] += Stmp;

				};
			};
		};
	};

};


/*
Stamps the Diode in the RightHandVector
*/
void Diode::stampRightHand(Circuit& circuit) {

	//Creates temporary variables for improved readability
	double G;
	G = this->value;
	double n1 = nodes[0] - 1;
	double n2 = nodes[1] - 1;

	/*
	Calculates the polarization of the diode
	*/

	/*
	Ih must be:
	Ik,m(t-dt) = -ik,m(t-dt), when S=1;
	Ik,m(t-dt) = G * Vk,m(t-dt), when S=0.

	It's valid to note that for programming purpose the Ih source is pointed towards the node 1,
	instead of node 2 as it is in the Capacitor and inductor.
	*/
    S = S * (Ih - G * (*Voltage[0] - *Voltage[1]) < 0) + (1 - S)*(*Voltage[0] > *Voltage[1]);

    Ih = S * Ih - (2 * S - 1) * G*(*Voltage[0] - *Voltage[1]);



	//Stamps Ih in the correct position in RightHandVector
	if (n1 != -1) {
        circuit.RightHandVector[n1] += Ih;
	}
	if (n2 != -1) {
        circuit.RightHandVector[n2] -= Ih;
	}


};
void Diode::idealStamp(Circuit& circuit) {}
std::string Diode::nodesQuery() {
    std::string nodesStr;
    for(int i: nodes){
        nodesStr+= std::to_string(i) + "\n";
    }
    return nodesStr;
}

void Diode::calculateState(){
    this->S = (*(Voltage[0]) - *(Voltage[1]))>0;
}

double Diode::getCurrent(Circuit &circuit){
    return (*(Voltage[0]) - *(Voltage[1]))/pow((this->value),(1-2*S));
}

#endif
