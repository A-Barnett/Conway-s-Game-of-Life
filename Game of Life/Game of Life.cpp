#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <chrono>
#include <thread>
#include <ctime>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <mutex>

#include "Shape.cpp"
#include "Grid.cpp"
#include "SaveData.cpp"

using namespace std;

vector<bool> grid1, grid2;
vector<bool>* currentGrid;
vector<bool>* nextGrid;
pair<int, int>* gridSize;
vector<shape> shapes;


void printGrid() {
    Grid<bool> grid(gridSize->first, gridSize->second);
    grid.gridData = *currentGrid; 
    string output= ". ";
    for (int col = 0; col < gridSize->first; ++col) {
        output += '.';
        output += ' ';
    }
    output += '\n';
    for (int row = 0; row < gridSize->second; ++row) {
        output += '.';
        for (int col = 0; col < gridSize->first; ++col) {
            bool cell = grid(col, row, gridSize->first);
            output += cell ? 'O' : ' ';
            output += '.';
        }
        output += '\n';
    }
    system("cls");
    cout << output;
}



bool checkSamePoints(int p, shape* s) {
    if (!s->dynamic) {
        for (int k = 0; k < s->height; k++) {
            for (int l = 0; l < s->width; l++) {
                if (!(*currentGrid)[p + l + (gridSize->first * k)] == s->positions[l + (s->width * k)]) {
                    return false;
                }
            }
        }
    }
    else {

        for (int k = 0; k < s->dynamicWidthHeight[s->dynamicLoopCount].second; k++) {
            for (int l = 0; l < s->dynamicWidthHeight[s->dynamicLoopCount].first; l++) {
                if (!(*currentGrid)[p + l + (gridSize->first * k)] == (s->dynamicPoses[s->dynamicLoopCount])[l + (s->dynamicWidthHeight[s->dynamicLoopCount].first * k)]) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool checkShape(shape* s) {
    if (!s->dynamic) {
        for (int j = 0; j <= gridSize->second - s->height; j++) {
            for (int i = 0; i <= gridSize->first - s->width; i++) {
                if (checkSamePoints(i + (j * gridSize->first), s)) {
                    return true;
                }
            }
        }
    }
    else {
        bool found = false;
        for (int j = 0; j <= gridSize->second - s->height; j++) {
            for (int i = 0; i <= gridSize->first - s->width; i++) {
                if (checkSamePoints(i + (j * gridSize->first), s)) {
                    found = true;
                    break;
                }
            }
        }

        if (found == false) {
            s->dynamicLoopCount = 0;
            s->found = false;
        }
        else {
            s->dynamicLoopCount++;
        }
        if (s->dynamicLoopCount >= s->dynamicPoses.size()) {
            s->found = true;
            s->dynamicLoopCount = 0;
            return true;
        }
        else if (s->found) {
            return true;
        }
    }

    return false;
}

pair<int, int>* enterGridSize() {
    cout << "Welcome to Conway's Game of Life!" << endl;
    string input;
    cout << "Please input a grid size (e.g. 10x10): ";
    cin >> input;
    size_t sepPos = input.find_first_of("xX*");
    int first = stoi(input.substr(0, sepPos));
    int second = stoi(input.substr(sepPos + 1));

    return new pair<int, int>{ first, second };
}

void takeInput() {
    cout << "Enter grid cell you would like to change (e.g. 5,7) or 'exit': " << endl;
    string cellInput;

    while (true) {
        cin >> cellInput;

        if (cellInput == "exit") {
            break;
        }

        size_t commaPos = cellInput.find(',');
        if (commaPos == string::npos) {
            cout << "Invalid input. Enter grid cell you would like to change (e.g. 5,7) or 'exit'" << endl;
            continue;
        }

        string rowPart = cellInput.substr(0, commaPos);
        string colPart = cellInput.substr(commaPos + 1);

        if (!isdigit(rowPart[0]) || !isdigit(colPart[0])) {
            cout << "Invalid input. Enter grid cell you would like to change (e.g. 5,7) or 'exit'" << endl;
            continue;
        }

        int col = stoi(rowPart);
        int row = stoi(colPart);

        if (row < 0 || row >= gridSize->second || col < 0 || col >= gridSize->first) {
            cout << "Invalid input. Row must be between 0 and " << gridSize->second - 1
                << ", and column must be between 0 and " << gridSize->first - 1 << "." << endl;
            continue;
        }

        (*currentGrid)[row * gridSize->first + col] = !(*currentGrid)[row * gridSize->first + col];

        printGrid();

        cout << "Enter grid cell you would like to change (e.g. 5,7) or 'exit': " << endl;
    }
}

int checkNeighbours(int pos) {
    int width = gridSize->first;
    int height = gridSize->second;
    Grid<bool> grid(width, height);
    grid.gridData = *currentGrid;

    int row = pos / width;
    int col = pos % width;
    int aliveNeighbours = 0;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;

            int neighbourRow = row + i;
            int neighbourCol = col + j;

            if (neighbourRow >= 0 && neighbourRow < height && neighbourCol >= 0 && neighbourCol < width) {
                aliveNeighbours += grid(neighbourCol, neighbourRow, width) ? 1 : 0;
            }
        }
    }

    return aliveNeighbours;
}




int updateGame() {
    int pos = 0;
    int changes = 0;
    vector<int> positions;
    for (bool cell : *currentGrid) {
        int neighbours = checkNeighbours(pos);
        if ((*currentGrid)[pos] == true && (neighbours < 2 || neighbours > 3)) {
            (*nextGrid)[pos] = false;
            changes++;
        }
        else if ((*currentGrid)[pos] == false && neighbours == 3) {
            (*nextGrid)[pos] = true;
            positions.push_back(pos);
            changes++;
        }
        else {
            if ((*currentGrid)[pos] == true) {
                positions.push_back(pos);
            }
            (*nextGrid)[pos] = (*currentGrid)[pos];
        }
        pos++;
    }

    swap(currentGrid, nextGrid);

    return changes;
}

void randomInput(int numPoints,int seed,saveData s,bool hasSave, bool waitForInput) {
    for (int i = 0; i < (*currentGrid).size(); i++) {
        (*currentGrid)[i] = false;
    }
    srand(seed);
    for (int i = 0; i < numPoints; i++) {
        int x = rand() % gridSize->first;
        int y = rand() % gridSize->second;
        int pos = (x * gridSize->second) + y;
        (*currentGrid)[pos] = true;
    }
    if (waitForInput) {
        printGrid();
    }
    if (hasSave) {
        s.printAll();
        cout << "ERN: " << numPoints + gridSize->first + gridSize->second << endl;
    }
    if (waitForInput) {
        cout << "Input any key to start" << endl;
        string temp;
        cin >> temp;
    }

}



shape makeShape(vector<bool> poses, int w, int h, bool dynamic, string name) {
    shape s;
    s.positions = poses;
    s.width = w;
    s.height = h;
    s.dynamic = dynamic;
    s.name = name;
    return s;
}



void flipVector(shape s,bool flipHorizontally, bool flipVertically) {
    vector<bool> flipped(s.width * s.height);
    for (int y = 0; y < s.height; ++y) {
        for (int x = 0; x < s.width; ++x) {
            int newX = flipHorizontally ? s.width - 1 - x : x;     
            int newY = flipVertically ? s.height - 1 - y : y;
            flipped[(newY * s.width) + newX] = s.positions[y * s.width + x];
        }
    }
    shape flip = makeShape(flipped, s.width, s.height, s.dynamic, s.name);
    if (s.dynamic) {
        flip.makeDynamic();
    }
    shapes.push_back(flip);

}





void makeShapes() {
    shapes.clear();

    vector<bool> squarePoses =
    { 0,0,0,0,
     0,1,1,0,
     0,1,1,0,
     0,0,0,0 };
    shape square = makeShape(squarePoses, 4, 4, false, "Square");
    shapes.push_back(square);

    vector<bool> beePoses =
    { 0,0,0,0,0,0,
     0,0,1,1,0,0,
     0,1,0,0,1,0,
     0,0,1,1,0,0,
     0,0,0,0,0,0 };
    shape bee = makeShape(beePoses, 6, 5, false, "Bee-hive");
    shapes.push_back(bee);

    vector<bool> blinkerPos =
    { 0,0,0,
     0,1,0,
     0,1,0,
     0,1,0,
     0,0,0 };
    shape blinker = makeShape(blinkerPos, 3, 5, true, "Blinker");
    blinker.makeDynamic();
    shapes.push_back(blinker);

    vector<bool> toadPos =
    { 0,0,0,0,0,0,
     0,0,1,1,1,0,
     0,1,1,1,0,0,
     0,0,0,0,0,0 };
    shape toad = makeShape(toadPos, 6, 4, true, "Toad");
    flipVector(toad, true, false);
    toad.makeDynamic();
    shapes.push_back(toad);

   vector<bool> gliderPos =
    { 0,0,0,0,0,
     0,0,0,1,0,
     0,1,0,1,0,
     0,0,1,1,0,
     0,0,0,0,0 };
    shape glider = makeShape(gliderPos, 5, 5, true, "Glider");
    flipVector(glider, true, false);
    flipVector(glider, false, true);
    flipVector(glider, true, true);
    glider.makeDynamic();
    shapes.push_back(glider);

    vector<bool> LWSSPos =
    {0,0,0,0,0,0,0,
     0,0,1,1,1,1,0,
     0,1,0,0,0,1,0,
     0,0,0,0,0,1,0,
     0,1,0,0,1,0,0,
     0,0,0,0,0,0,0 };
    shape LWSS = makeShape(LWSSPos, 7, 6, true, "LWSS");
    flipVector(LWSS, true, false);
    LWSS.makeDynamic();
    shapes.push_back(LWSS);
}



// Threading
std::mutex printMutex;

void checkShapeThreaded(shape* s) {
    bool found = checkShape(s);

    std::lock_guard<std::mutex> lock(printMutex); 
    if (found) {
        std::cout << "Shape " << s->name << " found" << std::endl;
    }
}

void printShapeChecks() {
    if (std::thread::hardware_concurrency() >= shapes.size()) {
        std::vector<std::thread> threads;

        for (int i = 0; i < shapes.size(); i++) {
            threads.push_back(std::thread(checkShapeThreaded, &shapes[i]));
        }
        for (auto& th : threads) {
            th.join();
        }
    }
    else {
        for (int i = 0; i < shapes.size(); i++) {
            if (checkShape(&shapes[i])) {
                cout << "Shape " << shapes[i].name << " found" << endl;
            }
        }
    }

}

pair<bool,string> fileShapesCheck(int shapeChoice,bool ern) {
    if (ern) {
        switch (shapeChoice)
        {
        case 1:
            if (checkShape(&shapes[0])) {
                return{ true, shapes[0].name };
            }
            break;
        case 2:
            if (checkShape(&shapes[1])) {
                return{ true, shapes[1].name };
            }
            break;
        case 3:
            if (checkShape(&shapes[2])) {
                return{ true, shapes[2].name };
            }
            break;
        case 4:
            if (checkShape(&shapes[3])) {
                return{ true, shapes[3].name };
            }else if (checkShape(&shapes[4])) {
                return{ true, shapes[4].name };
            }
            break;
        case 5:
            if (checkShape(&shapes[5])) {
                return{ true, shapes[5].name };
            }
            else if (checkShape(&shapes[6])) {
                return{ true, shapes[6].name };
            }
            else if (checkShape(&shapes[7])) {
                return{ true, shapes[7].name };
            }
            else if (checkShape(&shapes[8])) {
                return{ true, shapes[8].name };
            }
            break;
        case 6:
            if (checkShape(&shapes[9])) {
                cout << "FOUND GLIDER 1" << endl;
                return{ true, shapes[9].name };
            }
            else if (checkShape(&shapes[10])) {
                cout << "FOUND GLIDER 2" << endl;
                return{ true, shapes[10].name };
            }
            break;
        }
    }
    else {


        switch (shapeChoice)
        {
        case 1:
            if (checkShape(&shapes[0])) {
                return{ true, shapes[0].name };
            }
            else if (checkShape(&shapes[1])) {
                return{ true, shapes[1].name };
            }
            break;
        case 2:
            if (checkShape(&shapes[2])) {
                return{ true, shapes[2].name };
            }
            else if (checkShape(&shapes[3])) {
                return{ true, shapes[3].name };
            }
            else if (checkShape(&shapes[4])) {
                return{ true, shapes[4].name };
            }
            break;
        case 3:
            bool found = false;
            int foundAt = 0;
            for (int i = 5; i < shapes.size(); i++) {
                if (checkShape(&shapes[i])) {
                    found = true;
                    foundAt = i;
                }
            }
            return{ found, shapes[foundAt].name };
            break;
        }
    }
    return {false,"MD"};
}


void runSimulation(int choiceInt, int loops) {
    if (choiceInt == 5) {
        return;
    }
    using namespace chrono;
    using namespace this_thread;
    auto start = high_resolution_clock::now();
    bool stopped = false;
    for (int i = 0; i < loops; i++) {

        int changes = updateGame();
        if (choiceInt == 1 || choiceInt == 3 || choiceInt == 5) {
            printGrid();
            printShapeChecks();
           // std::this_thread::sleep_for(nanoseconds(10000000));
        }
        else {
            printShapeChecks();
        }
    }
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    if (!stopped) {
        printGrid();
    }
    std::cout << "Elapsed time: " << elapsed.count() << " seconds." << std::endl;

}

void saveRun(int randomInt, int numInputs, int foundAt, int attempts) {
    cout << "Would you like to save the simulation? (y/n)" << endl;
    string saveChoice;
    cin >> saveChoice;
    if (saveChoice == "y" || saveChoice == "Y") {
        saveData newSave;
        newSave.seed = randomInt;
        newSave.attempts = attempts;
        newSave.numberInputs = numInputs;
        newSave.foundAt = foundAt;
        newSave.gridSize = *gridSize;
        cout << "Input Name: " << endl;
        string fileName;
        cin >> fileName;
        newSave.saveFile(fileName);
        cout << "File saved" << endl;
    }
}

void foundRandomShape(int seed, int attempts,int steps, int numInputs,string name,bool ern) {
    saveData temp;
    randomInput(numInputs, seed, temp, false, false);
    printGrid();
    cout << "\nFound shape "<<name << " in " << attempts+1 << " attempts" << endl;
    if (ern) {
        cout << "ERN: " << (gridSize->first + gridSize->second + numInputs) << endl;
    }
    cout << "Input any key to start" << endl;
    string temp2;
    cin >> temp2;
    using namespace chrono;
    using namespace this_thread;
    for (int i = 0; i < steps; i++) {
        int changes = updateGame();
        printGrid();
       // std::this_thread::sleep_for(nanoseconds(15000));
    }
    saveRun(seed, numInputs,steps,attempts+1);


}

pair<int,string> randomSearchUpdate(int steps, int shapeChoice, bool ern) {
    int foundCount = 0;
    for (int i = 0; i < steps; i++) {
        int changes = updateGame();
        if (changes == 0 && ((ern && shapeChoice>2)||(!ern && shapeChoice!=1))) {
            return { -1,"NP" };
        }
        pair <bool, string> check = fileShapesCheck(shapeChoice,ern);
        if (check.first) {
            foundCount++;
            if (foundCount >= 3) {
                return { i,check.second };
            }
        }
        else {
            foundCount = 0;
        }
    }
    return {-1,"NP"};
}


void randomSearch(int shapeChoice,int steps,int numPoints,int maxAttempts, bool ern) {
    int initialNum = numPoints;
    bool found = false;
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        int randomInt = time(NULL);
        saveData temp;
        randomInput(numPoints, randomInt, temp, false,false);
        pair<int,string> stepTaken = randomSearchUpdate(steps, shapeChoice,ern);
        if(stepTaken.first !=-1){
            found = true;
            int a = ern ? ((numPoints-initialNum)*maxAttempts)+attempt : attempt;
            foundRandomShape(randomInt, a,stepTaken.first, numPoints, stepTaken.second,ern);
            break;
        }
        if (attempt == maxAttempts - 1 && ern) {
            numPoints++;
            cout << "Attempting: " << numPoints << endl;
            attempt = 0;
        }
    }
    if (!found) {
        cout << "Could not find Shape in " << maxAttempts << " attempts";
    }
}


void searchFor(int steps) {
    cout << "Run ERN experiment? (y/n)" << endl;
    string ernChoice;
    cin >> ernChoice;
    if (ernChoice == "y" || ernChoice == "Y"){
        cout << "Which shape would you like to search for?" << endl;
        cout << "1: Block\n2: Beehive\n3: Blinker\n4: Toad\n5: Glider\n6: LWSS" << endl;
        int lookingFor;
        cin >> lookingFor;
        cout << "Input initial number of random points" << endl;
        string numPointsString;
        cin >> numPointsString;
        cout << "Max attempts before increasing points" << endl;
        string maxAttemptsString;
        cin >> maxAttemptsString;
        int numInputs = stoi(numPointsString);
        int maxAttempts = stoi(maxAttemptsString);
        randomSearch(lookingFor, steps, numInputs, maxAttempts, true);
    }
    else {
        cout << "Which shape would you like to search for?" << endl;
        cout << "1: Block or Beehive\n2: Blinker or Toad\n3: Glider or LWSS" << endl;
        int lookingFor;
        cin >> lookingFor;
        cout << "Input number of random points" << endl;
        string numPointsString;
        cin >> numPointsString;
        cout << "Max attempts" << endl;
        string maxAttemptsString;
        cin >> maxAttemptsString;
        int numInputs = stoi(numPointsString);
        int maxAttempts = stoi(maxAttemptsString);
        randomSearch(lookingFor,steps, numInputs,maxAttempts,false);
    }
}





int main() {
    makeShapes();
    cout << "Would you like to display from file? (y/n)" << endl;
    string fromFile;
    cin >> fromFile;
    int choiceInt;
    int loops;
    int randomInt;
    int numInputs;

    if (fromFile == "y" || fromFile == "Y"){
        cout << "Please enter file name from SaveData directory" << endl;
        string filename;
        cin >> filename;
        saveData pullSave;
        pullSave.pullFile(filename);
        cout << "Opened file " << filename << endl;;
        gridSize = &pullSave.gridSize;
        grid1.resize(gridSize->first * gridSize->second, false);
        grid2.resize(gridSize->first * gridSize->second, false);
        currentGrid = &grid1;
        nextGrid = &grid2;
        randomInput(pullSave.numberInputs, pullSave.seed,pullSave,true,true);
        loops = pullSave.foundAt;
        choiceInt = 1;
    }
    else {
        gridSize = enterGridSize();
        grid1.resize(gridSize->first * gridSize->second, false);
        grid2.resize(gridSize->first * gridSize->second, false);
        currentGrid = &grid1;
        nextGrid = &grid2;
        printGrid();
        cout << "Select Simulation Type: \n1: Choose points and print\n2: Choose points and don't print\n3: Random points and print\n4: Random points and don't print\n5: Search for Shape" << endl;
        string choice;
        string steps;
        cin >> choice;
        choiceInt = stoi(choice);
        string inputQuestion = choiceInt == 5 ? "Input number of steps per run before restart" : "Input number of steps";
        cout <<  inputQuestion<< endl;
        cin >> steps;
        loops = stoi(steps);
        randomInt = time(NULL);
        if (choiceInt == 1 || choiceInt == 2) {
            takeInput();
        }
        else if (choiceInt == 3 || choiceInt == 4) {
            cout << "Input number of random points" << endl;
            string numPointsString;
            cin >> numPointsString;
            numInputs = stoi(numPointsString);
            saveData temp;
            randomInput(numInputs, randomInt, temp,false,true);
        }
        else if (choiceInt == 5) {
            searchFor(loops);
        }
    }
    runSimulation(choiceInt,loops);
    if (choiceInt == 3 || choiceInt == 4) {
        saveRun(randomInt, numInputs,0,0);
    }
 

    return 0;
}