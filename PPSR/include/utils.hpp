#ifndef UTILS_HPP
#define UTILS_HPP

#include "config.h"
#include "read_csv_ZZ.hpp"
#include "she.hpp"

struct DataPoint {
    std::string name;                // Name of the data point
    std::vector<double> coordinates; // 512-dimensional vector
};

string generate_random_string(size_t length);
ZZ random_ZZ(int min=0, int max=10);
vector<ZZ> generate_random_vector(int l);
vector<ZZ> expand_vector(const vector<ZZ>& a, ZZ gamma);
ZZ calculate_inner_product(const Point_ZZ& p1, const Point_ZZ& p2);
vector<pair<ZZ, size_t>> compute_inner_products(
    const vector<Point_ZZ>& centroids_ZZ,
    const Point_ZZ& query_point_ZZ_median_enc,
    const ZZ& encryption_of_negative_one,
    int k1,
    SHE she
);

vector<pair<ZZ, string>> compute_inner_products_for_cluster(
    const map<int, vector<Point_ZZ>>& clustered_points,
    const vector<size_t>& cluster_ids,
    const Point_ZZ& query_point_ZZ_leaf_enc,
    const ZZ& encryption_of_negative_one,
    int k1,
    SHE she
);

// Read data from a CSV file
std::vector<DataPoint> read_csv_for_recall(const std::string& filename);

// Compute the Euclidean distance between two data points
double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b);

// Retrieve the names of points within a threshold distance from the query point
std::vector<std::string> find_within_threshold(
    const std::vector<DataPoint>& data,
    const DataPoint& query,
    double threshold
);

// Compute the intersection ratio between two vectors
double calculate_intersection_ratio(
    const std::vector<std::string>& vec1,
    const std::vector<std::string>& vec2
);

vector<Point_ZZ> readAndProcessCSV(const string &filename);

#endif