#include "table.h"
#include <fstream>

Table* Table::readTableFromCSV(const string& file){
    std::ifstream inputFile;
    inputFile.open(file);
    if(!inputFile.is_open()){
        return nullptr;
    }
    std::string name = file.substr(0, file.find('.')); 

    //Create the table and give it a name
    Table table(name);

};