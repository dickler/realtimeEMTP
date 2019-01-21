#pragma once

#include "mkl.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <cmath>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#define _use_math_defines
#include <math.h>

/*
Elements Includes
*/

#include "switchDevice.h"
#include "electricelement.h"
#include "circuit.h"
#include "resistance.h"
#include "capacitor.h"
#include "dcvoltagesource.h"
#include "acvoltagesource.h"
#include "inductor.h"
#include "accurrentsource.h"
#include "dccurrentsource.h"
#include "diode.h"
#include "squarewave.h"
#include "switch.h"


#ifdef IdealSwitch
	#ifdef EXPANDED_SWITCH
		#undef EXPANDED_SWITCH
	#endif
#endif

#ifdef OptimizeG
#define EXPANDED_SWITCH
#endif

/*
Recieves an element and stamps it in the recieved circuit.

Adm, RightHand and Del determine if:
Adm -> The Admittance Matrix is going to be stamped.
RightHand -> RHS Vector is going to be stamped.
Del -> if the element should be deleted afterwards.
*/
void Stamp(ElectricElement* Element, Circuit& circuit,bool Adm,bool RightHand,bool Del) {
	if (Adm)
	(*Element).stamp(circuit);
	if (RightHand)
	(*Element).stampRightHand(circuit);
	if (Del)
	delete Element;

};

/*
Outputs:
-Admittance Matrix
-RHS vector
-Current Solved Vector
-Inverted Admittance Matrix
*/
void SeeMatrix(Circuit& circuit) {
	circuit.ToString();
	circuit.ToStringRHS();
	circuit.ToStringSolved();
	circuit.ToStringInv();
}

/*
Reads a .txt to:
-Create a circuit object
-Stamp the static members the Admittance and RHS vector
-Save dynamic components' object in a vector for later re-stamp
-Save switch in a separate vectors for a possible pejovic optimization
*/
void createFromTxt(std::ifstream& netlist, std::vector<ElectricElement*>& ElectricVector, std::vector<DynamicElement*>& DynamicVector, std::vector<switchDevice*>& SwitchVector,Circuit& circuit)
{
	//Initialization of variables
	std::vector<std::string> extraLineTypes;
	std::vector<int> Nodes;
	std::string type;
	std::string name;
	double Value0;
	double DC;
	int node1;
	int node2;
	int Gate;
	double value;
	double simTime;
	double step;
	int numNodes=0;
	int numSource=0;
	double frequency;
	double phase;
	double OFFSET;
	std::vector<bool> sVector;


	/* 
	All the types of included elements:
	R - Resistance
	L - Inductor
	C - Capacitor
	DCV - DC Voltage Source
	ACV - AC Voltage Source
	DCC - DC Current Source
	ACC - AC Current Source
	SQV - Square Voltage Source
	S - Switch
	D - Diode
	*/



	/*
	Adds the types of element which requires an line
	*/
	extraLineTypes.push_back("DCV");
	extraLineTypes.push_back("ACV");
	extraLineTypes.push_back("SQV");

#ifdef MNA_CAPACITOR
	extraLineTypes.push_back("L");
#endif

#ifdef MNA_CAPACITOR
	extraLineTypes.push_back("C");
#endif
	
#ifdef EXPANDED_SWITCH
	extraLineTypes.push_back("S");
	extraLineTypes.push_back("D");
#endif

	

	if (netlist.is_open()); {
		/*
		Run the first time the netlist to measure the size of the Admittance
		Create the Circuit element of said size.
		*/
		netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //Ignores the first line, it only contains time parameters

		/*
		Loops through the netlist getting the type and its nodes for each element 
		Ignores the last
		*/
		while (netlist >> type >> name) {
			netlist >> node1 >> node2;
			netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			/*
			Increment the size of the circuit for each node not in the already existent nodes
			*/	
			if (std::find(Nodes.begin(), Nodes.end(), node1) == Nodes.end()) {
				Nodes.push_back(node1);
				numNodes++;
			}
			if (std::find(Nodes.begin(), Nodes.end(), node2) == Nodes.end()) {
				Nodes.push_back(node2);
				numNodes++;
			}

			/*
			Increment the size of the circuit if the type is in the extraLineTypes
			*/
			if (std::find(extraLineTypes.begin(), extraLineTypes.end(), type) != extraLineTypes.end()) {
				numSource++;
			}
		}

		
		/*
		Re-initilize the netlist for a second sweep
		*/
		netlist.clear();
		netlist.seekg(0, netlist.beg);


		/*
		Gets the time parameters and create the circuit
		*/
		netlist >> name >> simTime >> step;
		netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		circuit.setValues(numNodes-1,numSource,simTime,step);


		/*
		Creates the header for the txt files
		*/
		std::ofstream myfile;
		myfile.open("example.txt");
		for (int i = 0; i < circuit.numNodes; i++) {
			myfile << 'V' << (i + 1) << '\t';
		}
		myfile.close();


		
		/*
		 Reads through the netlist and:
		 -Stamps and discards the static elements;
		 -Stores the dynamic elements:
			-Elements in general are stored in the ElectricVector;
			-Switches are stored in SwitchVector.
		*/
		while (netlist >> type ) {
			if (type == "R") {
				netlist >> name >> node1 >> node2 >> value;
				Resistance* R = new Resistance(value, node1, node2);
				Stamp(R, circuit,1,0,1);
			}

			else if (type == "DCV") {
				netlist >> name >> node1 >> node2 >> value;
				DCVoltageSource* DCV = new DCVoltageSource(value, node1, node2,circuit);
				Stamp(DCV, circuit, 1, 1, 1);
			}

			else if (type == "L") {
				netlist >> name >> node1 >> node2 >> value>> Value0;
				DynamicVector.push_back(new Inductor(value, node1, node2,Value0, circuit));
			}

			else if (type == "C") {
				netlist >> name >> node1 >> node2 >> value >> Value0;
				DynamicVector.push_back(new Capacitor(value, node1, node2, Value0, circuit));
			}

			else if (type == "ACV") {
				netlist >> name >> node1 >> node2 >> value >> frequency >> phase ;
				ElectricVector.push_back(new ACVoltageSource(value, frequency, phase, node1, node2, circuit));
			}

			else if (type == "SQV") {
				netlist >> name >> node1 >> node2 >> value >> frequency >> phase >> DC >> OFFSET ;
				ElectricVector.push_back(new SquareWave(value, frequency, node1, node2, circuit, phase, DC, OFFSET));
			}

			else if (type == "ACC") {
				netlist >> name >> node1 >> node2 >> value >> frequency >> phase;
				ElectricVector.push_back(new ACCurrentSource(value, frequency, phase, node1, node2, circuit));
			}

			else if (type == "DCC") {
				netlist >> name >> node1 >> node2 >> value;
				DCCurrentSource* DCC = new DCCurrentSource(value, node1, node2);
				Stamp(DCC, circuit, 0, 1, 1);
			}

			else if (type == "D") {
				netlist >> name >> node1 >> node2 >> value;
				SwitchVector.push_back(new Diode(value, node1, node2,circuit));
			}

			else if (type == "S") {
				netlist >> name >> node1 >> node2 >> Gate >> value;
				SwitchVector.push_back(new Switch(value, node1, node2,Gate,circuit));
			}

		}


		/*
		Stamps the ElectricVector elements on the Admittance Matrix
		*/
		for (int i = 0; i < ElectricVector.size(); i++) {
			(*ElectricVector[i]).stamp(circuit);
		}
		for (int i = 0; i < DynamicVector.size(); i++) {
			(*DynamicVector[i]).stamp(circuit);
		}

		/*
		Saves the Admittance Matrix and the RHS vector
		*/
		circuit.Save();
		circuit.registerVector();

		for (int i = 0; i < DynamicVector.size(); i++) {
			(*DynamicVector[i]).initialize(circuit);
		}
	}
	
}




