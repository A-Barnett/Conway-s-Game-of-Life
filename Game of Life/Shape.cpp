#include<iostream>
#include<vector>

using namespace std;

class BaseShape {
public:
    virtual void printPoses() const = 0;  // Pure virtual function
};


class shape : public BaseShape { // Polymorphic function overriding
public:
    vector<bool> positions;
    int width, height;
    bool dynamic;
    string name;
    vector<vector<bool>> dynamicPoses;
    vector<pair<int, int>> dynamicWidthHeight;
    int dynamicLoopCount;
    bool found;
    bool flippableX;
    bool flippableY;

    // Override
    void printPoses() const override {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                cout << (positions[i * width + j] ? '1' : '0') << " ";
            }
            cout << '\n';
        }
        cout << '\n';
    }

    void printDynamicPoses() const {
        for (int frame = 0; frame < dynamicPoses.size(); frame++) {
            const vector<bool>& currentPose = dynamicPoses[frame];
            int currentWidth = dynamicWidthHeight[frame].first;
            int currentHeight = dynamicWidthHeight[frame].second;

            cout << "Frame " << frame + 1 << " (" << currentWidth << "x" << currentHeight << "):\n";
            for (int i = 0; i < currentHeight; ++i) {
                for (int j = 0; j < currentWidth; ++j) {
                    cout << (currentPose[i * currentWidth + j] ? '1' : '0') << " ";
                }
                cout << '\n';
            }
            cout << '\n';
        }
    }

    // Operator Overloading
    bool operator==(const vector<bool>& otherPositions) const {
        return (this->positions == otherPositions);
    }

    bool operator==(const shape& other) const {
        return (this->width == other.width && this->height == other.height && this->positions == other.positions);
    }




    void makeDynamic() {
        dynamicPoses.push_back(positions);
        dynamicWidthHeight.push_back({ width, height });
        vector<bool> currentPositions = positions;
        int currentWidth = width, currentHeight = height;

        while (true) {
            vector<bool> nextState = currentPositions;
            int pos = 0;

            for (bool cell : currentPositions) {
                int neighbours = checkNeighboursShape(currentPositions, pos, currentWidth, currentHeight);
                if (cell && (neighbours < 2 || neighbours > 3)) {
                    nextState[pos] = false;
                }
                else if (!cell && neighbours == 3) {
                    nextState[pos] = true;
                }
                else {
                    nextState[pos] = cell;
                }
                pos++;
            }

            vector<bool> paddedState;
            int paddedWidth, paddedHeight;
            applyPadding(nextState, currentWidth, currentHeight, paddedState, paddedWidth, paddedHeight);

            if (find(dynamicPoses.begin(), dynamicPoses.end(), paddedState) != dynamicPoses.end()) {
                break;
            }
            else {
                dynamicPoses.push_back(paddedState);
                dynamicWidthHeight.push_back({ paddedWidth, paddedHeight });
            }

            currentPositions = paddedState;
            currentWidth = paddedWidth;
            currentHeight = paddedHeight;
        }

    }


    void applyPadding(const vector<bool>& currentState, int currentWidth, int currentHeight,
        vector<bool>& paddedState, int& paddedWidth, int& paddedHeight) {
        bool addTop = false, addBottom = false, addLeft = false, addRight = false;


        for (int i = 0; i < currentWidth; ++i) {
            if (currentState[i]) {
                addTop = true;
                break;
            }
        }


        for (int i = 0; i < currentWidth; ++i) {
            if (currentState[(currentHeight - 1) * currentWidth + i]) {
                addBottom = true;
                break;
            }
        }

        for (int i = 0; i < currentHeight; ++i) {
            if (currentState[i * currentWidth]) {
                addLeft = true;
                break;
            }
        }


        for (int i = 0; i < currentHeight; ++i) {
            if (currentState[i * currentWidth + (currentWidth - 1)]) {
                addRight = true;
                break;
            }
        }

        paddedWidth = currentWidth + (addLeft ? 1 : 0) + (addRight ? 1 : 0);
        paddedHeight = currentHeight + (addTop ? 1 : 0) + (addBottom ? 1 : 0);
        paddedState.assign(paddedWidth * paddedHeight, false);

        for (int i = 0; i < currentHeight; ++i) {
            for (int j = 0; j < currentWidth; ++j) {
                int newRow = i + (addTop ? 1 : 0);
                int newCol = j + (addLeft ? 1 : 0);
                paddedState[newRow * paddedWidth + newCol] = currentState[i * currentWidth + j];
            }
        }


        removeExtraPadding(paddedState, paddedWidth, paddedHeight);
    }

    void removeExtraPadding(vector<bool>& paddedState, int& paddedWidth, int& paddedHeight) {
        bool done = false;
        while (!done) {
            done = true;

            bool emptyTop = true, emptySecondTop = true;
            for (int j = 0; j < paddedWidth; ++j) {
                if (paddedState[j]) {
                    emptyTop = false;
                    break;
                }
            }
            for (int j = 0; j < paddedWidth; ++j) {
                if (paddedState[paddedWidth + j]) {
                    emptySecondTop = false;
                    break;
                }
            }
            if (emptyTop && emptySecondTop && paddedHeight > 1) {
                paddedState.erase(paddedState.begin(), paddedState.begin() + paddedWidth);
                paddedHeight -= 1;

            }

            bool emptyBottom = true, emptySecondBottom = true;
            for (int j = 0; j < paddedWidth; ++j) {
                if (paddedState[(paddedHeight - 1) * paddedWidth + j]) {
                    emptyBottom = false;
                    break;
                }
            }
            for (int j = 0; j < paddedWidth; ++j) {
                if (paddedState[(paddedHeight - 2) * paddedWidth + j]) {
                    emptySecondBottom = false;
                    break;
                }
            }
            if (emptyBottom && emptySecondBottom && paddedHeight > 1) {
                paddedState.erase(paddedState.end() - paddedWidth, paddedState.end());
                paddedHeight -= 1;

            }



            bool emptyLeft = true, emptySecondLeft = true;
            for (int i = 0; i < paddedHeight; ++i) {
                if (paddedState[i * paddedWidth]) {
                    emptyLeft = false;
                    break;
                }
            }
            for (int i = 0; i < paddedHeight; ++i) {
                if (paddedState[i * paddedWidth + 1]) {
                    emptySecondLeft = false;
                    break;
                }
            }
            vector<bool> temp;
            if (emptyLeft && emptySecondLeft && paddedWidth > 1) {
                for (int i = 0; i < paddedHeight * paddedWidth; ++i) {
                    if (i % paddedWidth != 0) {
                        temp.push_back(paddedState[i]);
                    }

                }
                paddedState = temp;
                paddedWidth -= 1;

            }


            bool emptyRight = true, emptySecondRight = true;
            for (int i = 0; i < paddedHeight; ++i) {
                if (paddedState[i * paddedWidth + (paddedWidth - 1)]) {
                    emptyRight = false;
                    break;
                }
            }

            for (int i = 0; i < paddedHeight; ++i) {
                if (paddedState[i * paddedWidth + (paddedWidth - 2)]) {
                    emptySecondRight = false;
                    break;
                }
            }

            vector<bool> tempR;
            if (emptyRight && emptySecondRight && paddedWidth > 1) {
                for (int i = 0; i < paddedHeight * paddedWidth; ++i) {
                    if (i % paddedWidth != paddedWidth - 1) {
                        tempR.push_back(paddedState[i]);
                    }

                }
                paddedState = tempR;
                paddedWidth -= 1;

            }
        }
    }int checkNeighboursShape(const vector<bool>& state, int pos, int currentWidth, int currentHeight) {
        int row = pos / currentWidth;
        int col = pos % currentWidth;
        int aliveNeighbours = 0;

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;

                int neighbourRow = row + i;
                int neighbourCol = col + j;

                if (neighbourRow >= 0 && neighbourRow < currentHeight && neighbourCol >= 0 && neighbourCol < currentWidth) {
                    aliveNeighbours += state[neighbourRow * currentWidth + neighbourCol];
                }
            }
        }
        return aliveNeighbours;
    }
};