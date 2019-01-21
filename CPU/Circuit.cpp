#include "Circuit.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "mkl.h"
#include <bitset>
#include <complex>
#include "ElectricElement.h"
#include "switchDevice.h"
#define maxSwitches 16

//The Circuit class will store the nodal equations in matricial form,
//and handle it's solution

/*
The empty constructor assigns the default values to their correspondents variables
Then calls the initializator of the dynamic Matrices and the SourceId
*/

Circuit::Circuit() {
	this->time = 0;
	this->timeStep = 1e-6;
	this->numNodes = 1;
	this->numVSources = 1;
	this->totalSize = 2;
	this->init();
}

/*
The constructor assigns the values to their correspondents variables
Then calls the initializator of the dynamic Matrices and the SourceId
*/

Circuit::Circuit(int numNodes, int numVSources, double time, double step) {
	this->simTime = time;
	this->timeStep = step;
	this->numNodes = numNodes;
	this->numVSources = numVSources;
	this->totalSize = numNodes + numVSources;
	this->init();
};

/*
Change paramenters of an already created Circuit
*/

void Circuit::setValues(int numNodes, int numVSources,double time, double step) {
	this->simTime = time;
	this->timeStep = step;
	this->numNodes = numNodes;
	this->numVSources = numVSources;
	this->totalSize = numNodes + numVSources;
	this->init();
};




/*
The init function initialize the Vectors to its correct size and the SourceId to 0
*/

void Circuit::init() {
	this->AdmittanceMatrix.resize(totalSize*totalSize, 0);
	this->InvAdmittanceMatrix.resize(totalSize*totalSize, 0);
	this->RightHandVector.resize(totalSize, 0);
	this->SolvedVector.resize(totalSize, 0);
	this->SourceId = 0;
};

/*
Creates an Circuit adress
*/

Circuit& Circuit::Default() {
	static Circuit circuit;
	return circuit;
}


/*
Inverts the current AdmittanceMatrix and stores it in InvAdmittanceMatrix
*/

void Circuit::InvertMatrix() {

	//Creates and assign temporary dynamic 1D array
	//a = AdmittanceMatrix
	double* a;
	a = new double[this->totalSize*this->totalSize]();
	std::copy(this->AdmittanceMatrix.begin(), this->AdmittanceMatrix.end(), a);


	//Assing the length to a more readable variable m
	int m = this->totalSize;


	//Factorization of AdmittanceMatrix according to its documentation
	this -> ipiv = new int[this->totalSize];
	LAPACKE_dgetrf(LAPACK_ROW_MAJOR, m, m, a, m, ipiv);
	LAPACKE_dgetri(LAPACK_ROW_MAJOR, m, a, m, ipiv);

	//Pass a to InvAdmittanceMatrix
	//and deletes it
	std::vector<double> b(a, a + m*m);
	this->InvAdmittanceMatrix = b;

	delete[] a;
};

//Solves the circuit using LAPACK_getrs
void Circuit::Solve() {

	//Creates and assign temporary dynamic 1D arrays
	//a = AdmittanceMatrix
	//b = RightHandVector
	double* a;
	double* b;
	double* c;
	a = new double[this->totalSize*this->totalSize]();
	b = new double[this->totalSize]();
	c = new double[this->totalSize]();
	std::copy(this->InvAdmittanceMatrix.begin(), this->InvAdmittanceMatrix.end(), a);
	std::copy(this->RightHandVector.begin(), this->RightHandVector.end(), b);

	//Assing the length to a more readable variable m
	int m = this->totalSize;

	//Run the solver according to its documentation
	//returning b as the solved vector
	//LAPACKE_dgetrs(LAPACK_ROW_MAJOR, 'N', m, 1, a, m, ipiv, b, 1);
	cblas_dgemv(CblasRowMajor, CblasNoTrans, m, m, 1, a, m, b, 1, 0, c, 1);
	//Declares c as vector and assings it as the values of b
	std::vector<double> result(c, c + m);

	//Pass the result to its member SolvedVector
	this->SolvedVector = result;

	//Deletes the "new" variables
	delete[] a, b, ipiv;
};


//Shows the AdmittanceMatrix as string in the format
//
// [ A1,1   A1,2   . . .  A1,n-1   A1,n   ]
// [ A2,1   A2,2   . . .  A2,n-1   A2,n   ]
// [  .       .                    .      ]
// [  .               .            .      ]
// [  .                     .      .      ]
// [ An-1,1 An-1,2 . . .  An-1,n-1 An-1,n ]
// [ An,1   An,2   . . .  An,n-1   An,n   ]
void Circuit::ToString() {

	int i = 0;
	int ii = 0;

	//For each row
	for (i = 0; i< (this->totalSize); i++) {
		//writes "[ "
		std::cout << "[ ";

		//For each column writes
		for (ii = 0; ii< (this->totalSize); ii++) {
			// A[row][column] + "\t"
			std::cout << this->AdmittanceMatrix[(this->totalSize)*i + ii] << "\t";
		};

		// Closes each row with "] \n"
		std::cout << "] \n";


	};
	std::cout << "\n\n\n";

};

