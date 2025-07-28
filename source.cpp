//Jay Olson
//CIS 1202-800
//July 25, 2025
//Final Project - Read EDI 810 Invoice

/*

Content also in readme file on GitHub:

For this project, I leveraged my experience working in the Department of Defense and Air Force contracting eBusiness to make a program that can read an electronic invoice in the American National Standards Institute (ANSI) X12 electronic data interchange (EDI) 810 format. I'm quite familiar with X12, but doing something with it programmatically like this is entirely new to me and was a huge challenge that took me a couple weeks and a couple scrapped iterations before this one -- they were so different that I decided just to scrap them for parts and take a different approach each time. 

The DoD's business is more complex than a commercial grocery chain's, so I decided to use Kroger's January 2025 implementation convention (IC) posted at https://edi.kroger.com/EDIPortal/documents/Maps/kroger-modernized-systems/Kroger-Modernized-systems_EDI810.pdf. This program merely handles most of the basics, although concepts like looping segments are ignored. Perhaps I'll return to that once I'm more experienced with programming and make this more robust.


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

There are several complexities I did not attempt to implement here that would make this much more robust. For example, as previously mentioned, items can be "looped" (an example of this is the two IT1 lines). Also note that segment names can be repeated and contents may vary depending on where in the "document" a segment is located (header, detail, or summary). And character encoding issues popped up with line breaks when I tried to read in the file as binary text... Things that I didn't know how to tackle because they're beyond my skill level. I therefore slightly modified the sample file contents in Kroger's IC document. I removed carriage returns in the data file used with the program, as a silent \r\n character was inhibiting my progress. I also removed content from BIG04 because it kept feeding in garbage. I first wrote code to read in as a binary file, but abandoned that approach because I couldn't get the tokens to parse quite correctly. I ended up reusing the code to write binary output instead. My goal here is merely to demonstrate as many concepts as I reasonably could from CIS 1202 in a final project program. Much more work could be done in a future effort for schema validations and perhaps even generating an 810 from user-provided data.

Concepts from the course: tons of string processing, including both C++ string objects and C-strings; pointers; dynamic memory allocation with arrays; STL vector; advanced file operations with both binary and ASCII text (I did not include seekg/tellg in the ultimate product since I changed file read methods); structs, enums, classes, inheritance, passing many types of objects by reference, and more.

See the README in the GitHub repository for how to use the program.

*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <cmath>
#include "Schema.h"
#include "InvDocument.h"
#include "ElementData.h"
//#include "TestFunctions.h"
using namespace std;


fstream openInvoiceInputFile();
string readInvoiceInputFile(fstream&, int&, int&);
void closeInvoiceInputFile(fstream&);
InvDocument* populateInvoiceDocumentStructureArr(InvDocument*, string, const int, const int);
vector <ElementData>& populateElementDataVect(vector <ElementData>&, InvDocument*, const int, const int);
string generateElementID(string, int);
void displayElementDataVectContents(vector <ElementData>&);
int lookupSequenceNumberForElement(vector <ElementData>&, string);
double convertStringtoDoubleCustom(string);
fstream& openBinaryOutputFile(fstream&);
void closeBinaryOutputFile(fstream&);
void renderInvoiceForHumans(vector <ElementData>&);
fstream& renderInvoiceForHumans(vector <ElementData>&, fstream&); //Overloaded function.

int displayMenu(int VIEW_HUMAN_INVOICE_ON_CONSOLE, int OUTPUT_HUMAN_INVOICE_TO_FILE, int VIEW_MACHINE_INVOICE, int QUIT);
bool getYNResponseAsBool();


int main() {

	const int VIEW_HUMAN_INVOICE_ON_CONSOLE = 1; //I could have made these global, but they only get passed to menuSelection, so easy enough to manage this way.
	const int OUTPUT_HUMAN_INVOICE_TO_FILE = 2;
	const int VIEW_MACHINE_INVOICE = 3;
	const int QUIT = 4;

	bool again = true;
	int menuSelection;
	fstream invoiceInputFile;
	fstream invoiceBinaryOutputFile;
	string invoiceInputFileContentsStr;
	int totalElementDelimiterCounter = 0;
	int totalLineDelimiterCounter = 0;


	//*******************************************************************************************************************************************************************************
	//This is all preprocessing activity before getting to the menu/first user prompt.


	//Open, read, close inputFile to pre-process/get set up. Also get the number of rows/columns (even though each row has a variable number of contents) while reading it.
	
	try {

		invoiceInputFile = openInvoiceInputFile();

	}
	
	catch (string exceptionMsg) {

		cout << exceptionMsg;
		cout << "Try one more time and press any key: ";
		cin.get();
		invoiceInputFile = openInvoiceInputFile();

	}

	catch (...) {

		cout << "Please ensure the input file is correctly placed and named, then re-run the program." << endl;
		exit(EXIT_SUCCESS);

	}


	invoiceInputFileContentsStr = readInvoiceInputFile(invoiceInputFile, totalElementDelimiterCounter, totalLineDelimiterCounter);
	closeInvoiceInputFile(invoiceInputFile);


	//Create a dynamically allocated array to store the invoice document structure info.
	InvDocument* invDocumentStructureArr = nullptr;
	invDocumentStructureArr = new InvDocument[totalLineDelimiterCounter];
	
	invDocumentStructureArr = populateInvoiceDocumentStructureArr(invDocumentStructureArr, invoiceInputFileContentsStr, totalElementDelimiterCounter, totalLineDelimiterCounter);

	vector <ElementData> elementDataVect;

	elementDataVect = populateElementDataVect(elementDataVect, invDocumentStructureArr, totalElementDelimiterCounter, totalLineDelimiterCounter);



	//*******************************************************************************************************************************************************************************
	//This is for the menu.

	cout << "Kroger EDI 810 Invoice Reader and Generator" << endl;
	cout << "By Jay Olson" << endl;
	cout << "-------------------------------------------" << endl << endl;


	while (again) {

		menuSelection = displayMenu(VIEW_HUMAN_INVOICE_ON_CONSOLE, OUTPUT_HUMAN_INVOICE_TO_FILE, VIEW_MACHINE_INVOICE, QUIT);

		switch (menuSelection) {

		case VIEW_HUMAN_INVOICE_ON_CONSOLE:

			system("cls"); //Clear the screen to remove clutter.
			renderInvoiceForHumans(elementDataVect); //This is the console output version.

			break;

		case OUTPUT_HUMAN_INVOICE_TO_FILE:

			system("cls"); //Clear the screen to remove clutter.
			openBinaryOutputFile(invoiceBinaryOutputFile);
			renderInvoiceForHumans(elementDataVect, invoiceBinaryOutputFile); //The is the file output version.
			closeBinaryOutputFile(invoiceBinaryOutputFile);
			cout << "File output complete. If a previous file existed, it has been overwritten. See \"invoiceOutputFile.dat\" in the program's directory." << endl;

			break;

		case VIEW_MACHINE_INVOICE:

			system("cls"); //Clear the screen to remove clutter.
			displayElementDataVectContents(elementDataVect);

			break;


		case QUIT:

			cout << "Program terminating..." << endl;
			return 0;

		default:
			return 1;

		}

		cout << "\n\n\nWOULD YOU LIKE TO MAKE ANOTHER MENU CHOICE? (Y/N): ";
		again = getYNResponseAsBool();
		cout << endl << endl;

	}



	delete[] invDocumentStructureArr; //Do some memory management. This array isn't needed after the vector is populated... At least in this implementation.
	invDocumentStructureArr = nullptr;


	cout << endl << endl;
	system("pause");
	return 0;

}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 

//*******************************************************************************************************************************************
//
//Function openInvoiceInputFile simply opens a file and passes it back (I did not pass it back by reference, although I could have. These
//files are pretty lightweight. Process and close activites are left to other functions.
//
//*******************************************************************************************************************************************


fstream openInvoiceInputFile() {

	fstream invoiceInputFile;
	invoiceInputFile.open("krogerSampleInvoice810.dat", ios::in);
	string cannotLocateFileException = "ERROR. File cannot open. Please check the directory.\n";

	if (!invoiceInputFile) {

		throw cannotLocateFileException;

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


	//Iterate through each element using the element length and handling for the delimiter *.


	for (int i = 0; i < totalLineDelimiterCounter; i++) { //Iterate through each document line.

		contents << invDocumentStructureArr[i].getLineContents(); //Assign the contents of the line from the document structure array to a stringstream that can then be parsed into tokens with getline.

		tempSegmentID = invDocumentStructureArr[i].getSegmentID(); //Use this to make a linkage with each element and line using the segmentID from InvDocument. This is where all that work to get to inheritance pays off.
		
		for (int j = 0; j < invDocumentStructureArr[i].getNumElements(); j++) {
			
			getline(contents, token, elementDelimiter);
			
			if (token == "") { //Explicitly write that a token is null if there's nothing between delimiters.
				token = "NULL";
			}

			//Populate elements.

			tempObject.setStrValue(token);
			tempObject.setElementLength(token.length());
			tempObject.setSegmentID(tempSegmentID);
			tempObject.setElementNum(generateElementID(tempSegmentID, j)); //Function generateElementID does some work that I offloaded to simplify the instant function.

			elementDataVect.push_back(tempObject);

			

		}

		contents.str("");
		contents.clear();

	}

	return elementDataVect;

}



//*******************************************************************************************************************************************
//
//Function generateElementID does some string manipulation to make an element ID for each element in a given segment. The function
//concatenates the alphanumeric segment ID + 0 (if the position is 0-9) + a numeric position, returning as a string.
//
//*******************************************************************************************************************************************

string generateElementID(string segmentID, int elementSequenceNumber) {

	string generatedElementID;

	//First, make the sequence number itself a string

	if (elementSequenceNumber >= 0 && elementSequenceNumber < 10) {

		generatedElementID = "0";
		generatedElementID = generatedElementID.append(to_string(elementSequenceNumber));
		
	}

	else {

		generatedElementID = to_string(elementSequenceNumber);

	}

	//Second, prepend the segmentID

	generatedElementID = segmentID.append(generatedElementID);

	return generatedElementID;

}



//*******************************************************************************************************************************************
//
//Function displayElementVectContents prints a few pieces of data from an EDI 810 file to the console in a tabular format.
//
//*******************************************************************************************************************************************

void displayElementDataVectContents(vector <ElementData>& elementDataVect) {

	cout << "#" << setw(20) << "Element ID" << setw(30) << "Value" << endl;
	cout << "----------------------------------------------------------------------------" << endl;

	for (int i = 0; i < elementDataVect.size(); i++) {

		cout << i << setw(20); //0-9 being single digit makes the formatting quite not aligned, but that's not worth picking at.
		elementDataVect[i].displayElemNum();
		cout << setw(30);
		elementDataVect[i].displayStrValue();
		cout << endl;

	}

}

int lookupSequenceNumberForElement(vector <ElementData>& elementDataVect, string segmentID) {

	int sequenceNumberForElement = -1;

	try {

		for (int i = 0; i < elementDataVect.size(); i++) {

			if (segmentID == elementDataVect[i].getElementNum()) {
				sequenceNumberForElement = i;
			}

		}

	}

	catch (string exceptionValueNotFound) {

		cout << exceptionValueNotFound << endl;
		exit(EXIT_SUCCESS);

	}

	if (sequenceNumberForElement == -1){

		string exceptionValueNotFound = "Error! Element not found in elementDataVect.";

	}

	return sequenceNumberForElement;

}



//*******************************************************************************************************************************************
//
//Function convertStringtoDoubleCustom is a exactly as described: it is a custom function I wrote to take in a string and convert it to a
//double. This demonstrates some low-level stuff with C-strings and pointers.
//
//*******************************************************************************************************************************************

double convertStringtoDoubleCustom(string strToConvert) {

	double dblConverted = 0.0;
	int strLength;
	const char* stringStartPtr; //Have to use const char* with the c_str function because that function returns a pointer to a const char*.
	

	stringStartPtr = nullptr;
	
	strLength = strToConvert.length();

	stringStartPtr = strToConvert.c_str();

	dblConverted = atof(stringStartPtr); //The book says atof converts to a double (I would have thought it'd be float). This appears to work.

	return dblConverted;

}



//*******************************************************************************************************************************************
//
//Function openBinaryOutputFile opens a file expected to receive output from another function.
//
//*******************************************************************************************************************************************

fstream& openBinaryOutputFile(fstream& binaryOutputFile) {

	binaryOutputFile.open("invoiceOutputFile.dat", ios::in | ios::binary); //Check to see whether the file is already open.

	if (binaryOutputFile.fail()) {
		//The file does not exist, so create it.
		binaryOutputFile.open("invoiceOutputFile.dat", ios::out | ios::binary);

	}

	else {

		binaryOutputFile.close(); //This is just here temporarily. The error handler should do something.

	}

	return binaryOutputFile;

}




//*******************************************************************************************************************************************
//
//Function closeBinaryOutputFile closes a file passed in by reference from another function.
//
//*******************************************************************************************************************************************

void closeBinaryOutputFile(fstream& binaryOutputFile) {

	if (binaryOutputFile.fail()) {

		//Do nothing.

	}

	else {

		binaryOutputFile.close();

	}


}



//*******************************************************************************************************************************************
//
//Function renderInvoiceForHumans takes key elements from the populated array with ElementData objects and marries with the 810 IC schema
// to provide a human-readable view. This is an overloaded function that takes in one of two values: 0 for console or 1 for file.
//
//*******************************************************************************************************************************************

void renderInvoiceForHumans(vector <ElementData>& elementDataVect) {
	
	cout << "Human-Readable Invoice" << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
	
	cout << "TOP-LEVEL" << endl;
	cout << "_________________________________" << endl << endl;
	cout << BIG02.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG02")].getStrValue() << endl; //BIG02 = Vendor Name
	cout << BIG01.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG01")].getStrValue() << endl; //BIG01 = Invoice Date
	cout << BIG04.elementName << " (" << BIG04.description << "): " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG04")].getStrValue() << endl; //BIG04 = PO Ref Number

	//I'm taking a short-cut here... If I weren't trying to demonstrate use of structs as Schema for the class, I'd change that to be a class and make a bunch of member functions that would do something like take in N101, which is the party name qualifier and return a different value to render for each (VN = Vendor, ST = Ship To). All qualifiers/enumerations could be spelled out. Instead, I'm just going to hard-wire VN here for this exercise since this is the only N101 in the sample file.

	cout << "Vendor ";
	cout << N102.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "N102")].getStrValue() << endl << endl; //N102 = Party Name

	cout << "\nLINE ITEM DETAIL:*" << endl;
	cout << "_________________________________" << endl << endl;

	cout << IT107.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT107")].getStrValue() << endl; //IT107 = Product/Svc ID
	cout << IT102.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT102")].getStrValue() << endl; //IT102 = Qty
	cout << IT103.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT103")].getStrValue() << endl; //IT103 = Unit of Measure

	//Convert string to double with my custom function to then be able to set precision to display dollar amount at 2 characters even though the same file used has four passed in. Yes, there's a lot going on here.
		
	cout << IT104.elementName << "**: $" << setprecision(2) << fixed << convertStringtoDoubleCustom(elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT104")].getStrValue()) << endl; //IT104 = Unit Price

	cout << "\nSUMMARY:" << endl;
	cout << "_________________________________" << endl << endl;
	//Same idea here as above, but adding the round function to demonstrate CMATH since it's a requirement for the assignment. This is a pretty monstrous line that I pieced together
	//one step at a time. All the years using Excel come in handy. When reading this, start getting the sequence number lookup for TDS01. This allows you to get the string value from 
	//the vector. Then the string value can be converted into a double, which can then be rounded, which could then be formatted to display with two digits.

	cout << TDS01.elementName << "***^: $" << setprecision(2) << fixed << round(convertStringtoDoubleCustom(elementDataVect[lookupSequenceNumberForElement(elementDataVect, "TDS01")].getStrValue())) << endl; //IT104 = Unit Price
	

	cout << "\n\nNOTES:" << endl;
	cout << "_________________________________" << endl;
	cout << "\n*I cut some corners here. This project assumes only one line item is submitted on the invoice. Perhaps I'll extend it to handle segment loops one day.";
	cout << "\n\n**When more than two decimal places are used, they are not formatted for display here even though they are still carried behind the scenes.";
	cout << "\n\n***" << TDS01.description << endl;
	cout << "\n\n^Amount listed here is rounded up to nearest dollar (to demonstrate CMATH)." << endl;

}



fstream& renderInvoiceForHumans(vector <ElementData>& elementDataVect, fstream& fout) { //I used fout here to make it easier to compare between this function and the one using cout. Just pass in the binaryOutputFile and fout serves as an alias within this scope.

	fout << "Human-Readable Invoice" << endl;
	fout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl << endl;
	//If a value isn't returned as expected, lookupSequenceNumberForElement terminates the program when the exception is thrown. This may be a bit severe, but I do not ever expect an issue -- at least with the EDI file used as a sample.

	fout << "TOP-LEVEL" << endl;
	fout << "_________________________________" << endl << endl;
	fout << BIG02.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG02")].getStrValue() << endl; //BIG02 = Vendor Name
	fout << BIG01.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG01")].getStrValue() << endl; //BIG01 = Invoice Date
	fout << BIG04.elementName << " (" << BIG04.description << "): " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "BIG04")].getStrValue() << endl; //BIG04 = PO Ref Number

	//I'm taking a short-cut here... If I weren't trying to demonstrate use of structs as Schema for the class, I'd change that to be a class and make a bunch of member functions that would do something like take in N101, which is the party name qualifier and return a different value to render for each (VN = Vendor, ST = Ship To). All qualifiers/enumerations could be spelled out. Instead, I'm just going to hard-wire VN here for this exercise since this is the only N101 in the sample file.

	fout << "Vendor ";
	fout << N102.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "N102")].getStrValue() << endl << endl; //N102 = Party Name

	fout << "\nLINE ITEM DETAIL:*" << endl;
	fout << "_________________________________" << endl << endl;

	fout << IT107.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT107")].getStrValue() << endl; //IT107 = Product/Svc ID
	fout << IT102.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT102")].getStrValue() << endl; //IT102 = Qty
	fout << IT103.elementName << ": " << elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT103")].getStrValue() << endl; //IT103 = Unit of Measure

	//Convert string to double with my custom function to then be able to set precision to display dollar amount at 2 characters even though the same file used has four passed in. Yes, there's a lot going on here.

	fout << IT104.elementName << "**: $" << setprecision(2) << fixed << convertStringtoDoubleCustom(elementDataVect[lookupSequenceNumberForElement(elementDataVect, "IT104")].getStrValue()) << endl; //IT104 = Unit Price

	fout << "\nSUMMARY:" << endl;
	fout << "_________________________________" << endl << endl;
	//Same idea here as above, but adding the round function to demonstrate CMATH since it's a requirement for the assignment. This is a pretty monstrous line that I pieced together
	//one step at a time. All the years using Excel come in handy. When reading this, start getting the sequence number lookup for TDS01. This allows you to get the string value from 
	//the vector. Then the string value can be converted into a double, which can then be rounded, which could then be formatted to display with two digits.

	fout << TDS01.elementName << "***^: $" << setprecision(2) << fixed << round(convertStringtoDoubleCustom(elementDataVect[lookupSequenceNumberForElement(elementDataVect, "TDS01")].getStrValue())) << endl; //IT104 = Unit Price


	fout << "\n\nNOTES:" << endl;
	fout << "_________________________________" << endl;
	fout << "\n*I cut some corners here. This project assumes only one line item is submitted on the invoice. Perhaps I'll extend it to handle segment loops one day.";
	fout << "\n\n**When more than two decimal places are used, they are not formatted for display here even though they are still carried behind the scenes.";
	fout << "\n\n***" << TDS01.description << endl;
	fout << "\n\n^Amount listed here is rounded up to nearest dollar (to demonstrate CMATH)." << endl;


	return fout; //Return fout so it can be closed from a different calling function.

}




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//*******************************************************************************************************************************************
//
//Function displayMenu is the main menu for this program. It asks the user for a choice of capability and provides the answer back to main.
//
//*******************************************************************************************************************************************

int displayMenu(int VIEW_HUMAN_INVOICE_ON_CONSOLE, int OUTPUT_HUMAN_INVOICE_TO_FILE, int VIEW_MACHINE_INVOICE, int QUIT) {

	int userChoice = -1;

	cout << "Please enter a selection from the menu below." << endl << endl;
	cout << "1. View human-readable invoice from EDI 810 file on the CONSOLE." << endl;
	cout << "2. View human-readable invoice from EDI 810 file in a BINARY FILE." << endl;
	cout << "3. View machine-readable invoice from EDI 810 file on the CONSOLE." << endl;
	cout << "4. Quit." << endl << endl;
	cout << "Selection: ";

	cin >> userChoice;

	while (userChoice < VIEW_HUMAN_INVOICE_ON_CONSOLE || userChoice > QUIT) {
		cout << "Invalid input. Please enter an integer between " << VIEW_HUMAN_INVOICE_ON_CONSOLE << " and " << QUIT << ": ";
		cin >> userChoice;
	}

	return userChoice;

}



//*******************************************************************************************************************************************
//
//Function getYNResponseAsBool is a reusable utility function that can collect a Y/N char answer from the console, 
//validate input, andd return a boolean value with 0 = N and 1 = Y.
//
//*******************************************************************************************************************************************

bool getYNResponseAsBool() {

	bool response;
	char charResponse = 'X';

	if (cin.fail()) {

		cin.ignore();

	}

	cin >> charResponse;
	charResponse = toupper(charResponse);

	while (charResponse != 'Y' && charResponse != 'N') {

		cout << "Invalid input. Please enter \"Y\" or \"N\": ";
		cin >> charResponse;
		toupper(charResponse);

	}

	if (charResponse == 'Y') {

		response = true;

	}

	else {

		response = false;

	}

	cout << endl;

	return response;

}