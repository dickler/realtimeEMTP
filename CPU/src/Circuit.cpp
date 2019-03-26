#include "Circuit.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "mkl.h"
#include <bitset>
#include <iterator>
#include <sstream>
#include <complex>
#include "Elements.h"
#define maxSwitches 256
//#define TIMED
#ifdef TIMED
#include <QElapsedTimer>
#include <QDebug>
#endif
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
#ifdef TIMED
    QElapsedTimer timer;
    timer.start();
#endif
	//Creates and assign temporary dynamic 1D arrays
	//a = AdmittanceMatrix
	//b = RightHandVector
	double* a;
	double* b;
	double* c;

    int currentState = this->readState();
    std::vector<double>  backupSolved;


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
    backupSolved = this->SolvedVector;
	//Pass the result to its member SolvedVector
    this->SolvedVector = result;
    if(this->switchFlag) {
        this->switchFlag=0;

    }
    else{
        this->newStep();
        this->trapStepStamp();
        for(switchDevice* i:this->SwitchVector){
        if(std::string("D TH").find(i->classQuery()) != std::string::npos){
            i->calculateState();
        }
    }
    if(currentState != this->readState()){
        this->SolvedVector = backupSolved;
        this->importState();
        this->Solve();
        this->switchFlag=1;
    }
    else{
        this->setState(currentState);
    }
    }
	//Deletes the "new" variables
	delete[] a, b, ipiv;
#ifdef TIMED
    float nsec = timer.nsecsElapsed();

    std::cout << "The operation took " << nsec << " nanoseconds\n\n";
#endif
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

/*
Import the netlist and populate the element vectors
*/
void Circuit::importNetlist(std::ifstream &netlist){
        std::string line;

        /*
        Gets the time parameters and create the circuit
        */
        netlist.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
        netlist >> this->simTime >> this->timeStep;
        netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        /*
         Reads through the netlist and store Elements in its vectors
        */

        while (std::getline(netlist,line)) {
            if (line.length()>5)   this->createElement(line);
        }

}

/*
From a line creates a element
*/
void Circuit::createElement(std::string Line){
    std::stringstream stringHandler(Line);
    std::istream_iterator<std::string> begin(stringHandler);
    std::istream_iterator<std::string> end;
    std::vector<std::string> Arguments(begin, end);

    if (Arguments[0] == "R") {
        // name >> node1 >> node2 >> value

        this->SingleStampVector.insert(this->SingleStampVector.begin(),
                                    new Resistance(std::stod(Arguments[4]), std::stoi(Arguments[2]), std::stoi(Arguments[3])));
        this->SingleStampVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "DCV") {
        // name >> node1 >> node2 >> value
         this->SingleStampVector.insert(this->SingleStampVector.begin(),
                                     new DCVoltageSource(std::stod(Arguments[4]), std::stoi(Arguments[2]),
                                                         std::stoi(Arguments[3]),*this));
        this->SingleStampVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "L") {
        // name >> node1 >> node2 >> value >> value0
        this->DynamicVector.insert(this->DynamicVector.begin(),
                                   new Inductor(std::stod(Arguments[4]), std::stoi(Arguments[2]),
                                                std::stoi(Arguments[3]), std::stod(Arguments[5]), *this));
        this->DynamicVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "C") {
        // name >> node1 >> node2 >> value >> value0
        this->DynamicVector.insert(this->DynamicVector.begin(),
                             new Capacitor(std::stod(Arguments[4]), std::stoi(Arguments[2]),
                                           std::stoi(Arguments[3]), std::stod(Arguments[5]), *this));
        this->DynamicVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "ACV") {
        // name >> node1 >> node2 >> value >> frequency >> phase
        this->ElectricVector.insert(this->ElectricVector.begin(),
                              new ACVoltageSource(std::stod(Arguments[4]), std::stod(Arguments[5]), std::stod(Arguments[6]),
                                                  std::stoi(Arguments[2]), std::stoi(Arguments[3]), *this));
        this->ElectricVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "SQV") {
        // name >> node1 >> node2 >> value >> frequency >> phase >> DC >> OFFSET
        this->ElectricVector.insert(this->ElectricVector.begin(),
                              new SquareWave(std::stod(Arguments[4]), std::stod(Arguments[5]), std::stoi(Arguments[2]),
                                             std::stoi(Arguments[3]), *this, std::stod(Arguments[6]),
                                             std::stod(Arguments[7]), std::stod(Arguments[8])));
    }

    else if (Arguments[0] == "ACC") {
        // name >> node1 >> node2 >> value >> frequency >> phase
        this->ElectricVector.insert(this->ElectricVector.begin(),
                              new ACCurrentSource(std::stod(Arguments[4]), std::stod(Arguments[5]), std::stod(Arguments[6]),
                                                  std::stoi(Arguments[2]), std::stoi(Arguments[3]), *this));
        this->ElectricVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "DCC") {
        // name >> node1 >> node2 >> value;
        this->SingleStampVector.insert(this->SingleStampVector.begin(),
                                    new DCCurrentSource(std::stod(Arguments[4]), std::stoi(Arguments[2]),
                                                        std::stoi(Arguments[3])));
       this->SingleStampVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "D") {
        // name >> node1 >> node2 >> value;
        this->SwitchVector.insert(this->SwitchVector.begin(),
                                  new Diode(std::stod(Arguments[4]), std::stoi(Arguments[2]), std::stoi(Arguments[3]), *this));
        this->SwitchVector[0]->Name = Arguments[1];
    }

    else if (Arguments[0] == "S") {
        // name >> node1 >> node2 >> Gate >> value;
        this->SwitchVector.insert(this->SwitchVector.begin(),
                                  new Switch(std::stod(Arguments[5]), std::stoi(Arguments[2]), std::stoi(Arguments[3]),
                                             std::stoi(Arguments[4]), *this));
        this->SwitchVector[0]->Name = Arguments[1];;
    }
    else if (Arguments[0] == "TH") {
        // name >> node1 >> node2 >> value;
        this->SwitchVector.insert(this->SwitchVector.begin(),
                                  new Thyristor(std::stod(Arguments[4]), std::stoi(Arguments[2]), std::stoi(Arguments[3]), *this));
        this->SwitchVector[0]->Name = Arguments[1];
    }
    else if (Arguments[0] == "CCS"){
        //name >> node1 >> node2 >> gain >> element >> type;
        this->ElectricVector.insert(this->ElectricVector.begin(),
                                   new CurrentControlledSource(std::stod(Arguments[4]), std::stoi(Arguments[2]), std::stoi(Arguments[3]),Arguments[5], Arguments[6], *this));
        this->ElectricVector[0]->Name = Arguments[1];
    }
    else if (Arguments[0] == "VCS"){
        //name >> node1 >> node2 >> gain >> element >> type;
        this->ElectricVector.insert(this->ElectricVector.begin(),
                                   new VoltageControlledSource(std::stod(Arguments[4]), std::stoi(Arguments[2]), std::stoi(Arguments[3]),Arguments[5], Arguments[6], *this));
        this->ElectricVector[0]->Name = Arguments[1];
    }
}