//Shows the InvAdmittanceMatrix as string in the format
//
// [ A1,1   A1,2   . . .  A1,n-1   A1,n   ]
// [ A2,1   A2,2   . . .  A2,n-1   A2,n   ]
// [  .       .                    .      ]
// [  .               .            .      ]
// [  .                     .      .      ]
// [ An-1,1 An-1,2 . . .  An-1,n-1 An-1,n ]
// [ An,1   An,2   . . .  An,n-1   An,n   ]
void Circuit::ToStringInv() {

	int i = 0;
	int ii = 0;

	//For each row
	for (i = 0; i < (this->totalSize); i++) {
		//writes "[ "
		std::cout << "[ ";

		//For each column writes
		for (ii = 0; ii < (this->totalSize); ii++) {
			// A[row][column] + "\t"
			std::cout << this->InvAdmittanceMatrix[(this->totalSize)*i + ii] << "\t";
		};

		// Closes each row with "] \n"
		std::cout << "] \n";


	};
	std::cout << "\n\n\n";

};


//Shows the RightHandVector as string in the format
//
// [  A1   ]
// [  A2   ]
// [   .   ]
// [   .   ]
// [   .   ]
// [  An-1 ]
// [  An   ]
void Circuit::ToStringRHS() {

	int i = 0;

	//For each row write "[ Ai ] \n"
	for (i = 0; i< (this->totalSize); i++) {

		std::cout << "[ ";
		std::cout << this->RightHandVector[i] << "\t";
		std::cout << "] \n";


	};
	std::cout << "\n\n\n";

};


//Shows the SolvedVector as string in the format
//
// [  A1   ]
// [  A2   ]
// [   .   ]
// [   .   ]
// [   .   ]
// [  An-1 ]
// [  An   ]
void Circuit::ToStringSolved() {

	int i = 0;

	//For each row write "[ Ai ] \n"
	for (i = 0; i< (this->totalSize); i++) {

		std::cout << "[ ";
		std::cout << this->SolvedVector[i] << "\t";
		std::cout << "] \n";


	};
	std::cout << "\n\n\n";

};



//Saves its RightHandVector as the one which will continue as Static
void Circuit::registerVector() {
	this->StaticRightHandVector = this->RightHandVector;
};


//Loads its StaticRightHandVector to a new iteration
void Circuit::newStep() {
	this->RightHandVector = this->StaticRightHandVector;
};

//Saves the AdmittanceMatrix
void Circuit::Save() {
	this->StaticAdmittanceMatrix = this->AdmittanceMatrix;
};

//Loads the AdmittanceMatrix
void Circuit::Load() {
	this->AdmittanceMatrix = this->StaticAdmittanceMatrix;
};

/*
Gets the eigenvalues for the AdmittanceMatrix as a complex vector
*/
void Circuit::takeEigen(std::vector<double> Vec, std::vector<std::complex<double> > & EigenVec) {

	/*
	Declares and initilize the needed variables for the eigen calculator method
	*/
	int totalSize = sqrt(Vec.size());
	lapack_int ilo = 0;
	lapack_int ihi = 0;
	double* scale = new double[totalSize];
	double* tau = new double[totalSize - 1]{ 0 };
	double* wr = new double[totalSize] { 0 };
	double* wi = new double[totalSize] { 0 };
	double* z = new double[totalSize] { 0 };
	double* a = new double[totalSize*totalSize];

	std::copy(Vec.begin(), Vec.end(), a);


	/*
	Runs the method according to its documentation
	*/
	LAPACKE_dgebal(LAPACK_ROW_MAJOR, 'P', totalSize, a, totalSize, &ilo, &ihi, scale);
	LAPACKE_dgehrd(LAPACK_ROW_MAJOR, totalSize, ilo, ihi, a, totalSize, tau);
	LAPACKE_dhseqr(LAPACK_ROW_MAJOR, 'E', 'N', totalSize, ilo, ihi, a, totalSize, wr, wi, z, totalSize);

	/*
	Loads the returned values in the result vector
	*/
	for (int ii = 0; ii < totalSize; ii++) {
		EigenVec.push_back(std::complex<double>(wr[ii], wi[ii]));
	}	


	/*
	Deletes the temporary variables
	*/
	delete scale, tau, wr, wi, z, a;
}

