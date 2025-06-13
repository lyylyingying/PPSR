#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <Eigen/Dense>

// data point structure
struct DataPoint {
    std::string name;                // name
    Eigen::VectorXd features;       // vector of features
};

// Read CSV file
std::vector<DataPoint> read_csv(const std::string& filename) {
    std::vector<DataPoint> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::getline(ss, name, ','); // Read the first column as name

        std::vector<double> feature_values;
        std::string value;
        while (std::getline(ss, value, ',')) {
            feature_values.push_back(std::stod(value));
        }

        Eigen::VectorXd features = Eigen::VectorXd::Map(feature_values.data(), feature_values.size());
        data.push_back({name, features});
    }

    return data;
}

// Write data to CSV file
void write_csv(const std::string& filename, const std::vector<DataPoint>& data) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    for (const auto& point : data) {
        file << point.name;
        for (int i = 0; i < point.features.size(); ++i) {
            file << "," << point.features[i];
        }
        file << "\n";
    }
}

// PCA dimensionality reduction
std::vector<DataPoint> apply_pca(const std::vector<DataPoint>& data, int target_dim) {
    if (data.empty()) {
        throw std::runtime_error("Input data is empty.");
    }

    // Construct data matrix
    int num_samples = data.size();
    int original_dim = data[0].features.size();
    Eigen::MatrixXd matrix(num_samples, original_dim);

    for (int i = 0; i < num_samples; ++i) {
        matrix.row(i) = data[i].features;
    }

    // Compute mean and center the data
    Eigen::VectorXd mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();

    // Compute covariance matrix
    Eigen::MatrixXd covariance = (matrix.transpose() * matrix) / (num_samples - 1);

    // Perform eigenvalue decomposition on the covariance matrix
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(covariance);
    Eigen::MatrixXd eigen_vectors = solver.eigenvectors().rightCols(target_dim);

    // Project to low-dimensional space
    Eigen::MatrixXd reduced_data = matrix * eigen_vectors;

    // Construct reduced data points
    std::vector<DataPoint> reduced_points;
    for (int i = 0; i < num_samples; ++i) {
        reduced_points.push_back({data[i].name, reduced_data.row(i)});
    }

    return reduced_points;
}

int main() {
    try {
        const std::string input_filename = "../../CNN/50x80/50x40_features_512.csv";  // Input CSV file
        const std::string output_filename = "../../testfile/PCA/reduced_features_2000_256.csv"; // Output CSV file
        const int target_dim = 256;  // Target dimension for reduction

        // Read CSV file
        std::vector<DataPoint> data = read_csv(input_filename);

        // Apply PCA dimensionality reduction
        std::vector<DataPoint> reduced_data = apply_pca(data, target_dim);

        // Write reduced data to CSV
        write_csv(output_filename, reduced_data);

        std::cout << "PCA dimensionality reduction completed. Results written to " << output_filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}