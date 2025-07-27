#ifndef ELEMENTDATA_H
#define ELEMENTDATA_H

#include <string>
#include "InvDocument.h"
using namespace std;

class ElementData : public InvDocument {

private:

	string elementNum;
	string strValue;
	int elementLength;

public:

	ElementData(); //See ElementData.cpp for definitions

	ElementData(string elemNum, string strVal, int elemLen);

	~ElementData() {}



	//Mutators

	void setElementNum(string elemNum)
	{
		elementNum = elemNum;
	}


	void setStrValue(string strVal)
	{
		strValue = strVal;
	}

	void setElementLength(int elemLen)
	{
		elementLength = elemLen;
	}


	//Accessors

	string getElementNum() const
	{
		return elementNum;
	}
	

	string getStrValue() const
	{
		return strValue;
	}

	int getElementLength() const
	{
		return elementLength;
	}


	void displayElemNum()
	{
		std::cout << getElementNum();
	}

	void displayStrValue()
	{
		std::cout << getStrValue();
	}

	void displayElementLength()
	{
		std::cout << getElementLength();
	}


};

#endif