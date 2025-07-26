#ifndef SCHEMA_H
#define SCHEMA_H

#include <string>
using namespace std;

/*

This is my attempt at deciphering the Kroger EDI 810 implementation convention to electronically send invoice transactions to Kroger. I did this by hand, and there's undoubtedly a couple mistakes in actual content given the tedious nature. The implementation convention (IC) can be found at: 

https://edi.kroger.com/EDIPortal/documents/Maps/kroger-modernized-systems/Kroger-Modernized-systems_EDI810.pdf


*/



enum DocLocation{ HEADING, DETAIL, SUMMARY };
enum Required{ MANDATORY, OPTIONAL, CONDITIONAL };

struct Invoice {

	DocLocation placement;
	string id;
	int pos;
	string name;
	Required reqIndicator;
	int maxUse;
	int repeatLimit;
	string loopID;

};

//Instantiations to use in constructing EDI document as a whole:
//----------------------------------------------------------

//Heading Invoices:

Invoice seg_ST_heading{ HEADING, "ST", 0000, "Transaction Set Header", MANDATORY, 1, 1, "None" }; //ST segment is not defined in Kroger's IC, but it is convention... It is metadata about the transaction marking the beginning of the file, just as SE segment is not defined and is the trailer for the file. Think of it as an envelope for contents.
Invoice seg_BIG_heading{ HEADING, "BIG", 0200, "Beginning Segment for Invoice", MANDATORY, 1, 0, "None" };
Invoice seg_CUR_heading{ HEADING, "CUR", 0400, "Currency", OPTIONAL, 1, 0 };
Invoice seg_N1_heading_loop1{ HEADING, "N1", 0700, "Party Identification", OPTIONAL, 1, 200, "N1" };
Invoice seg_N1_heading_loop2{ HEADING, "N1", 0700, "Party Identification", OPTIONAL, 1, 200, "N1" };
Invoice seg_ITD_heading{ HEADING, "ITD", 1300, "Terms of Sale/Deferred Terms of Sale", OPTIONAL, 999, 0, "None" };


//Detail Invoices:

Invoice seg_IT1_detail{ DETAIL, "IT1", 0100, "Baseline Item Data (Invoice)", OPTIONAL, 1, 0, "IT1" };
Invoice seg_IT3_detail{ DETAIL, "IT3", 0300, "Additional Item Data", OPTIONAL, 5, 0, "IT1" };
Invoice seg_SAC_detail{ DETAIL, "SAC", 1800, "Service, Promotion, Allowance, or Charge Information", OPTIONAL, 1, 0, "SAC" };



//Summary Invoices:

Invoice seg_TDS_summary{ SUMMARY, "TDS", 0100, "Total Monetary Value Summary", MANDATORY, 1, 0, "None" };
Invoice seg_SAC_summary{ SUMMARY, "SAC", 0400, "Service, Promotion, Allowance, or Charge Information", OPTIONAL, 1, 0, "SAC" };
Invoice seg_SE_summary{ SUMMARY, "SE", 0000, "Ending Segment", MANDATORY, 1, 1, "None" }; //See comment above at ST segment.



//Generic Segment structure:
//----------------------------------------------------------


struct Segment {

	string segmentId;
	string ref;
	int id;
	string elementName;
	Required reqIndicator;
	string type;
	int minUse;
	int maxUse;
	bool mustUse;
	string description;

};


//Instantiations for Segments:
//----------------------------------------------------------


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Heading Level
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//BIG = Beginning Segment for Invoice

Segment BIG01{ "BIG", "BIG01", 373, "Invoice Issue Date", MANDATORY, "DT", 8, 8, 1, "Date expressed as CCYYMMDD where CC represents the first two digits of the calendar year. Note: Invoice issue date cannot be in the future." };

Segment BIG02{ "BIG", "BIG02", 76, "Invoice Number", MANDATORY, "AN", 1, 22, 1, "Identifying number assigned by issuer." };

Segment BIG03{ "BIG", "BIG03", 373, "PO Issue Date", OPTIONAL, "DT", 8, 8, 0, "Date expressed as CCYYMMDD where CC represents the first two digits of the calendar year." };

Segment BIG04{ "BIG", "BIG04", 324, "Purchase Order Number", OPTIONAL, "AN", 1, 22, 0, "Identifying number for Purchase Order assigned by the orderer/purchaser." };



//CUR = Currency

Segment CUR01{ "CUR", "CUR01", 98, "Entity Identifier Code", MANDATORY, "ID", 2, 3, 1, "Code identifying an organizational entity, a physical location, property or an individual." };

Segment CUR02{ "CUR", "CUR02", 100, "Currency Code", MANDATORY, "ID", 3, 3, 0, "Code (Standard ISO) for country in whose currency the charges are specified. Only required if not US Dollars." };



//N101 = Party Identification. Note the Kroger IC has two loops, but I consolidated them here because they're basically the same, but for code/enumeration values/qualifiers.

Segment N101{ "N1", "N101", 98, "Entity Identifier Code", MANDATORY, "ID", 2, 3, 1, "Code identifying an organizational entity, a physical location, property, or an individual." }
;

Segment N102{ "N1", "N102", 93, "Name", OPTIONAL, "AN", 1, 60, 0, "Free-form name." };

Segment N103{ "N1", "N103", 66, "Identification Code Qualifier", CONDITIONAL, "ID", 1, 2, 0, "Code designating the system/method of code structure used for Identification Code (67)." };

Segment N104{ "N1", "N104", 67, "Identification Code", CONDITIONAL, "AN", 2, 80, 0, "Differing infromation for Ship To vs Supplier ID for this field. See the implementation convention." };



//ITD = Terms of Sale/Deferred Terms of Sale

