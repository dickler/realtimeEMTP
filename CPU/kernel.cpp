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
#define _USE_MATH_DEFINES
#include <math.h>


#include "Circuit.h"
#include "ElectricElement.h"
#include "Resistance.h"
#include "Capacitor.h"
#include "DCVoltageSource.h"
#include "ACVoltageSource.h"
#include "Inductor.h"
#include "ACCurrentSource.h"
#include "DCCurrentSource.h"
#include "Diode.h"
#include "SquareWave.h"
#include "Switch.h"



//This recieves an element and stamps it into the also recieved circuit, after this it deletes the element
void Stamp(ElectricElement* Element, Circuit& circuit, bool Adm, bool RightHand, bool Del) {
	if (Adm)
		(*Element).stamp(circuit);
	if (RightHand)
		(*Element).stampRightHand(circuit);
	if (Del)
		delete Element;

};

void SeeMatrix(Circuit& circuit) {
	circuit.ToString();
	circuit.ToStringSource();
	circuit.ToStringSolved();

}


void createFromTxt(std::ifstream& netlist, std::vector<ElectricElement*>& ElectricVector, Circuit& circuit)
{
	std::vector<std::string> extraLineTypes;
	std::vector<int> Nodes;
	std::string type;
	std::string name;
	double DC;
	int node1;
	int node2;
	int Gate;
	double value;
	double simTime;
	double step;
	int numNodes = 0;
	int numSource = 0;
	double frequency;
	double phase;
	double SwitchIndex;
	double OFFSET;
	std::vector<bool> sVector;


	extraLineTypes.push_back("DCV");
	extraLineTypes.push_back("ACV");
	extraLineTypes.push_back("L");
	extraLineTypes.push_back("C");
	extraLineTypes.push_back("SQV");



	if (netlist.is_open()); {

		netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		while (netlist >> type >> name) {
			netlist >> node1 >> node2;
			netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


			if (std::find(Nodes.begin(), Nodes.end(), node1) == Nodes.end()) {
				Nodes.push_back(node1);
				numNodes++;
			}
			if (std::find(Nodes.begin(), Nodes.end(), node2) == Nodes.end()) {
				Nodes.push_back(node2);
				numNodes++;
			}

			if (std::find(extraLineTypes.begin(), extraLineTypes.end(), type) != extraLineTypes.end()) {
				numSource++;
			}
		}


		netlist.clear();
		netlist.seekg(0, netlist.beg);

		netlist >> name >> simTime >> step;

		circuit.setValues(numNodes - 1, numSource, simTime, step);

		std::ofstream myfile;

		myfile.open("example.txt");
		for (int i = 0; i < circuit.numNodes; i++) {
			myfile << 'V' << (i + 1) << '\t';
		}
		myfile.close();

		netlist.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		while (netlist >> type) {
			if (type == "R") {
				netlist >> name >> node1 >> node2 >> value;
				Resistance* R = new Resistance(value, node1, node2);
				Stamp(R, circuit, 1, 0, 1);
			}

			else if (type == "DCV") {
				netlist >> name >> node1 >> node2 >> value;
				DCVoltageSource* DCV = new DCVoltageSource(value, node1, node2, circuit);
				Stamp(DCV, circuit, 1, 0, 1);
			}

			else if (type == "L") {
				netlist >> name >> node1 >> node2 >> value;
				ElectricVector.push_back(new Inductor(value, node1, node2, circuit));
			}

			else if (type == "C") {
				netlist >> name >> node1 >> node2 >> value;
				ElectricVector.push_back(new Capacitor(value, node1, node2, circuit));
			}

			else if (type == "ACV") {
				netlist >> name >> node1 >> node2 >> value >> frequency >> phase;
				ElectricVector.push_back(new ACVoltageSource(value, frequency, phase, node1, node2, circuit));
			}

			else if (type == "SQV") {
				netlist >> name >> node1 >> node2 >> value >> frequency >> phase >> DC >> OFFSET;
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
				ElectricVector.push_back(new Diode(value, node1, node2));
			}

			else if (type == "S") {
				netlist >> name >> node1 >> node2 >> Gate >> value;
				ElectricVector.push_back(new Switch(value, node1, node2, Gate));
			}

		}


		for (int i = 0; i < ElectricVector.size(); i++) {
			(*ElectricVector[i]).stamp(circuit);
		}

		circuit.registerVector();
	}

}


int main(void) {

	Circuit circuit = Circuit();
	std::vector<ElectricElement*> ElectricVector;
	std::ifstream netlist;
	netlist.open("netlist.txt");
	createFromTxt(netlist, ElectricVector, circuit);
	std::ostringstream myStr;
	myStr << "Time \t Diode \t Switch \n";
	SeeMatrix(circuit);
	circuit.InvertMatrix();


	for (; circuit.time <= circuit.simTime; circuit.time += circuit.timeStep) {
		circuit.newStep();

		for (auto i : ElectricVector) {
			Stamp(i, circuit, 0, 1, 0);
		}


		circuit.Solve();

		for (int i = 0; i < circuit.totalSize; i++) {
			myStr << circuit.SolvedMatrix[i] << "\t";
		}

		myStr << circuit.time;
		myStr << "\n";

	}


	std::ofstream myfile;
	myfile.open("example.txt", std::ios::out | std::ios::app);
	myfile << myStr.str();
	myfile.close();

	return 0;
}