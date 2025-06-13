#ifndef K_MEANS_HPP
#define K_MEANS_HPP

#include "config.h"

// Define a struct to represent a point
struct Point {
    std::string name;                 // Name of the vector
    std::vector<double> coordinates; // Coordinates of the vector
    int cluster_id;                   // Record which cluster the point belongs to
};

// Compute the Euclidean distance between two points
double euclidean_distance(const Point& p1, const Point& p2);

// Compute the centroid (mean) of a cluster
Point compute_centroid(const std::vector<Point>& cluster, const std::string& name);

// K-Means algorithm
void kmeans(std::vector<Point>& points, int k, int max_iters=100);

// Read data from a CSV file
std::vector<Point> read_csv(const std::string& filename);

// Write clustering results to a CSV file
void write_csv(const std::string& filename, std::vector<Point>& points);
// void write_clusters(const std::string& filename, const std::unordered_map<Point, std::vector<Point>>& clusters_with_centroids);

// Get the vectors for each cluster centroid
std::vector<Point> get_centroids(const std::vector<Point>& points, int k);

// Compute the centroid (mean) of all cluster centroids
Point compute_overall_centroid(const vector<Point>& centroids);

#endif