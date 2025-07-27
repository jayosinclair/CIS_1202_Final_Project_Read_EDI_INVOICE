//Jay Olson
//CIS 1202-800
//July 25, 2025
//Final Project - Read EDI 810 Invoice

/*

Content also in readme file on GitHub:

For this project, I leveraged my experience working in the Department of Defense and Air Force/Navy contracting eBusiness to make a program that can read an electronic invoice in the American National Standards Institute (ANSI) X12 electronic data interchange (EDI) 810 format. The DoD's business is more complex than a commercial grocery chain's, so I decided to use Kroger's January 2025 implementation convention (IC) posted at https://edi.kroger.com/EDIPortal/documents/Maps/kroger-modernized-systems/Kroger-Modernized-systems_EDI810.pdf. This program merely handles most of the basics, although things like looping segments are ignored.


Here are contents from the .txt file that is used with the submittal of this program, followed by an explanation.

ST*810*0001~
BIG*20210520*5615789**9~
N1*VN*VENDOR NAME*92*123456~
IT1*ST*KROGER OCADO - MONROE*9*1178579681001~
IT1**2*CA*9.6**UK*10120009998646~
TDS*3840~
SE*8*0001~


Explanation:

Each line in an X12 file like this starts with what is called a "segment." The segment is a collection of related data elements. Some data elements are values themselves, and some are qualifiers of those values. Some values are free-from, and others are expected to be used from an enumeration list. For example, the first "N1" segment on line 3 represents "Party Identification," as detailed on page 8 of the IC. The N1 line is divided into tokens, delimited by * characters (and the line is terminated with a ~). Each token is assigned a position number after the segment ID:

N101 = VN (a qualifier)
N102 = VENDOR NAME

IT101 = Not populated (there is nothing between the delimiters
IT102 = Quantity invoiced
IT103 = Unit of Measure (CA = CASE)

There are several complexities I did not attempt to implement here that would make this much more robust. For example, items can be "looped" (an example of this is the two IT1 lines). Also note that segment names can be repeated and contents may vary depending on where in the "document" a segment is located (header, detail, or summary). And character encoding issues popped up with line breaks when I tried to read in the file as binary text... Things that I didn't know how to tackle because they're beyond my skill level. I left such complexities to a project for another day and therefore slightly modified the sample file contents in Kroger's IC document. I removed carriage returns in the data file used with the program, as a silent \r\n character was inhibiting my progress. I also removed content from BIG04 because it kept feeding in garbage. I first wrote code to read in as a binary file, but abandoned that approach because I couldn't get the tokens to parse quite correctly. My goal here is merely to demonstrate as many concepts as I reasonably could from CIS 1202 in a final project program. I'd love to come back to this after I learn more and fine tune details.

*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <cmath> //TODO: Need to find a reason to use cmath since it's in the rubric... maybe a rounding function or something?
#include "Schema.h"
#include "InvDocument.h"
#include "ElementData.h"
using namespace std;




fstream openInvoiceInputFile();
string readInvoiceInputFile(fstream&, int&, int&);
void closeInvoiceInputFile(fstream&);
InvDocument* populateInvoiceDocumentStructureArr(InvDocument*, string, const int, const int);
void displayInvDocumentArrContents(const int, InvDocument*);
vector <ElementData>& populateElementDataVect(vector <ElementData>&, InvDocument*, const int, const int);
void displayElementDataVectContents(vector <ElementData>&);



int main() {

	fstream invoiceInputFile;
	string invoiceInputFileContentsStr;
	int totalElementDelimiterCounter = 0;
	int totalLineDelimiterCounter = 0;

	//Open, read, close inputFile to pre-process/get set up. Also get the number of rows/columns (even though each row has a variable number of contents) while reading it.
	invoiceInputFile = openInvoiceInputFile();
	invoiceInputFileContentsStr = readInvoiceInputFile(invoiceInputFile, totalElementDelimiterCounter, totalLineDelimiterCounter);
	closeInvoiceInputFile(invoiceInputFile);


	//Create a dynamically allocated array to store the invoice document structure info.
	InvDocument* invDocumentStructureArr = nullptr;
	invDocumentStructureArr = new InvDocument[totalLineDelimiterCounter]; //TODO: Make sure to dealloc later.
	
	invDocumentStructureArr = populateInvoiceDocumentStructureArr(invDocumentStructureArr, invoiceInputFileContentsStr, totalElementDelimiterCounter, totalLineDelimiterCounter);


	//displayInvDocumentArrContents(totalLineDelimiterCounter, invDocumentStructureArr); //This call is just here in for testing.


	////Create a dynamically allocated array to store element data.
	//ElementData* elementDataArr = nullptr;
	//elementDataArr = new ElementData[totalElementDelimiterCounter]; //TODO: Make sure to dealloc later.

	vector <ElementData> elementDataVect;

	elementDataVect = populateElementDataVect(elementDataVect, invDocumentStructureArr, totalElementDelimiterCounter, totalLineDelimiterCounter);

	//For testing:

	displayElementDataVectContents(elementDataVect);



	cout << endl << endl;
	system("pause");
	return 0;

}




//*******************************************************************************************************************************************
//
//Function openInvoiceInputFile simply opens a file and passes it back (I did not pass it back by reference, although I could have. These
//files are pretty lightweight. Process and close activites are left to other functions.
//
//*******************************************************************************************************************************************


fstream openInvoiceInputFile() {

	fstream invoiceInputFile;
	invoiceInputFile.open("krogerSampleInvoice810.dat", ios::in);

	if (!invoiceInputFile) {

		cout << "Error";
		exit(EXIT_SUCCESS); //TODO: Maybe write an exception rather than abort the whole program.

	}

	return invoiceInputFile;

}




//*******************************************************************************************************************************************
//
//Function readInvoiceInputFile takes in the inputFile by reference, modifies reference variables for a couple key things to set up other 
//functions (number of lines, number of elements total), and outputs all the inputFile's contents into a string variable.
//
//*******************************************************************************************************************************************

string readInvoiceInputFile(fstream &invoiceInputFile, int &totalElementDelimiterCounter, int &totalLineDelimiterCounter) {

	string fileContentsStr;

	char elementDelimiter = '*';
	char lineDelimiter = '~';

	totalElementDelimiterCounter = 0; //Just to be safe.
	totalLineDelimiterCounter = 0;

	while (getline(invoiceInputFile, fileContentsStr)) {

		for (int i = 0; i < fileContentsStr.length(); i++) {

			if (fileContentsStr.at(i) == elementDelimiter) {
				totalElementDelimiterCounter++;
			}

			else if (fileContentsStr.at(i) == lineDelimiter) {
				totalLineDelimiterCounter++;
			}

		}

	}
	
	return fileContentsStr;

}



//*******************************************************************************************************************************************
//
//Function closeInvoiceInputFile simply closes the inputFile if it is open.
//
//*******************************************************************************************************************************************

void closeInvoiceInputFile(fstream &invoiceInputFile) {

	if (invoiceInputFile.is_open()) {
		invoiceInputFile.close();
	}

}



//*******************************************************************************************************************************************
//
//Function populateInvoiceDocumentStructureArr populates the invoiceDocumentStructureArr array of type InvDocument to have key items
//about the document's structure in one place within InvDocument instances' member variables. InvDocument is the base class for the ElementData
//derived class.
//
//*******************************************************************************************************************************************

InvDocument* populateInvoiceDocumentStructureArr(InvDocument* invoiceDocumentStructureArr, string fileContentsStr, const int totalElementDelimiterCounter, const int totalLineDelimiterCounter) {

	int index = 0;
	int lineElementCounter = 0;
	int lineLength = 0;
	char elementDelimiter = '*';
	char lineDelimiter = '~';
	char charBuffer = '@';
	stringstream fileContentsStream; //Setting up a stringstream to make parsing much friendlier.
	stringstream lineContentsStream;
	string lineBuffer;
	string elementBuffer;

	fileContentsStream << fileContentsStr;
	
	//This first loop gets all things EXCEPT the number of elements in an array. While this could probably be written a bit more efficiently with a single loop, I figured I was tempting fate.

	while (getline(fileContentsStream, lineBuffer, lineDelimiter)){ //Go through the entire document, which is now represented as FileContentsStream.
		
		invoiceDocumentStructureArr[index].setLineContents(lineBuffer);
		invoiceDocumentStructureArr[index].setLineLength(lineBuffer.length());
		invoiceDocumentStructureArr[index].setSequence(index + 1);

		lineContentsStream << lineBuffer;

		getline(lineContentsStream, elementBuffer, elementDelimiter); //Go through each line to just extract the first segment (beginning of line until * delimiter).
		invoiceDocumentStructureArr[index].setSegmentID(elementBuffer);
		invoiceDocumentStructureArr[index].setSegmentIDLen(elementBuffer.length());

		index++;

		lineContentsStream.str("");  //This isn't covered in the Gaddis book (I don't think it is, at least), so I had to research why my segmentLinesArray wasn't advancing with each iteration. Turns out the entireLine needs to be reset/cleared each time using these member functions. Can't just overwrite with each pass through the for loop. https://cplusplus.com/reference/sstream/stringstream/?kw=stringstream
		lineContentsStream.clear();

	}

	//Second loop gets the number of delimiters per line and populates the arribute for each item in the InvDocument array accordingly. Clear the buffer first just to be safe.
	lineBuffer = "";

	for (int i = 0; i < totalLineDelimiterCounter; i++) {

		lineBuffer = invoiceDocumentStructureArr[i].getLineContents();
		lineElementCounter = 0;

		for (int j = 0; j < lineBuffer.length(); j++) {

			if (lineBuffer.at(j) == elementDelimiter) {
				lineElementCounter++;
			}

		}

		invoiceDocumentStructureArr[i].setNumElements(lineElementCounter + 1); //Add 1 to the counter since we just counted delimiters and there's content ahead of the first delimiter.

	}

	return invoiceDocumentStructureArr;

}



//*******************************************************************************************************************************************
//
//Function displayInvDocumentArrContents prints the contents of each of the display memember functions of the InvDocment class when used with
//invDocumentStructureArr element instances.
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
//Function populateElementDataVect extracts tokens from the EDI file, with the help of data stored within invDocumentStructureArr and general
//row/column-like size information previously sought, and populates a vector containing ElementData objects. The output of this function
//is the elementDataVect vector passed back by reference with every element in the EDI document mapped to a particular segment address.
//
//*******************************************************************************************************************************************

vector <ElementData>& populateElementDataVect(vector <ElementData>& elementDataVect, InvDocument* invDocumentStructureArr, const int totalElementDelimiterCounter, const int totalLineDelimiterCounter) {

	string token;
	string tempSegmentID;
	char elementDelimiter = '*';
	stringstream contents;
	ElementData tempObject;


	/*
	
	elementNum = "";
	strValue = "";
	elementLength = 0;
	
	*/


		//Iterate through each element using the element length and handling for the delimiter *.


	for (int i = 0; i < totalLineDelimiterCounter; i++) { //Iterate through each document line.

		contents << invDocumentStructureArr[i].getLineContents(); //Assign the contents of the line from the document structure array to a stringstream that can then be parsed into tokens with getline.

		tempSegmentID = invDocumentStructureArr[i].getSegmentID();
		
		for (int j = 0; j < invDocumentStructureArr[i].getNumElements(); j++) {
			
			getline(contents, token, elementDelimiter);
			
			if (token == "") { //Explicitly write that a token is null if there's nothing between delimiters.
				token = "NULL";
			}

			tempObject.setStrValue(token);
			tempObject.setElementLength(token.length());
			tempObject.setSegmentID(tempSegmentID);

			/*TODO: Populating the elementNumber for each token, such as ST01, ST02, etc is going to be tricky. We need to use the segmentID at the line level to populate all the elementNumber values for a given line.HOWEVER, we also need to find a way to concatenate the token number itself with the segmentID, adding a 0 between the segmentID and the token number is a single digit.Perhaps break this into a separate function?
			
			To demonstrate:

			segmentID = ST (this can come from InvDocument)
			token number = 1
			Need to add a 0 between ST and 1 to get ST01

			But, say we had 14 elements on the ST line... we would not add a filler 0. We'd just have ST14.

			I think it'd make sense to loop through all the lines, to say for each line, assign the segment ID now that we know how many segments exist per line. Then we need to assign an element sequence number. Then concatenate

			*/


			elementDataVect.push_back(tempObject);

		}

		cout << endl;
		contents.str("");
		contents.clear();

	}

	return elementDataVect;

}








void displayElementDataVectContents(vector <ElementData>& elementDataVect) {

	for (int i = 0; i < elementDataVect.size(); i++) {

		cout << "Element " << i << ": ";
		elementDataVect[i].displayStrValue();
		cout << "   ";
		elementDataVect[i].displayElementLength();
		cout << "   ";
		elementDataVect[i].displaySegmentID();

		cout << endl;

	}

}