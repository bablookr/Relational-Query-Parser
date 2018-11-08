/*
 *functions.h
 *
 *  Header file
 */

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <map>

using namespace std;


struct Table{                                                           // Structure for a table
    string name;
    vector<pair<string,string> > att;
    vector<vector<string> > vec;
};


Table* ParseFile(const char* filename);                                 // File parser

Table* Select(Table* table,string args);                                // Select operation

Table* Project(Table* table,string args);                               // Project operation

Table* Rename(Table* table,string args);                                // Rename operation

Table* Union(Table* table1, Table* table2);                             // Union operation

Table* SetDifference(Table* table1,Table* table2);                      // Set difference operation

Table* CartesianProduct(Table* table1,Table* table2);                   // Cartesian product operation

Table* Operate(Table* table,string operation,string args);              // Operate function

Table* ParseQuery(string query);                                        // Query parser

map<string,int> IndexMap(Table* table);                                 // Index map of a table

vector<string> postfix(string args);                                    // Infix to postfix converter

bool isOperator(string c);                                              // Checks if a char is an operator

bool check(string st,string value,string operation);                    // Checks the truth of a statement

bool match(char c1,char c2);                                            // Checks if the brackets match

bool bracketsBalanced(string query);                                    // Checks if the brackets are balanced

void Display(Table* table);                                             // Display the contents of a  table

void QueryFormats();                                                    // Display the query formats on command prompt


#endif // FUNCTIONS_H_INCLUDED
