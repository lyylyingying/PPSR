#include "kmeans.hpp"

// Calculate Euclidean distance
double euclidean_distance(const Point& p1, const Point& p2) {
    double dist = 0.0;
    for (size_t i = 0; i < p1.coordinates.size(); ++i) {
        dist += pow(p1.coordinates[i] - p2.coordinates[i], 2);
    }
    return sqrt(dist);
}

// Compute the centroid (mean) of a cluster
Point compute_centroid(const std::vector<Point>& cluster, const std::string& name) {
    size_t dim = cluster[0].coordinates.size();
    std::vector<double> centroid_coordinates(dim, 0.0);

    for (const auto& point : cluster) {
        for (size_t i = 0; i < dim; ++i) {
            centroid_coordinates[i] += point.coordinates[i];
        }
    }

    for (size_t i = 0; i < dim; ++i) {
        centroid_coordinates[i] /= cluster.size();
    }

    return {name, centroid_coordinates, -1};
}

void kmeans(std::vector<Point>& points, int k, int max_iters) {
    srand(time(0));
    std::vector<Point> centroids;

    // Randomly initialize k cluster centroids
    for (int i = 0; i < k; ++i) {
        centroids.push_back(points[rand() % points.size()]);
        centroids[i].name = "cluster-" + std::to_string(i);
    }

    for (int iter = 0; iter < max_iters; ++iter) {
        // Assign each point to the nearest cluster centroid
        for (auto& point : points) {
            double min_dist = std::numeric_limits<double>::max();
            int closest_centroid = -1;
            for (int i = 0; i < k; ++i) {
                double dist = euclidean_distance(point, centroids[i]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = i;
                }
            }
            point.cluster_id = closest_centroid;
        }

        // Update each cluster's centroid
        std::vector<std::vector<Point>> clusters(k);
        for (const auto& point : points) {
            clusters[point.cluster_id].push_back(point);
        }

        bool converged = true;
        for (int i = 0; i < k; ++i) {
            if (!clusters[i].empty()) {
                Point new_centroid = compute_centroid(clusters[i], "cluster-" + std::to_string(i));
                if (euclidean_distance(centroids[i], new_centroid) > 1e-4) {
                    centroids[i] = new_centroid;
                    converged = false;
                }
            }
        }

        if (converged) {
            std::cout << "Converged in " << iter + 1 << " iterations." << std::endl;
            break;
        }
    }
}

std::vector<Point> read_csv(const std::string& filename) {
    std::vector<Point> points;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::getline(ss, name, ',');

        std::vector<double> coordinates;
        std::string value;
        while (std::getline(ss, value, ',')) {
            coordinates.push_back(std::stod(value));
        }

        points.push_back({name, coordinates, -1});
    }

    return points;
}

void write_csv(const std::string& filename, std::vector<Point>& points) {
    // Sort by cluster ID
    std::vector<Point> sorted_points = points;
    std::sort(sorted_points.begin(), sorted_points.end(), [](const Point& a, const Point& b) {
        return a.cluster_id < b.cluster_id;
    });

    std::ofstream file(filename);

    for (const auto& point : sorted_points) {
        file << point.name << ",";
        for (size_t i = 0; i < point.coordinates.size(); ++i) {
            file << point.coordinates[i];
            if (i < point.coordinates.size() - 1) file << ",";
        }
        file << "," << point.cluster_id << std::endl; // Append cluster ID
    }
}

std::vector<Point> get_centroids(const std::vector<Point>& points, int k) {
    std::vector<Point> centroids;
    std::vector<std::vector<Point>> clusters(k);

    // Assign points to clusters
    for (const auto& point : points) {
        clusters[point.cluster_id].push_back(point);
    }

    // Compute the centroid for each cluster
    for (int i = 0; i < k; ++i) {
        if (!clusters[i].empty()) {
            Point centroid = compute_centroid(clusters[i], "cluster-" + std::to_string(i));
            centroids.push_back(centroid);
        }
    }

    return centroids;
}

// Compute the centroid (mean) of all cluster centroids
Point compute_overall_centroid(const vector<Point>& centroids) {
    size_t dim = centroids[0].coordinates.size();
    vector<double> overall_centroid_coordinates(dim, 0.0);

    // Compute the mean of all cluster centroids' coordinates
    for (const auto& centroid : centroids) {
        for (size_t i = 0; i < dim; ++i) {
            overall_centroid_coordinates[i] += centroid.coordinates[i];
        }
    }

    // Normalize
    for (size_t i = 0; i < dim; ++i) {
        overall_centroid_coordinates[i] /= centroids.size();
    }

    return {"overall_centroid", overall_centroid_coordinates, -1};
}