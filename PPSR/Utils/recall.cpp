#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

struct DataPoint {
    std::string name;                // name
    std::vector<double> coordinates; // vector of 512 size
};

// Read data from CSV file
std::vector<DataPoint> read_csv(const std::string& filename) {
    std::vector<DataPoint> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the file.");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::getline(ss, name, ','); // Read the first column as name

        std::vector<double> coordinates;
        std::string value;
        while (std::getline(ss, value, ',')) {
            coordinates.push_back(std::stod(value)); // Convert to double
        }

        if (coordinates.size() != 512) {
            throw std::runtime_error("Invalid vector dimension: must be 512.");
        }

        data.push_back({name, coordinates});
    }

    return data;
}

// Compute the Euclidean distance between two data points
double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Dimension mismatch between vectors.");
    }

    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

int main() {
    try {
        const std::string input_filename = "../../CNN/50x80/50x40_features_512.csv"; // Input CSV file name
        const std::string output_filename = "../../testfile/recall/result.txt"; // Output result file name
        const double threshold = 8.0; // Euclidean distance threshold

        // Read CSV file
        std::vector<DataPoint> data = read_csv(input_filename);
        if (data.empty()) {
            std::cerr << "No data found in the file." << std::endl;
            return 1;
        }

        // Select the first data point as the query
        const DataPoint& query = data[3];

        // Record results
        std::vector<std::string> result_names;
        for (const auto& point : data) {
            double distance = euclidean_distance(query.coordinates, point.coordinates);
            if (distance < threshold) {
                result_names.push_back(point.name);
            }
        }

        // Output results count
        std::cout << "Number of points within threshold: " << result_names.size() << std::endl;

        // Write results to file
        std::ofstream output_file(output_filename);
        if (!output_file.is_open()) {
            throw std::runtime_error("Failed to open the output file.");
        }

        for (const auto& name : result_names) {
            output_file << name << "\n";
        }

        std::cout << "Results written to " << output_filename << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
