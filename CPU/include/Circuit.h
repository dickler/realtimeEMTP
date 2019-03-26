#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <complex>
#include "mkl.h"
#include "Defines.h"

class ElectricElement;
class DynamicElement;
class switchDevice;




//The Circuit class will store the nodal equations in matricial form,
//and handle it's solution
class Circuit {

private:
	/*
	Inaccessible members, handled only by some methods
	*/

    //Elements Vector
    std::vector<ElectricElement*>   SingleStampVector;
    std::vector<ElectricElement*>   ElectricVector;
    std::vector<switchDevice*>      SwitchVector;
    std::vector<DynamicElement*>    DynamicVector;

    //Output Variables
    std::ofstream outputFile;
    std::vector<int> trackedVars;




public:
    /*
    Matrix Variables
    */
    std::vector<double> StaticRightHandVector;	//Stored RightHandVector
    std::vector<double> StaticAdmittanceMatrix;	//Stored AdmittanceMatrix
    lapack_int * ipiv;							//Variable needed for the solver
    std::vector<double> AdmittanceMatrix;		// These are the matrices and vectors used in the Ax=b.
    std::vector<double> RightHandVector;		//
    std::vector<double> SolvedVector;			// They are vectors and linear matrices with row major configuration, i.e.:
    std::vector<double> InvAdmittanceMatrix;	// Aij = A[ (i*totalSize) + j ]
    std::vector<std::vector<double>*> MatrixStorage;
    std::vector<std::vector<double>*> invMatrixStorage;
    std::vector<std::string> NameVector;
    std::vector<bool*>               controlSignals;
	double V0 = 0;
	double time = 0;	//Current time in the simulation
	double timeStep;	//Time step for the simulation
	double simTime;		//Total simulation time
	int SourceId;		//Id the current Voltage Source when stamped, passed by to explicitate the line for the voltage input
	int numNodes;		//Number of non-ground nodes
	int numVSources;	//Number of voltage sources
	int totalSize;		//Total numNodes of the nodal admittance square matrix


    int switchFlag=0;





	/*
	Declaration of the methods in Circuit, each method description can be found in the .cpp file
	*/
	Circuit();

	Circuit(int numNodes, int numVSources, double time, double step);

	static Circuit& Default();

	void setValues(int numNodes, int numVSources, double time, double step);

	void init();

	void Solve();

	void InvertMatrix();

	void ToString();

	void ToStringRHS();

	void ToStringSolved();
	
	void ToStringInv();

	void registerVector();

	void Save();

	void Load();

	void newStep();

	void takeEigen(std::vector<double> Vec, std::vector<std::complex<double> > & EigenVec);

	void optimizeG(std::vector<switchDevice*> SwitchVector, double range = 2, int points = 200);

    void importNetlist(std::ifstream& netlist);

    void createElement(std::string Line);

    void calculateDimension();

    void staticAdmittanceStamp();

    void staticRHSStamp();

    void setState(int n);

    int  readState();

    void importState();

    void createStateMatrices();

    void linkElements();

    void storeNames();

    ElectricElement* requestElement(std::string name);

    void preSimulation(std::ifstream &netlist);

    void RHSStamp();

    void trapStepStamp();

    void Simulate();

    void SetOutputFile(std::string OutputFile);

    void addTracker(std::string Name, int Var);

    void writeOutput();

    void closeOutputFile();
};
