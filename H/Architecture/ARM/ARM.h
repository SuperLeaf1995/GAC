#ifndef ARM_H_
#define ARM_H_
#include "../../OpC/OpC.h"
namespace ARM
{
	vector<OpC*> OpCodes;
	void Factory();
	int ARM_ADD(vector<int> Parameters)
	{
		return Parameters.at(0) + Parameters.at(1);
	}

	int ARM_SUB(vector<int>Parameters)
	{
		return Parameters.at(0) - Parameters.at(1);
	}

	int ARM_IMUL(vector<int>Parameters)
	{
		return Parameters.at(0) * Parameters.at(1);
	}

	int ARM_IDIV(vector<int>Parameters)
	{
		return Parameters.at(0) / Parameters.at(1);
	}

	int ARM_MUL(vector<int>Parameters)
	{
		return Parameters.at(0) * Parameters.at(1);
	}

	int ARM_DIV(vector<int>Parameters)
	{
		return Parameters.at(0) / Parameters.at(1);
	}
}


#endif