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
    circuit.addTracker("V1",1);
//    circuit.addTracker("V2",3);

    /*
    Opens the netlist file and reads it to create the circuit and vectors
    */
    netlist.open("netlist.txt");
    circuit.preSimulation(netlist);
    circuit.Simulate();
    circuit.closeOutputFile();
}
