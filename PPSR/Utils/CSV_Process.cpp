#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

struct DataRow {
    string name;          // name
    vector<double> values; // values
};

// read csv file
vector<DataRow> readCSV(const string& filename) {
    vector<DataRow> data;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string name;
        string value;
        DataRow row;

        // Get name (first column)
        getline(ss, name, ',');
        row.name = name;

        // Get vector data (remaining columns)
        while (getline(ss, value, ',')) {
            row.values.push_back(stod(value));
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

// Compute the square sum of each row's vector
double computeSquareSum(const vector<double>& values) {
    double sum = 0.0;
    for (double value : values) {
        sum += value * value;
    }
    return sum;
}

int main() {
    string filename = "../../CNN/50x80/50x80_features_512.csv"; // Input CSV file name
    vector<DataRow> data = readCSV(filename);

    string maxName;
    double maxSquareSum = -numeric_limits<double>::infinity();

    // Process data
    for (auto& row : data) {
        // Multiply each data point by 100
        for (double& value : row.values) {
            value *= 10000;
        }

        // Compute square sum
        double squareSum = computeSquareSum(row.values);

        // Update maximum square sum and corresponding name
        if (squareSum > maxSquareSum) {
            maxSquareSum = squareSum;
            maxName = row.name;
        }
    }

    // Output results
    cout << "Data with the maximum square sum:" << endl;
    cout << "Name: " << maxName << endl;
    cout << "Maximum square sum: " << maxSquareSum << endl;

    return 0;
}
