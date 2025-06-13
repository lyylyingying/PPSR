#include "read_csv_ZZ.hpp"

// Read data from a CSV file and group vectors with the same cluster ID together
map<int, std::vector<Point_ZZ>> read_csv_and_group_by_cluster(const std::string& filename) {
    std::map<int, std::vector<Point_ZZ>> clustered_points;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::getline(ss, name, ',');  // Read vector name

        std::vector<ZZ> coordinates;
        std::string value;

        // Read each column's coordinate value and convert it to ZZ type
        while (std::getline(ss, value, ',')) {
            if (ss.peek() == EOF)
            {
                break;
            }
            double val = std::stod(value);  // Convert to double type
            ZZ zz_val = to_ZZ(val * 10000); // Convert double type value to ZZ type after multiplying by 10000
            coordinates.push_back(zz_val);
        }

        // Read cluster ID
        int cluster_id = std::stoi(value);  // Read last column as cluster ID

        // Add point to map
        Point_ZZ point = {name, coordinates, cluster_id};
        clustered_points[cluster_id].push_back(point);
    }

    return clustered_points;
}

void extend_points(std::map<int, std::vector<Point_ZZ>>& clustered_points, int l) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 10); // Generate random integers from 0 to 10
    for (auto& cluster : clustered_points) {
        for (auto& point : cluster.second) {
            // Compute sum of squares
            ZZ sum_of_squares = to_ZZ(0);
            for (const auto& coord : point.coordinates) {
                sum_of_squares += coord * coord;  // Compute sum of squares
            }
            // Add first element: 1
            point.coordinates.push_back(to_ZZ(1));
            // Add second element: sum of squares
            point.coordinates.push_back(sum_of_squares);

            if (l > 0)
            {
                //a
                vector<ZZ> a;
                for (int i = 0; i < l; i++)
                {
                    a.push_back(to_ZZ(dist(gen)));
                }
                //a'
                vector<ZZ> a_prime(a);
                for (const auto& elem : a) {
                    a_prime.push_back(-2 * elem * elem); // Add square multiplied by -2
                }
                // Compute sum of cubes for each element in a
                ZZ cube_sum = to_ZZ(0);
                for (const auto& elem : a) {
                    cube_sum += elem * elem * elem; // Compute sum of cubes
                }
                // Add sum of cubes to the end of a'
                a_prime.push_back(cube_sum);

                // Append each element in a' to point.coordinates
                for (const auto& elem : a_prime) {
                    point.coordinates.push_back(elem);
                }
            }
            
        }
    }
}