Segment ITD03{ "ITD", "ITD03", 338, "Terms Discount Percent" , OPTIONAL, "R", 1, 6, 0, "Terms discount percentage, expressed as a percent, available to the purchaser if an invoice is paid on or before the Terms Discount Due Date. "};

Segment ITD05{ "ITD", "ITD05", 351, "Terms Discount Days Due", CONDITIONAL, "N0", 1, 3, 0, "Number of days in the terms discount period by which payment discount is earned." };

Segment ITD06 { "ITD", "ITD06", 446, "Terms Net Due Date", OPTIONAL, "DT", 8, 8, 0, "Date when total invoice amount becomes due expressed in CCYYMMDD where CC represents the first two digits of the calendar year." };

Segment ITD07 { "ITD", "ITD07", 386, "Terms Net Days", OPTIONAL, "N0", 1, 3, 0, "Number of days until total invoice amount is due (discount not applicable)." };

Segment ITD08{ "ITD", "ITD08", 362, "Terms Discount Amount", OPTIONAL, "N2", 1, 10, 0, "Total amount of terms discount." };



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Detail Level
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//IT1 = Baseline Item Data (Invoice)

Segment IT102{ "IT1", "IT102", 358, "Quantity Invoiced", CONDITIONAL, "R", 1, 15, 0, "Number of units invoiced (supplier units)." };

Segment IT103 { "IT1", "IT103", 355, "Unit or Basis for Measurement Code", CONDITIONAL, "ID", 2, 2, 0, "Code specifying the units in which a value is being expressed, or manner in which a measurement has been taken." };

Segment IT104 { "IT1", "IT104", 212, "Unit Price", CONDITIONAL, "R", 1, 17, 0, "Price per unit of product, service, commodity, etc." };

Segment IT106 { "IT1", "IT106", 235, "Product/Service ID Qualifier", CONDITIONAL, "ID", 2, 2, 0, "Code identifying the type/source of the descriptive number used in Product/Service ID (234). Note: must send at least 1 of the item formats from the purchase order." };

Segment IT107 { "IT1", "IT107", 234, "Product/Service ID", CONDITIONAL, "AN", 2, 2, 0, "Identifying number for a product or service." };

Segment IT108 { "IT1", "IT108", 235, "Product/Service ID Qualifier", CONDITIONAL, "ID", 2, 2, 0, "Code identifying the type/source of the descriptive number used in product/service ID (234). Note: only send 1 reference item format (UK/UP)." };

Segment IT109{ "IT1", "IT109", 234, "Product/Service ID", CONDITIONAL, "AN", 1, 48, 0, "Identifying number for a product or service." };



//IT3 = Additional Item Data

Segment IT301 { "IT3", "IT301", 382, "Number of Units Shipped", CONDITIONAL, "R", 1, 10, 0, "Numeric value of units shipped in manufacturer's shipping units for a line item or transaction set. Note: send if unit of measure code differs from IT103 as in Random Weight Items (LB)." };

Segment IT302 { "IT3", "IT302", 355, "Unit or Basis for Measurement Code", CONDITIONAL, "ID", 2, 2, 0, "Code specifying the units in which a value is being expressed, or manner in which a measurement has been taken. Note: CA = CASE. IT301 should contain number of cases & IT302 = CA. There's more in implementation convention to read..." };



//SAC = Service, Promotion, Allowance, or Charge Information

Segment SAC01_detail { "SAC", "SAC01", 248, "Allowance or Charge Indicator", MANDATORY, "ID", 1, 1, 1, "Code which indicates an allowance or charge for the service specified." };

Segment SAC02_detail { "SAC", "SAC02", 1300, "Service, Promotion, Allowance, or Charge Code", CONDITIONAL, "ID", 4, 4, 0, "Code identifying the service, promotion, allowance, or charge. Please refer to https://edi.kroger.com/EDIPortal/EDIGuideAndReq_OcadoGroup.html for a list of valid allowance/charge codes at the invoice and item level." };

Segment SAC08_detail { "SAC", "SAC08", 118, "Rate", OPTIONAL, "R", 1, 9, 0, "Rate expressed in the standard monetary denomination for the currency specified. Note: The rate is based on the same UOM (IT103) as the previous item/ IT1 segment. You must provide the decimal on the rate. Allowance rates must be negative; charge rates must be positive."};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Summary Level
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//TDS = Total Monetary Value Summary

Segment TDS01 {"TDS", "TDS01", 610, "Amount", MANDATORY, "N2", 1, 15, 1, "Monetary amount. Note: the total invoice amount (item quantities times cost, adjusted with any item allowance/charge; totaled for all items; adjusted with any invoice allowance/charge." };



//SAC = Service, Promotion, Allowance, or Charge Information

Segment SAC01_summary{ "SAC", "SAC01", 248, "Allowance or Charge Indicator", MANDATORY, "ID", 1, 1, 1, "Code which indicates an allowance or charge for the service specified." };

Segment SAC02_summary{ "SAC", "SAC02", 1300, "Service, Promotion, Allowance, or Charge Code", CONDITIONAL, "ID", 4, 4, 0, "Code identifying the service, promotion, allowance, or charge. Please refer to https://edi.kroger.com/EDIPortal/EDIGuideAndReq_OcadoGroup.html for a list of valid allowance/charge codes at the invoice and item level." };

Segment SAC05_summary{ "SAC", "SAC05", 610, "Amount", OPTIONAL, "N2", 1, 15, 0, "Monetary amount, 2 decimals are implied on the amount. Allowance amounts must be negative; charge amounts must be positive. Note: this SAC segment is for invoice level allowance/charge. Must combine if more than 1." };

#endif