/*
Optimizes the values of G according to Razzaghi's method of squared euclidean distance of the eigenvalues
*/
void Circuit::optimizeG(std::vector< switchDevice* > SwitchVector, double range, int points) {
	
	/*
	Declares the variables that will be used
	*/
	double step = range / points;
	int switches = SwitchVector.size();
	std::vector< double > BestG;
	std::vector<std::vector<std::complex<double> > > EigenVec;
	std::vector<std::complex<double> > Eigen;
	int totalSize = sqrt((this->AdmittanceMatrix).size());
	double sum;
	double bestSum=0;
	std::vector<double> bestVec;
	double g;
	BestG.resize(switches);


	/*
	Gets the binary representative of the values from 0 to 2^switches, sets the
	switch on or off depending on its correspondent bit value and idealStamp it
	*/
	for (int i = 0; i < pow(2, switches); i++) {
		for (int j = 0; j < switches; j++) {

			if (std::bitset<maxSwitches>(i)[j] == 1) {

				(*SwitchVector[j]).S = 1;
				(*SwitchVector[j]).idealStamp(*this);

			}
			else {

				(*SwitchVector[j]).S = 0;
				(*SwitchVector[j]).idealStamp(*this);

			}
		}

		/*
		Calculate and stores the eigenvalues for each switch configuration
		*/

		this->takeEigen(this->AdmittanceMatrix, Eigen);
		EigenVec.push_back(Eigen);


		//for (int m = 0; m < totalSize; m++)
		//{
		//	std::cout <<"["<< Eigen[m] <<"]\n";
		//}

		/*
		Clear the eigen Vector and prepares the AdmittanceMatrix for another iteration
		*/
		Eigen.clear();
		this->Load();
	}



	/*  
	std::ofstream myfile;
	myfile.open("example.txt");
	myfile << "Sum\tE1\tE2\tE3\tE4\tE5\tE6\tE7\tE8\tE9\tE10\tE11\tG1\tG2\n";
	*/



	for (int i = 0; i < pow(points, switches); i++) {
		

		int k = i;
		int temp=0;
		this->Load();
		double h = range / points;

		/*
		For points^switches times, calculate the pejovic conductance (g) for each switch and stamps it.
		
		
		This calculations follows these steps:
		(k is initialized i)

		-Get the remainder of k/points
		-g is calculated as the remainder+1 times increment of the conductance (this avoid g = 0)
		-k is reinitialized as k-remainder divided by points.

		For example, 3 switches, increment of 0.01 and 200 points. For the point number 3 225 548:

		Switch 1 gets remainder of 148, this sets its g to 1.48 and the next k = 16 127.
		
		Switch 2 gets remainder of 127, this sets its g to 1.27 and the next k = 80.

		Switch 3 gets remainder of 80, this sets its g to 0.80 finishes this loop.
		*/
		for (int j = 0; j < switches; j++) {

			temp = k % points;
			g = (temp + 1)*h;


			k = (k - temp) / points;
			(*SwitchVector[j]).value = g;
			(*SwitchVector[j]).stamp(*this);


		}

		/*
		Takes the eigen for the iteration
		*/
		this->takeEigen(this->AdmittanceMatrix, Eigen);
		
		

		/*
		Gets the sum of all euclidean squared distance between the Eigen Values of this iteration and
		each ideal switch configuration.
		*/
		sum = 0;
		for (int l = 0; l < pow(2, switches); l++) {

		for (int m = 0; m < totalSize; m++) {
			sum += pow(real(Eigen[m] - EigenVec[l][m]), 2)+ pow(imag(Eigen[m] - EigenVec[l][m]), 2);
		}
		}


		//myfile << sum << '\t';

		//for (int m = 0; m < totalSize; m++) {
		//	myfile << abs(Eigen[m]) << '\t';

		//}

		//for (int j = 0; j < switches; j++) {

		//	myfile << (*SwitchVector[j]).value << "\t";


		//}
		//myfile << "\n";

		/*
		Check if the stored sum is less than the stored one.
		If it is, the new is stored along side the AdmittanceMatrix and the corresponding switches g's
		*/

		if (bestSum == 0) {
			bestSum = sum;
			bestVec = this->AdmittanceMatrix;
			for (int j = 0; j < switches; j++) {
				BestG[j] = (*SwitchVector[j]).value;
			}
				
		}
		else {
			if (bestSum > sum) {

				bestSum = sum;
				bestVec = this->AdmittanceMatrix;
				
				for (int j = 0; j < switches; j++) {
					BestG[j] = (*SwitchVector[j]).value;
				}
			}

		}

		/*
		clear the Eigen vector for another iteration
		*/


		Eigen.clear();

		/*std::cout << i;
		std::cout << '\n';*/
	}
	//myfile.close();



	/*
	Loads the g's and the AdmittanceMatrix from the best result achieved.
	*/

	this->AdmittanceMatrix = bestVec;
	for (int j = 0; j < switches; j++) {
		(*SwitchVector[j]).value = BestG[j];
		std::cout << BestG[j] << "\n";
	}
};