int main(void) {


	/*
	Declares the variables
	*/
	Circuit circuit = Circuit();
	std::vector<ElectricElement*> ElectricVector;
	std::vector<switchDevice*> SwitchVector;
	std::vector<DynamicElement*> DynamicVector;
	std::ifstream netlist;
	std::ostringstream myStr;
	/*
	Opens the netlist file and reads it to create the circuit and vectors
	*/
	netlist.open("netlist.txt");
	createFromTxt(netlist, ElectricVector, DynamicVector, SwitchVector, circuit);

	/*
	Adds an column for Time in the .txt
	*/
	myStr << "Time\n";


	/*
	For the Pejovic condutance G in the switch, opts between the netlist defined or an optimization.
	*/
	SeeMatrix(circuit);
#ifdef OptimizeG
	SeeMatrix(circuit);
	circuit.optimizeG(SwitchVector, 2, 200);
#else
	for (int j = 0; j < SwitchVector.size(); j++) {
		(*SwitchVector[j]).stamp(circuit);
	}
#endif
	circuit.InvertMatrix();
	SeeMatrix(circuit);



	/*
	Starts all switches on for the first iteration.
	*/
	for (int k = 0; k < SwitchVector.size(); k++) {
		(*SwitchVector[k]).S = 1;
	}

	for (; circuit.time <= circuit.simTime; circuit.time += circuit.timeStep) {

		
		
		/*Switch Keys*/
		//if (std::fmod(circuit.time, 50e-6) < 0.33*50e-6) {
		//	(*SwitchVector[1]).S = 1;
		//}
		//else if (std::fmod(circuit.time, 50e-6) > 0.33*50e-6) {
		//	(*SwitchVector[1]).S = 0;
		//}


		/*
		Stamps the mutable vectors.
		*/
		for (auto i : ElectricVector) {
			Stamp(i, circuit,0,1,0);
		}
		for (auto i : SwitchVector) {
			Stamp(i, circuit, 0, 1, 0);
		}
		for (auto i : DynamicVector) {
			Stamp(i, circuit, 0, 1, 0);
		}

		/*
		Solve the circuit equations
		*/
		circuit.Solve();


		/*
		Writes the txt with the variables for each step
		*/
		for (int i = 0; i < circuit.totalSize; i++) {
			myStr << circuit.SolvedVector[i] << "\t";
		}
		myStr << circuit.time;
		myStr << "\n";



		/*
		Loads newStep for the static RHSVector
		*/
		circuit.newStep();
	}

	/*
	Closes inserts myStr in the output txt
	*/
	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << myStr.str();
	myfile.close();

	return 0;
}