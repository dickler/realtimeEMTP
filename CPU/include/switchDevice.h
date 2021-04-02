#pragma once
#include "ElectricElement.h"
#include <functional>
class switchDevice :
	public ElectricElement
{
	public:
        int flag;

		virtual void idealStamp(Circuit& circuit) = 0;
        virtual void calculateState() = 0;
		double Ih;
		double S = 1;
        int ID;

        void virtual stampConductance(Circuit& circuit) = 0;
        void virtual stampPejovic(Circuit& circuit) = 0;
        void virtual stampMANA(Circuit& circuit) = 0;

};

