#include "query.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  string cmd;
  ifstream in;
  cin >> cmd;
  while (cmd != "QUIT")
  {
    if (cmd == "READ")
    {
      cin >> cmd;
      in.open(cmd);
      if (in.is_open())
      {
        in.close();
        Table* t = Table::readTableFromCSV(cmd);
        if (t == nullptr)
          cout << "CSV file " << cmd << " could not be processed" << endl;
        else
        {
          cout << "Success:\n" << *t << endl;
          t->writeToFile();
          delete t;
        }
      }
    }
    else if (cmd == "SELECT")
    {
      string line;
      getline(cin, line);
      cmd += line;
      Query q(cmd);
      if (q.getCondition() == nullptr)
        cout << "Error in parsing query \"" << cmd << '"' << endl;
      Table* t = q.getTable();
      
      if (t == nullptr)
        cout << "Table " << q.getTable() << " does not exist" << endl;
      else
      {
        //cout << "Read table:\n" << *t << endl;
        //cout << "Query:  " << q << endl;
        Table* result = t->runQuery(q);
        cout << *result << endl;
        delete result;
      }
    }
    cin >> cmd;
  }

  return 0;
}