/*
From the Vectors of elements, calculate value of internal variables for the dimension of the matrices
*/
void Circuit::calculateDimension(){
    int n=0;
    int Vsources=0;
    int node;
    std::stringstream Handler;

    for(ElectricElement* i : this->SingleStampVector){
        if(i->classQuery().find("V")!=std::string::npos){
            Vsources++;
        }
        Handler.str(i->nodesQuery());
        while(Handler>>node){
            if(n<node) n = node;
        }
        Handler.clear();
    }
    for(ElectricElement* i : this->ElectricVector){
        if(i->classQuery().find("V")!=std::string::npos){
            Vsources++;
        }
        Handler.str(i->nodesQuery());
        while(Handler>>node){
            if(n<node) n = node;
        }
        Handler.clear();
    }
    for(ElectricElement* i : this->DynamicVector){
        Handler.str(i->nodesQuery());
        while(Handler>>node){
            if(n<node) n = node;
        }
        Handler.clear();
    }
    for(ElectricElement* i : this->SwitchVector){
        Handler.str(i->nodesQuery());
        while(Handler>>node){
            if(n<node){
                n = node;
            }
        }
        Handler.clear();
    }
    this->numNodes = n;
    this->numVSources = Vsources;
    this->totalSize = n + Vsources;
    this->init();
}

void Circuit::staticAdmittanceStamp(){
    for(ElectricElement* i: this->SingleStampVector)    i->stamp(*this);
    for(ElectricElement* i: this->ElectricVector)       i->stamp(*this);
    for(ElectricElement* i: this->DynamicVector)        i->stamp(*this);
}

void Circuit::staticRHSStamp(){
    for(ElectricElement* i: this->SingleStampVector)    i->stampRightHand(*this);
}

void Circuit::RHSStamp(){
    for(ElectricElement* i: this->ElectricVector){
        i->stampRightHand(*this);
    }
    for(DynamicElement* i: this->DynamicVector){
        i->stampRightHand(*this);
    }
}

void Circuit::trapStepStamp(){
    for(ElectricElement* i: this->ElectricVector){
        i->stampRightHand(*this);
    }
    for(DynamicElement* i: this->DynamicVector){
        i->trapStamp(*this);
    }
}

void Circuit::setState(int n){
    int ns = this->SwitchVector.size();
    int bit = 1;
    for(int j=0; j<ns;j++){
        this->SwitchVector[j]->S = (bit&n)==bit;
        bit<<=1;
    }
}

int Circuit::readState(){
    int ns = this->SwitchVector.size();
    int state=0;
    for(int j=0; j<ns;j++){
        state += pow(2,j)*this->SwitchVector[j]->S;
    }
    return state;
}

