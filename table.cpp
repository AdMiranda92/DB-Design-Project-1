#include "table.h"
#include <fstream>
#include <sstream>
#include <vector>

Table* Table::readTableFromCSV(const string& file){
    std::ifstream inputFile;
    inputFile.open(file);
    if(!inputFile.is_open()){
        return nullptr;
    }
    std::string name = file.substr(0, file.find('.')); 

    //Create the table and give it a name
    Table *newTable = new Table(name);
    std::string line;
    while (std::getline(inputFile, line)){
        // debug statement
        // std::cout << "Reading line: " << line << "\n";
        std::istringstream iss(line);
        vector<string> values;

        while(iss){
          std::string s;
          for(int i=1; i<=4; i++){
            if(i%4 != 0){
              if(!std::getline(iss, s, ',')){
                break;
              }
            }else{
              if(!std::getline(iss, s, '\n')){
                break;
            }
          }
          values.push_back(s);
          }
        }

        newTable->tableContents.push_back(values);
    }

    // table should now have a vector of string vectors holding all rows
    // the first row holds the table attributes
    // the second row holds the types of the attributes
    // all other rows contain information from the table
    return newTable;

};

void Table::writeToFile() const{
  std::ofstream outputFile;
  outputFile.open(name+".dat", ios::out | ios::binary);
  for(vector<string> i: tableContents){
    outputFile.write((char*)&i[0], i.size() * sizeof(string));
  }
  outputFile.close();
};

// still need to finish this function
// returning a table object to get rid of undefined error
Table* Table::runQuery(Query& q) const{
  std::string name = "testTable";
  Table* table = new Table(name);
  return table;
};

// still need to finish this function
// returning a table object to get rid of undefined error
Table* Table::getTableByName(const string& name){
  std::string tableName = name;
  Table *table = new Table(tableName);
  ifstream inFile;
  inFile.open(tableName+".dat", ios::in | ios::binary);
  vector<string> info(4);
  while(inFile.read((char*)&info, info.size() * sizeof(string))){
    table->addRow(info);
    info.empty();
  }
    inFile.close();
    return table;
};

ostream& operator<<(ostream& os, const Table& table){
  string data;
  int counter = 0;
  for(vector<string> i: table.tableContents){
    counter++;
    // we begin at one here to make the modulo operation work easier
    // so we can add the commas back using one if check
    for(int j=1; j<=i.size(); j++){
      if(j%4 != 0){
        data += i[j-1] + ",";
      }else{
        data += i[j-1] + '\n';
      }
    }
  }
  data += std::to_string(counter) + " rows\n";
  return os << data;
};
