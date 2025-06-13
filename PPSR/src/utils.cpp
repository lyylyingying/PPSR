#include "utils.hpp"

string generate_random_string(size_t length) {
    // Returns the binary number of an integer
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    random_device rd;  // Get random seed
    mt19937 gen(rd()); // Initialize random number generator
    uniform_int_distribution<> dis(0, charset.size() - 1); // Uniform distribution

    string random_str;
    for (size_t i = 0; i < length; ++i) {
        random_str += charset[dis(gen)];  // Randomly select character and append
    }

    return random_str;
}

// Generate random number
ZZ random_ZZ(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(min, max);
    return to_ZZ(dis(gen));
}

// Randomly generate an l-dimensional vector a, with elements between 0 and 10
vector<ZZ> generate_random_vector(int l) {
    vector<ZZ> a(l);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, 10);

    for (int i = 0; i < l; ++i) {
        a[i] = to_ZZ(dis(gen));  // Randomly select an integer between 0 and 10
    }

    return a;
}

// Expand vector a to get a'
vector<ZZ> expand_vector(const vector<ZZ>& a, ZZ gamma) {
    vector<ZZ> a_prime;
    int l = a.size();

    // First l elements: square each element and multiply by gamma squared
    for (int i = 0; i < l; ++i) {
        ZZ square = a[i] * a[i];
        a_prime.push_back(square * gamma * gamma);
    }

    // Next l elements: multiply each element by gamma squared
    for (int i = 0; i < l; ++i) {
        a_prime.push_back(a[i] * gamma * gamma);
    }

    // Last element: gamma squared
    a_prime.push_back(gamma * gamma);

    return a_prime;
}

// Inner product function
ZZ calculate_inner_product(const Point_ZZ& p1, const Point_ZZ& p2) {
    // Ensure both vectors have the same dimension
    if (p1.coordinates.size() != p2.coordinates.size()) {
        cerr << "Error: Vectors must have the same dimension for inner product calculation." << endl;
        return ZZ(0);
    }

    // Compute inner product
    ZZ inner_product = ZZ(0);
    for (size_t i = 0; i < p1.coordinates.size(); ++i) {
        inner_product += p1.coordinates[i] * p2.coordinates[i];
    }

    return inner_product;
}

// Compute inner products with all centroids and return results, along with operations with median node
vector<pair<ZZ, size_t>> compute_inner_products(
    const vector<Point_ZZ>& centroids_ZZ,
    const Point_ZZ& query_point_ZZ_median_enc,
    const ZZ& encryption_of_negative_one,
    int k1,
    SHE she
) {
    vector<pair<ZZ, size_t>> inner_products;

    for (size_t i = 0; i < centroids_ZZ.size(); ++i) {
        ZZ inner_product = calculate_inner_product(query_point_ZZ_median_enc, centroids_ZZ[i]);

        //random values r1 and r2
        ZZ r1, r2;
        r1 = to_ZZ(10);
        r2 = to_ZZ(5);
        inner_products.emplace_back(she.moduloN(r1 * inner_product + r2 * encryption_of_negative_one), i); // Save inner product value and index
    }

    return inner_products;
}

// Compute inner products for all points in a specified cluster and save results along with point names
vector<pair<ZZ, string>> compute_inner_products_for_cluster(
    const map<int, vector<Point_ZZ>>& clustered_points,
    const vector<size_t>& cluster_ids,
    const Point_ZZ& query_point_ZZ_leaf_enc,
    const ZZ& encryption_of_negative_one,
    int k1,
    SHE she
) {
    vector<pair<ZZ, string>> results;

    // Iterate through each id in cluster_ids
    for (size_t id : cluster_ids) {
        // Check if id exists in clustered_points
        auto it = clustered_points.find(id);
        if (it == clustered_points.end()) {
            cerr << "Cluster ID " << id << " not found in clustered_points." << endl;
            continue;
        }

        // Iterate through each point in the cluster
        for (const auto& point : it->second) {
            // Compute inner product
            ZZ inner_product = calculate_inner_product(query_point_ZZ_leaf_enc, point);

            // Generate random r1 and r2
            ZZ r1, r2;
            r1 = to_ZZ(10);
            r2 = to_ZZ(5);

            // Transform inner product result
            ZZ transformed_inner_product = she.moduloN(inner_product * r1 + r2 * encryption_of_negative_one);

            // Save result and point name
            results.emplace_back(transformed_inner_product, point.name);
        }
    }

    return results;
}

// Read data from CSV file
std::vector<DataPoint> read_csv_for_recall(const std::string& filename) {
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

        if (coordinates.empty()) {
            throw std::runtime_error("Invalid data: no coordinates.");
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

// Retrieve names of points whose Euclidean distance to the query point is less than the threshold
std::vector<std::string> find_within_threshold(
    const std::vector<DataPoint>& data,
    const DataPoint& query,
    double threshold
) {
    std::vector<std::string> result_names;
    for (const auto& point : data) {
        double distance = euclidean_distance(query.coordinates, point.coordinates);
        if (distance < threshold) {
            result_names.push_back(point.name);
        }
    }
    return result_names;
}

// Compute the intersection ratio of two vectors
double calculate_intersection_ratio(
    const std::vector<std::string>& vec1,
    const std::vector<std::string>& vec2
) {
    std::unordered_set<std::string> set1(vec1.begin(), vec1.end());
    std::unordered_set<std::string> set2(vec2.begin(), vec2.end());

    size_t intersection_count = 0;
    for (const auto& item : set2) {
        if (set1.find(item) != set1.end()) {
            ++intersection_count;
        }
    }

    if (set2.empty()) {
        return 0.0;
    }

    return (static_cast<double>(intersection_count) / set2.size()) * 100.0;
}

// Function to read a CSV file and process the data
vector<Point_ZZ> readAndProcessCSV(const string &filename) {
    vector<Point_ZZ> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return points;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Point_ZZ point;

        // Read the first column (image name)
        getline(ss, token, ',');
        point.name = token;

        // Read the feature vector
        vector<ZZ> coordinates;
        double value;
        while (getline(ss, token, ',')) {
            stringstream value_stream(token);
            value_stream >> value;

            // Convert to ZZ: multiply by 10000 and cast
            ZZ zz_value = to_ZZ(value * 10000);
            coordinates.push_back(zz_value);
        }

        // Calculate sum of squares
        ZZ sum_of_squares = ZZ(0);
        for (const auto &coord : coordinates) {
            sum_of_squares += coord * coord;
        }

        // Add the two additional elements
        coordinates.push_back(ZZ(1));
        coordinates.push_back(sum_of_squares);

        // Store the processed data in the structure
        point.coordinates = coordinates;
        points.push_back(point);
    }

    file.close();
    return points;
}