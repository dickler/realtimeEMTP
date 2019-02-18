#include "mkl.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#define _use_math_defines
#include <math.h>

/*
Elements Includes
*/

#include "Elements.h"


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


int main(void) {



    /*
    Declares the variables
    */
    Circuit circuit = Circuit();
    std::ifstream netlist;

    circuit.SetOutputFile("example.csv");
    circuit.addTracker("Iv",2);
    /*
    Opens the netlist file and reads it to create the circuit and vectors
    */
    netlist.open("netlist.txt");
    circuit.preSimulation(netlist);
    circuit.Simulate();

    circuit.closeOutputFile();
//    std::ostringstream myStr;
//    /*
//    Adds an column for Time in the .txt
//    */
//    myStr << "Time\n";


//    /*
//    For the Pejovic condutance G in the switch, opts between the netlist defined or an optimization.
//    */
//    SeeMatrix(circuit);
//#ifdef OptimizeG
//    SeeMatrix(circuit);
//    circuit.optimizeG(SwitchVector, 2, 200);
//#else
//    for (int j = 0; j < SwitchVector.size(); j++) {
//        (*SwitchVector[j]).stamp(circuit);
//    }
//#endif
//    circuit.InvertMatrix();
//    SeeMatrix(circuit);



//    /*
//    Starts all switches on for the first iteration.
//    */
//    for (int k = 0; k < SwitchVector.size(); k++) {
//        (*SwitchVector[k]).S = 1;
//    }

//    for (; circuit.time <= circuit.simTime; circuit.time += circuit.timeStep) {



//        /*Switch Keys*/
//        //if (std::fmod(circuit.time, 50e-6) < 0.33*50e-6) {
//        //	(*SwitchVector[1]).S = 1;
//        //}
//        //else if (std::fmod(circuit.time, 50e-6) > 0.33*50e-6) {
//        //	(*SwitchVector[1]).S = 0;
//        //}


//        /*
//        Stamps the mutable vectors.
//        */
//        for (auto i : ElectricVector) {
//            Stamp(i, circuit,0,1,0);
//        }
//        for (auto i : SwitchVector) {
//            Stamp(i, circuit, 0, 1, 0);
//        }
//        for (auto i : DynamicVector) {
//            Stamp(i, circuit, 0, 1, 0);
//        }

//        /*
//        Solve the circuit equations
//        */
//        circuit.Solve();


//        /*
//        Writes the txt with the variables for each step
//        */
//        for (int i = 0; i < circuit.totalSize; i++) {
//            myStr << circuit.SolvedVector[i] << "\t";
//        }
//        myStr << circuit.time;
//        myStr << "\n";



//        /*
//        Loads newStep for the static RHSVector
//        */
//        circuit.newStep();
//    }

//    /*
//    Closes inserts myStr in the output txt
//    */
//    std::ofstream myfile;
//    myfile.open("example.txt", std::ios::out | std::ios::app);
//    myfile << myStr.str();
//    myfile.close();

//    return 0;
}
