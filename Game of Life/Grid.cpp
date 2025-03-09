#include<iostream>
#include<vector>

using namespace std;


// Template
template<typename T>
class Grid {
public:
    vector<T> gridData;
    Grid(int width, int height) : gridData(width* height) {}


    // Operator Overloading
    T& operator()(int x, int y, int width) {
        return gridData[y * width + x];
    }
};


template<>
class Grid<bool> { 
public:
    vector<bool> gridData;
    Grid(int width, int height) : gridData(width* height) {}

    bool operator()(int x, int y, int width) const { // Polymorphic Function Override
        return gridData[y * width + x];
    }

    void set(int x, int y, int width, bool value) {
        gridData[y * width + x] = value;
    }

    void printGrid(int width,int height) {

    }
};