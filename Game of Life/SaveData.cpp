#include<iostream>
#include<string>
#include<fstream>
using namespace std;

class saveData {
public:
    int seed;
    int attempts;
    int foundAt;
    int numberInputs;
    pair<int, int> gridSize;

    void printAll() {
        cout << "Seed: " + to_string(seed) + " Attempts: " + to_string(attempts) + " Found at: " + to_string(foundAt) + " Number Inputs: " + to_string(numberInputs) + " Grid: " + to_string(gridSize.first)+"x"+ to_string(gridSize.second) << endl;
    }

    void saveFile(string name) {
        string path = "../Game of Life/SaveData/";
        ofstream file1;
        file1.open(path + name+".txt", ios::app);
        file1.write((char*)this, sizeof(*this));
        file1.close();
    }

    void pullFile(string name) {
        string path = "../Game of Life/SaveData/";
        fstream file2;
        file2.open(path + name + ".txt", ios::in);
        file2.seekg(0);
        file2.read((char*)this, sizeof(*this));
        file2.close();
    }
};