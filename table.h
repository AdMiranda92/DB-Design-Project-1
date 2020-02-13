#ifndef __TABLE_H
#define __TABLE_H

#include <string>
#include <iostream>
#include <vector>

using namespace std;

//File name of table data file = (table name) + ".dat"
const string DATA_FILE_EXTENSION = ".dat";

enum datatype_t { UNKNOWN, BOOL, INT, STRING };

class Table;
class Row;
class Query;

class Table
{
public:
  vector<vector<string>> tableContents;
  std::string name;
  
  //Class constructor
  Table(string &name):name(name){}

  //Constructs a Table based on the data in the given CSV file
  //Name of Table should be the string before ".csv"
  static Table* readTableFromCSV(const string& file);

  //Constructs a Table based on the data in the file (name + ".dat")
  static Table* getTableByName(const string& name);

  //Returns this table's name
  string getName() const {return name;};

  //Returns the datatype of the given attribute
  datatype_t getAttributeType(const string& attName) const{
    string name = attName;
    if(name == "year"){
      return INT;
    }

    if(name == "category"){
      return STRING;
    }

    if(name == "winner"){
      return BOOL;
    }

    if(name == "entity"){
      return STRING;
    }

    return UNKNOWN;
  };

  void addRow(vector<string>& values){
    tableContents.push_back(values);
  }
  //Writes this Table to the file (name + ".dat")
  //Should be a binary file, but format is up to you
  void writeToFile() const;

  //Runs the given query on this Table, and returns a new Table with the result
  //The rows of the new Table should be the ones that cause q.getCondition()->getBoolValue(row) to return true
  //The attributes should be the ones returned by q.getAttributesToReturn(), unless
  // q.getAttributesToReturn returns the single attribute "*" (new table has all attributes)
  Table* runQuery(Query& q) const{
    Query *req = &q;
    vector<string> attributes = req->getAttributesToReturn();

    // need to search through the tableCOntents here
    // req->getCondition()->getBoolValue(tableContents);
  };
};

class Row
{
public:
  int *year;
  std::string *category, *entity;
  bool *winner;
  //Creates a dummy row with no data
  Row(){
    year = nullptr;
    category = nullptr;
    entity = nullptr;
    winner = nullptr;
  };

  //Returns the value of the given attribute for this row
  const void* getValue(const string& attName) const {
    std::string name = attName;
    if(name == "year"){
      return year;
    }else if(name == "category"){
      return category;
    }else if(name == "entity"){
      return entity;
    }else{
      return winner;
    }
  };
};

//Prints the given Table to an output stream
//Format is the same as the CSV file
ostream& operator<<(ostream& os, const Table& table);

#endif //__TABLE_H