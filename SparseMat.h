#include <iostream>

struct SparseMat {
    int numRows;
    int numCols;
    int numVals;

    std::vector<int> rowIndices;
    std::vector<int> colFirstIndices;
    std::vector<double> vals;

    SparseMat() {
        numRows = 0;
        numCols = 0;
        numVals = 0;
    }

    SparseMat(int newNumRows, int newNumCols, int numNewValues) {
        numRows = newNumRows;
        numCols = newNumCols;
        numVals = numNewValues;

        rowIndices.reserve(numVals);
        vals.reserve(numVals);

        for (int i = 0; i<numCols; i++) {
            colFirstIndices.push_back(INT_MAX);
        }
        colFirstIndices.push_back(numVals);
    }

    std::vector<double> multiply(std::vector<double> x) {
        std::vector<double> y;
        for (int i=0; i<numRows; i++) {
            y.push_back(0.0);
        }
        for (int i=0; i<numCols; i++) {
            for (int j=colFirstIndices[i]; j<colFirstIndices[i+1]; j++) {
                y[rowIndices[j]] += vals[j]*x[i];
            }
        }
        return y;
    }

    std::vector<double> multiplyInverse(std::vector<double> x) {
        std::vector<double> y(numCols);
        for (int i=0; i<numCols; i++) {
            y[i] = 0.0;
            for (int j=colFirstIndices[i]; j<colFirstIndices[i+1]; j++) {
                y[i] += vals[j]*x[rowIndices[j]];
            }
        }
        return y;
    }
};