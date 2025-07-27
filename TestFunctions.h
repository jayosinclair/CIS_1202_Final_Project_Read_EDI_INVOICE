#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H

#include <iostream>
#include <string>
#include <vector>
#include "ElementData.h"
using namespace std;



//*******************************************************************************************************************************************
//
//Function displayInvDocumentArrContents prints the contents of each of the display memember functions of the InvDocment class when used with
//invDocumentStructureArr element instances. This function is not used in the actual program, but it's something I made for testing purposes.
//
//*******************************************************************************************************************************************


void displayInvDocumentArrContents(const int totalLineDelimiterCounter, InvDocument* invDocumentStructureArr) {

	for (int i = 0; i < totalLineDelimiterCounter; i++) {

		cout << "Sequence: ";
		invDocumentStructureArr[i].displaySequence();
		cout << "\nSegment ID: ";
		invDocumentStructureArr[i].displaySegmentID();
		cout << "\nSegment Length: ";
		invDocumentStructureArr[i].displaySegmentIDLen();
		cout << "\nLine Length: ";
		invDocumentStructureArr[i].displayLineLength();
		cout << "\nElement Count: ";
		invDocumentStructureArr[i].displayNumElements();
		cout << "\nWhole Line: ";
		invDocumentStructureArr[i].displayLineContents();
		cout << endl << endl;

	}
}








//*******************************************************************************************************************************************
//
//Function displayElementVectContents is not used in the program itself, but it's something I made for testing.
//
//*******************************************************************************************************************************************




void displayElementDataVectContents(vector <ElementData>& elementDataVect) {

	for (int i = 0; i < elementDataVect.size(); i++) {

		cout << "Sequence number: " << i << "  ";

		elementDataVect[i].displayElemNum();
		cout << "   ";

		elementDataVect[i].displayStrValue();

		cout << endl;

	}

}


#endif