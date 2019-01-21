#include "Capacitor.h"
#include <fstream>

#ifdef MNA_CAPACITOR


Capacitor::Capacitor(double Value, int Node1, int Node2, double Value0, Circuit& circuit) {
	this->value = Value;
	this->Value0 = Value0;
	this->nodes[0] = Node1;
	this->nodes[1] = Node2;
	circuit.SourceId++;
	ID = circuit.SourceId;
	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << 'I' << 'c' << ID << '\t';


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
}



void Capacitor::stamp(Circuit& circuit) {
	StampValueAdm = 1;
	StampRightHand = this->value / circuit.timeStep;
	circuit.AdmittanceMatrix[circuit.totalSize*(circuit.numNodes + ID - 1) + circuit.numNodes + ID - 1] -= StampValueAdm;

	if (nodes[0] != 0) {

		circuit.AdmittanceMatrix[
			circuit.numNodes + circuit.totalSize*(this->nodes[0] - 1) + ID - 1
		] += StampValueAdm;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[0] - 1)
		] += StampRightHand;


	};


	if (nodes[1] != 0) {

		circuit.AdmittanceMatrix[
			circuit.numNodes + circuit.totalSize*(this->nodes[1] - 1) + ID - 1
		] -= StampValueAdm;


		circuit.AdmittanceMatrix[
			circuit.totalSize*(circuit.numNodes + ID - 1) + (this->nodes[1] - 1)
		] -= StampRightHand;


	}; 
};


void Capacitor::stampRightHand(Circuit& circuit) {

	StampRightHand = this->value / circuit.timeStep;

	circuit.RightHandVector[circuit.numNodes + ID - 1] += +StampRightHand * (*V1-*V2);


};

void Capacitor::initialize(Circuit& circuit) {
	double G = (2 * this->value) / circuit.timeStep;
	double n1 = nodes[0] - 1;
	double n2 = nodes[1] - 1;


	Ih = -G * Value0;

	if (n1 != -1) {
		circuit.RightHandVector[circuit.numNodes + ID - 1] += Ih;
	}
	if (n2 != -1) {
		circuit.RightHandVector[circuit.numNodes + ID - 1] -= Ih;
	}

}

#else


	/*
	Constructor of the Capacitor object
	*/
Capacitor::Capacitor(double Value, int Node1, int Node2, double Value0, Circuit& circuit) {
	/*
	Sets the internal values of the variables as passed by the function
	*/
	this->value = Value;
	this->Value0 = Value0;
	this->nodes[0] = Node1;
	this->nodes[1] = Node2;

	/*
	Sets the pointers V1 and V2 to the corresponding node in the SolvedVector, for the ground node exception
	sets the pointer to a member 0 in the Circuit.
	*/
	if (nodes[0] == 0) {
		V1 = &(circuit.V0);
	}
	else {
		V1 = &(circuit.SolvedVector[nodes[0]-1]);
	}
	if (nodes[1] == 0) {
		V2 = &(circuit.V0);
	}
	else {
		V2 = &(circuit.SolvedVector[nodes[1] - 1]);
	}
}



void Capacitor::stamp(Circuit& circuit) {


	//Creates temporary variables for improved readability
	double StampValue;
	double Stmp;
	int n = 0, nn = 0;
	StampValue = (2*this->value)/ circuit.timeStep;


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


					//Establish the Stmp as the G or -G depending if it is in the main diagonal or not
					Stmp = (2 * (n == nn) - 1)*StampValue;


					//Stamps the term A[i][ii] in the AdmittanceMatrix
					circuit.AdmittanceMatrix[n*circuit.totalSize + nn] += Stmp;


				};
			};
		};
	};
};



/*
Stamps the RHS vector
*/
void Capacitor::stampRightHand(Circuit& circuit) {

	//Creates temporary variables for improved readability
	double G = (2 * this->value) / circuit.timeStep;
	double n1 = nodes[0] - 1;
	double n2 = nodes[1] - 1;

	/*
	Creates the history current using trapezoidal Method:

	Ik,m(t-dt) = - ik,m(t-dt) - G*Vk,m(t-dt)

	Calculating ik,m as a parallel circuit of an Coductance and a Current Source:
	
	ik,m(t) = Ik,m(t-dt) + G*Vk,m(t)

	Then we have:
	Ik,m(t-dt) = - Ik,m(t-2*dt) - 2 * G*Vk,m(t-dt).
	*/
	Ih = - Ih - 2 * G*(*V1 - *V2);

	/*
	Stamps the Ih in the RHS vector
	*/
	if (n1 != -1) {
		circuit.RightHandVector[n1] -= Ih;
	}
	if (n2 != -1) {
		circuit.RightHandVector[n2] += Ih;
	}


};

/*
Initializate the Voltage in the capacitor
*/

void Capacitor::initialize(Circuit& circuit) {

	//Creates temporary variables for improved readability
	double G = (2 * this->value) / circuit.timeStep;
	double n1 = nodes[0] - 1;
	double n2 = nodes[1] - 1;

	/*
	For the first stamp in the circuit for an non-zero Voltage and zero-current


	Ik,m(t-dt) = - ik,m(t-dt) - G*Vk,m(t-dt)
	    |              |            |
	    |              |            |
	   \|/            \|/          \|/
	Ik,m(t-dt) = -     0      - G*Value0

	The stamp method uses:

	Ik,m(t-dt) = - Ik,m(t-2*dt) - 2 * G*Vk,m(t-dt)

	So setting Ik,m(-1) = G*Value0 gets the Ik,m(0) to the desired value(- G*Value0).
	*/
	Ih = G*Value0;
}
#endif