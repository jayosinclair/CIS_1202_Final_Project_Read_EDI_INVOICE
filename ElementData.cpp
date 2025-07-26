#include "ElementData.h"
using namespace std;


ElementData::ElementData() {

	elementNum = "";
	strValue = "";
	elementLength = 0;

}

ElementData::ElementData(string elemNum, string strVal, int elemLen){

	elementNum = elemNum;
	strValue = strVal;
	elementLength = elemLen;

}

