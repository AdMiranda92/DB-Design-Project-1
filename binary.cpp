#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

int binaryFunc(){
    vector<string> array = {"Hello", "World", "This", "Is", "A", "Test", "of", "A", "Binary", "System"};
    vector<string> array2 = {"One", "More", "TEST"};
    ofstream out;
    out.open("test.dat", ios::out | ios::binary);

    out.write((char*)&array[0], array.size() * sizeof(string));
    out.write((char*)&array2[0], array2.size() * sizeof(string));

    out.close();
    vector<string> inArray(10); 
    vector<string> inArray2(3); 

    ifstream in;
    in.open("test.dat", ios::in | ios::binary);
    in.read((char*)&inArray[0], inArray.size() * sizeof(string));
    in.read((char*)&inArray2[0], inArray2.size() * sizeof(string));

    in.close();

    std::cout << inArray[0] << " ";
    std::cout << inArray[1] << " ";

    std::cout << inArray2[0] << " ";
    std::cout << inArray2[1] << " ";
    return 0;
}