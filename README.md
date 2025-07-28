# CIS_1202_Final_Project_Read_EDI_INVOICE

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


HOW TO USE THIS PROGRAM:

The complexity with this program is entirely behind the scenes. From an end-user perspective, it's a simple menu-driven program.

The user is presented with four options:

1. View human-readable invoice from EDI 810 file on the CONSOLE.
2. View human-readable invoice from EDI 810 file in a BINARY FILE.
3. View machine-readable invoice from EDI 810 file on the CONSOLE.
4. Quit.

<img width="1482" height="762" alt="image" src="https://github.com/user-attachments/assets/710bd60a-b243-4ea7-9092-f716a2c65eb5" />


Selecting the first option will display the sample EDI 810 file, but rendered for human eyes. The rendering isn't a full rendering of all the EDI file's contents, but it shows a few. The schema struct file is leveraged to provide context to data elements.

Selecting the second option will show the same output as the first option, but it'll print to a binary DAT file at the project's directory instead. If a file named "invoiceOutputFile.dat" previously existed in the directory, it'll be replaced/overwritten when selecting this option.

Selecting the third option will display some of the critical elements of the EDI file on the console.

The fourth option is to quit the program.


It is important to note that not any EDI 810 file can be used with this program; just use "krogerSampleInvoice810.dat" as provided. The limiting factors are: 1.) it is assumed that Kroger's implementation convention is used (so DoD invoices would have some different implementation items from its own IC, for example). Also, since I skipped implementing segment loops because it would have made the project much more complex, having more than one line item, address, etc, would result in some unexpected behavior with my rendering logic since I didn't implement loops there. 
