//Jay Olson
//CIS 1202-800
//July 25, 2025
//Final Project - Read EDI 810 Invoice

/*

For this project, I leveraged my experience working in the Department of Defense in contracting eBusiness to make a program that can read an electronic invoice in the American National Standards Institute (ANSI) X12 electronic data interchange (EDI) 810 format. The DoD's business is more complex than a commercial grocery chain's, so I decided to use Kroger's January 2025 implementation convention (IC) posted at https://edi.kroger.com/EDIPortal/documents/Maps/kroger-modernized-systems/Kroger-Modernized-systems_EDI810.pdf.


Here are contents from the .txt file that is used with the submittal of this program, followed by an explanation.

ST*810*0001~
BIG*20210520*5615789**900000004801~
N1*VN*VENDOR NAME*92*123456~
N1*ST*KROGER OCADO - MONROE*9*1178579681001~
IT1**2*CA*9.6**UK*10120009998646~
IT1**2*CA*9.6**UK*10120009990224~
TDS*3840~
SE*8*0001~


Explanation:

Each line in an X12 file like this starts with what is called a "segment." The segment is a collection of related data elements. Some data elements are values themselves, and some are qualifiers of those values. Some values are free-from, and others are expected to be used from an enumeration list. For example, the first "N1" segment on line 3 represents "Party Identification," as detailed on page 8 of the IC. The N1 line is divided into tokens, delimited by * characters (and the line is terminated with a ~). Each token is assigned a position number after the segment ID:

N101 = VN (a qualifier)
N102 = VENDOR NAME

IT101 = Not populated (there is nothing between the delimiters
IT102 = Quantity invoiced
IT103 = Unit of Measure (CA = CASE)

There are several complexities I did not attempt to implement here that would make this much more robust. For example, items can be "looped" (an example of this is the two IT1 lines). Also note that segment names can be repeated and contents may vary depending on where in the "document" a segment is located (header, detail, or summary). And character encoding issues popped up with line breaks when I tried to read in the file as binary text... Things that I didn't know how to tackle because they're beyond my skill level. I left such complexities to a project for another day. My goal here is merely to demonstrate as many concepts as I reasonably could from CIS 1202 in a final project program.

*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include "Schema.h"
#include "InvDocument.h"
#include "ElementData.h"
using namespace std;




fstream openInvoiceInputFile();
string readInvoiceInputFileLine(fstream&, int&);



int main() {

	fstream invoiceInputFile;
	string invoiceInputFileLine;
	int delimiterCounter = 0;
	int lineCounter = 0;
	
	invoiceInputFile = openInvoiceInputFile();

	while (invoiceInputFile) {

		readInvoiceInputFileLine(invoiceInputFile, delimiterCounter);
		lineCounter++;

	}

	cout << "\nLines: " << lineCounter;

	

	cout << endl << endl;
	system("pause");
	return 0;

}



fstream openInvoiceInputFile() {

	fstream invoiceInputFile;
	invoiceInputFile.open("krogerSampleInvoice810.dat", ios::in);

	if (!invoiceInputFile) {

		cout << "Error";
		exit(EXIT_SUCCESS); //TODO: Maybe write an exception rather than abort the whole program.

	}

	return invoiceInputFile;


}



string readInvoiceInputFileLine(fstream &invoiceInputFile, int &delimiterCounter) {

	string contents;
	char delimiter = '*';
	char singleChar = '@'; //Initialize to garbage.

	delimiterCounter = 0;


	for (int i = 0; i < contents.length(); i++) {

		if (contents.at(i) == delimiter) {
			delimiterCounter++;
		}

	}

	return contents;

}