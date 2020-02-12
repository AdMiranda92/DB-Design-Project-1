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
    while (std::getline(inputFile, line, ',')){
        std::istringstream iss(line);
        std::string a, b, c, d;
        if (!(iss >> a >> b >> c >> d)) { break; } // end of file
            newTable->tableContents.push_back(vector<std::string> {a, b, c, d});
    }

    // table should now have a vector of string vectors holding all rows
    // the first row holds the table attributes
    // the second row holds the types of the attributes
    // all other rows contain information from the table
    return newTable;

};

ostream& operator<<(ostream& os, const Table& table){
  string row;
  for(vector<string> i: table.tableContents){
    // we begin at one here to make the modulo operation work easier
    // so we can add the commas back using one if check
    for(int j=1; j<=i.size(); j++){
      if(j%4 != 0){
        row += i[j-1] + ",";
      }else{
        row += i[j-1] + '\n';
      }
    }
  }
  return os << row;
};
