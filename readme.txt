// ----------------------------------------------------------------
// $ Revision:  1.0 $
// Creator: Alastair Robertson (frontaccounting@kwikpay.co.nz)
// date_:   2013-06-03
// Title:   Tax report module
// Free software under GNU GPL
// ----------------------------------------------------------------


Summary:
---------

This module provides setup and calculation functions for a generic tax report. It has been designed with Australian and New Zealand tax reporting in mind, but is intended to be useful for other jurisdictions.

The set up form, located on the set up page, miscellaneous category menu, defines each line in the tax report. Each line calculates a single total as either:
- the sum of one or more tax code's associated invoices or their tax values
- the sum or or or more general ledger accounts
- a calculation using the values of previous lines and arithmetic operations
- a user entered value

Each line is identified by a code, which is used in the calculation to refer to the value of the line.

The tax report lines are calculated and displayed in the sequence shown in the set up screen. You can use the up and down arrows to change the sequence individual lines.

The tax codes and accounts associated with each line vary according to the chart of accounts, so must be configured for each database. If using either the Australian or New Zealand pre-defined example reports, you must edit each report line and select the tax codes or account codes to be accumulated. Selcet multiple tax codes or accoutns by pressing the ctrl key whilst clicking each code you want to include in the accumulation for the report line.

If you inactivate all but one report, the remaining report will be run automatically, when selected from the menu. If you have multiple active tax reports, you will be offered a combo box to select which report to run.


Tax code totals:
----------------
For the one or more tax codes selected, the total will be accumulated from either:
- Sales invoice total including tax
- Sales invoice total less tax
- Sales tax amount
- Purchases invoice total including tax
- Purchase invoice total less tax
- Purchases tax amount

It the report basis is 'Accrual', transactions are selected when the invoice date is within the reporting period, otherwise, if the report basis is 'Payment', transactions are selected on the date payment is received or made for the invoice.

Account totals:
---------------
For each account selected, the total of all transactions with a date within the selected reporting period will be accumulated.

Calculation:
------------
The calcualtion line type displays the result of a simple arithmetic calculation based on previously calculated lines and numeric constants. If a calculation includes the code of a previous line, the calcualted value of that code's line is used in the calculation. If the code is numeric, it has to be enclosed by { }, other wise it will be assumed to be a numeric constant.
Examples of calculations are:

W1 + W2 + W3 + W4
This adds up the values of lines W1, W2, W3 and W4

T1 * T3 / 100
This calculates a percentage of the value of line T1, where line T3 contains the percentage rate

{8A} - {8B}
Subtracts the value of line 8B from the value of line 8A. The line codes are enclosed in {} otherwise they could be interpreted as the value 8.

