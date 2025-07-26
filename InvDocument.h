#ifndef INVDOCUMENT_H
#define INVDOCUMENT_H

#include <iostream>
#include <string>
using namespace std;


//The InvDocument class stores key information needed to store document-level segment information for storing / processing using at least one other child class.

class InvDocument{

	protected:

		string segmentID;
		int segmentIDLen;
		int numElements;
		int sequence;
	
	public:

		//Constructors and destructor

		InvDocument()  //These are all so short that I didn't break them into a separate cpp implementation file.

		{
			segmentID = "";
			segmentIDLen = 0;
			numElements = 0;
			sequence = 0;
		}

		InvDocument(string segIDVal, int segmentIDLength, int numElems, int seq, int elementLen)

		{
			segmentID = segIDVal;
			segmentIDLen = segmentIDLength;
			numElements = numElems;
			sequence = seq;
		}

		~InvDocument() {}



		//Mutators

		void setSegmentID(string segIDVal)
		{
			segmentID = segIDVal;
		}

		void setSegmentIDLen(int segmentIDLength) 
		{
			segmentIDLen = segmentIDLength;
		}

		void setNumElements(int numElems)
		{
			numElements = numElems;
		}

		void setSequence(int seq)
		{
			sequence = seq;
		}



		//Accessors

		string getSegmentID() const
		{
			return segmentID;
		}

		int getSegmentIDLen() const
		{
			return segmentIDLen;
		}

		int getNumElements() const
		{
			return numElements;
		}

		int getSequence() const 
		{
			return sequence;
		}



		void displaySegmentID() //TODO: Should const be displayed here? There's already a guard with the accessor in the called get functions.
		{
			std::cout << getSegmentID();
		}

		void displaySegmentIDLen()
		{
			std::cout << getSegmentIDLen();
		}

		void displayNumElements()
		{
			std::cout << getNumElements();
		}

		void displaySequence()
		{
			std::cout << getSequence();
		}



};


#endif