void Circuit::createStateMatrices(){
    int ns = this->SwitchVector.size();
    for(int i = 0; i<pow(2,ns);i++){
        this->Load();
        this->setState(i);
        for(switchDevice* j : this->SwitchVector){
            j->stamp(*this);
            std::cout << j->Name << " " <<  j->S<<"\n";
        };

        this->ToString();

        this->InvertMatrix();
        this->ToStringInv();
        this->MatrixStorage.push_back(new std::vector<double>(this->AdmittanceMatrix));
        this->invMatrixStorage.push_back(new std::vector<double>(this->InvAdmittanceMatrix));
    }
}

void Circuit::linkElements(){
    for(ElectricElement* i: SingleStampVector) i->linkCircuit(*this);
    for(ElectricElement* i: ElectricVector) i->linkCircuit(*this);
    for(ElectricElement* i: DynamicVector) i->linkCircuit(*this);
    for(ElectricElement* i: SwitchVector) i->linkCircuit(*this);
}

void Circuit::storeNames(){
    for(ElectricElement* i:this->SingleStampVector){
        if(std::find(this->NameVector.begin(), this->NameVector.end(), i->Name) != this->NameVector.end()) {
            std::cout << "\n\"" << i->Name << "\" has been used more than once in your circuit, this can cause problems with controlled sources.\n\n";
        }
        this->NameVector.push_back(i->Name);
    }
    for(ElectricElement* i:this->ElectricVector){
        if(std::find(this->NameVector.begin(), this->NameVector.end(), i->Name) != this->NameVector.end()) {
            std::cout << "\n\"" << i->Name << "\" has been used more than once in your circuit, this can cause problems with controlled sources.\n\n";
        }
        this->NameVector.push_back(i->Name);
    }
    for(ElectricElement* i:this->DynamicVector) {
        if(std::find(this->NameVector.begin(), this->NameVector.end(), i->Name) != this->NameVector.end()) {
            std::cout << "\n\"" << i->Name << "\" has been used more than once in your circuit, this can cause problems with controlled sources.\n\n";
        }
        this->NameVector.push_back(i->Name);
    };
    for(ElectricElement* i:this->SwitchVector){
        if(std::find(this->NameVector.begin(), this->NameVector.end(), i->Name) != this->NameVector.end()) {
            std::cout << "\n\"" << i->Name << "\" has been used more than once in your circuit, this can cause problems with controlled sources.\n\n";
        }
        this->NameVector.push_back(i->Name);
    } ;


}

ElectricElement* Circuit::requestElement(std::string name){
    int index=0;
    std::vector<std::string>::iterator it = std::find(this->NameVector.begin(), this->NameVector.end(), name);
    if (it == this->NameVector.end())
        std::cout << ("WARNING: NO \""+name+"\" ELEMENT FOUND.\n");
    else{
        index = std::distance(this->NameVector.begin(), it);
        if(index<this->SingleStampVector.size()){
            return SingleStampVector[index];

        }
        else if(index<this->SingleStampVector.size()+this->ElectricVector.size()){
            index -= this->SingleStampVector.size();
            return ElectricVector[index];
        }
        else if(index<this->SingleStampVector.size()+this->ElectricVector.size()+this->DynamicVector.size()){
            index -= this->SingleStampVector.size()+this->ElectricVector.size();
            return DynamicVector[index];
        }
        else{
            index -= this->SingleStampVector.size()+this->DynamicVector.size()+this->ElectricVector.size();
            return SwitchVector[index];
        }
}
}

void Circuit::importState(){
    int state;
    state = this->readState();
    this->InvAdmittanceMatrix = *(this->invMatrixStorage[state]);
    this->AdmittanceMatrix = *(this->MatrixStorage[state]);
}

void Circuit::preSimulation(std::ifstream &netlist){
    this->importNetlist(netlist);
    this->calculateDimension();
    this->storeNames();

    this->staticAdmittanceStamp();
    this->staticRHSStamp();
    this->Save();
    this->registerVector();
    this->createStateMatrices();
    this->linkElements();
}

void Circuit::Simulate(){
    this->time=0;
    this->importState();
    while(this->time<this->simTime){
        //
        //CHANGE controlSignal HERE
        //
        //
        //END CHANGE controlSignal Here
        //
        for(switchDevice* i: this->SwitchVector){
            i->calculateState();
        }
        this->importState();
        this->newStep();
        this->RHSStamp();
        this->Solve();
//        this->ToStringSolved();
//        std::cout << this->readState() << "\n";
        if(this->switchFlag==1){
            this->time += this->timeStep;
        }
        if(this->outputFile.is_open()) this->writeOutput();
        this->time += this->timeStep;
    }
}

void Circuit::SetOutputFile(std::string OutputFile){
    this->outputFile.open(OutputFile);
    this->outputFile<<"Time";
};

void Circuit::addTracker(std::string Name, int Var){
    this->outputFile<<"\t"<<Name;
    this->trackedVars.push_back(Var);
};

void Circuit::writeOutput(){
    this->outputFile << "\n";
    this->outputFile << this->time;
    for(int i:this->trackedVars){
        this->outputFile << "\t";
        this->outputFile << this->SolvedVector[i];
    }
};

void Circuit::closeOutputFile(){
    this->outputFile.close();
}
























