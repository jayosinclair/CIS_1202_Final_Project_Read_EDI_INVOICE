# CIS_1202_Final_Project_Read_EDI_INVOICE

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

There are several complexities I did not attempt to implement here that would make this much more robust. For example, items can be "looped" (an example of this is the two IT1 lines). Also note that segment names can be repeated and contents may vary depending on where in the "document" a segment is located (header, detail, or summary). And character encoding issues popped up with line breaks when I tried to read in the file as binary text... Things that I didn't know how to tackle because they're beyond my skill level. I left such complexities to a project for another day and therefore slightly modified the sample file contents in Kroger's IC document. I removed carriage returns in the data file used with the program, as a silent \r\n character was inhibiting my progress. I also removed content from BIG04 because it kept feeding in garbage. I first wrote code to read in as a binary file, but abandoned that approach because I couldn't get the tokens to parse quite correctly. I stored all tokens as strings rather than the data types that probably best match contents -- this was for the sake of simplicity. My goal here is merely to demonstrate as many concepts as I reasonably could from CIS 1202 in a final project program. I'd love to come back to this after I learn more and fine tune details.
