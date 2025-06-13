#include "config.h"
#include "she.hpp"
#include "aes.h"
#include "kmeans.hpp"
#include "HMACH.hpp"
#include "read_csv_ZZ.hpp"
#include "utils.hpp"

/*
Only two main functions are used in implementation
main_tree() is used for constructing a tree
main_no_tree() is used for scanning linearly
*/

// Transform the coordinates of query_point_ZZ
void transform_point(Point_ZZ& query_point_ZZ, ZZ tau_leaf, int l, ZZ gamma) {
    // Step 1: Compute the sum of squares and subtract tau_leaf's square
    ZZ sum_of_squares = to_ZZ(0);
    for (const auto& coord : query_point_ZZ.coordinates) {
        sum_of_squares += coord * coord;
    }

    ZZ tau_leaf_square = tau_leaf * tau_leaf;
    ZZ first_new_element = sum_of_squares - tau_leaf_square;

    // Step 2: Multiply each element in the coordinates by -2
    for (auto& coord : query_point_ZZ.coordinates) {
        coord *= to_ZZ(-2);
    }

    // Step 3: Add two new elements
    query_point_ZZ.coordinates.push_back(first_new_element);
    query_point_ZZ.coordinates.push_back(to_ZZ(1));

    if (l > 0)
    {
        // Step 4: Generate a random l-dimensional vector a
        vector<ZZ> a = generate_random_vector(l);

        // Step 5: Expand a to get a'
        vector<ZZ> a_prime = expand_vector(a, gamma);

        // Step 6: Append a' to the end of query_point_ZZ's coordinates
        query_point_ZZ.coordinates.insert(query_point_ZZ.coordinates.end(), a_prime.begin(), a_prime.end());
    }
    
}

// Transform the coordinates of the median node
void transform_point_median(Point_ZZ& query_point_ZZ_median, ZZ tau_median)
{
    // Step 1: Compute the sum of squares and subtract tau_median's square
    ZZ sum_of_squares = to_ZZ(0);
    for (const auto& coord : query_point_ZZ_median.coordinates) {
        sum_of_squares += coord * coord;
    }

    ZZ tau_median_square = tau_median * tau_median;
    ZZ first_new_element = sum_of_squares - tau_median_square;

    // Step 2: Multiply each element in the coordinates by -2
    for (auto& coord : query_point_ZZ_median.coordinates) {
        coord *= to_ZZ(-2);
    }

    // Step 3: Add two new elements
    query_point_ZZ_median.coordinates.push_back(first_new_element);
    query_point_ZZ_median.coordinates.push_back(to_ZZ(1));
}

void print_point(Point& point)
{
    cout << "name: " << point.name << "  " << "cluster id: " << point.cluster_id << endl;
    for (const auto& val : point.coordinates)
    {
        cout << val << " ";
    }
    cout << endl;
}

void print_point_ZZ(Point_ZZ& point)
{
    cout << "name: " << point.name << "  " << "cluster id: " << point.cluster_id << endl;
    for (const auto& val : point.coordinates)
    {
        cout << val << " ";
    }
    cout << endl;
}

int main()
{
    /*System Initialization*/
    //Initialization of SHE
    auto start_time = std::chrono::high_resolution_clock::now();
    //TODO: Modify Parameters
    int k0 = 1024;
    int k1 = 50;
    int k2 = 160;
    SHE she = SHE(k0, k1, k2);
    ZZ encryption_of_negative_one = she.encrypt(conv<ZZ>("-1"));

    //Initialization of HMAC
    string key_MAC = "secretkey";
    HMACH hmac(key_MAC);

    //Initialization of AES
    vector<unsigned char> key_AES(32); // AES-256 requires a 256-bit key
    vector<unsigned char> iv_AES(16);  // AES block size is 16 bytes
    AES aes= AES();

    if (RAND_bytes(key_AES.data(), key_AES.size()) != 1 || RAND_bytes(iv_AES.data(), iv_AES.size()) != 1) {
        aes.handleErrors();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "System Initialization time: " << duration.count()/1000.0 << " ms" << std::endl;

    /*Data Outsourcing*/
    auto start_time1 = std::chrono::high_resolution_clock::now();
    //Cluster
    // string input_file = "../../CNN/50x80/50x40_features_512.csv";  // Input CSV file name
    string input_file = "../../testfile/PCA/reduced_features_4000_64.csv";  // Input CSV file name
    string output_file = "../../testfile/clustered_data.csv";  // Output CSV file name
    //TODO: Modify Parameters
    int k = 5;  // Number of clusters

    // Read data from CSV file
    vector<Point> points = read_csv(input_file);
    cout << "Loaded " << points.size() << " points from " << input_file << endl;

    // Randomly select a vector as the query vector
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, points.size() - 1);
    int random_index = dis(gen);
    //TODO:
    int query_index = 3457;
    Point query_point = points[query_index];

    // Perform K-Means Clustering
    kmeans(points, k);
    auto end_time1 = std::chrono::high_resolution_clock::now();

    // Write clustering results to output file
    write_csv(output_file, points);
    cout << "Clustered data saved to " << output_file << endl;

    // Get and output cluster centroids (C)
    auto start_time2 = std::chrono::high_resolution_clock::now();
    vector<Point> centroids = get_centroids(points, k);

    // Compute and output the centroid of all cluster centroids (r)
    Point overall_centroid = compute_overall_centroid(centroids);
    auto end_time2 = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time2 - start_time2 + end_time1 - start_time1);
    std::cout << "K-Means Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    //Extension
    //TODO: Modify Parameters
    start_time = std::chrono::high_resolution_clock::now();
    int l_size_of_vector = 0;   // Length of the added random vector
    auto clustered_points = read_csv_and_group_by_cluster(output_file); //Already of ZZ type, expanded 10000 times
    extend_points(clustered_points, l_size_of_vector);

    //Encryption f
    for (auto& cluster : clustered_points)
    {
        for(auto& point : cluster.second)
        {
            for (auto& coord : point.coordinates)
            {
                coord = she.encrypt(coord);
            }
        }
    }

    //Encryption median
    vector<Point_ZZ> centroids_ZZ;
    for (const auto& centroid : centroids)
    {
        Point_ZZ centroid_ZZ;
        centroid_ZZ.name = centroid.name;
        centroid_ZZ.cluster_id = centroid.cluster_id;
        ZZ sum_of_squares = to_ZZ(0);
        // Convert double coordinates to ZZ type and encrypt
        for (const auto& coord : centroid.coordinates) {
            ZZ coord_ZZ = to_ZZ(coord * 10000);     // Multiply by 10000 to convert to integer
            sum_of_squares += coord_ZZ * coord_ZZ;
            centroid_ZZ.coordinates.push_back(she.encrypt(coord_ZZ));
        }

        // Add encrypted 1
        centroid_ZZ.coordinates.push_back(she.encrypt(to_ZZ(1)));
        // Add encrypted sum of squares
        centroid_ZZ.coordinates.push_back(she.encrypt(sum_of_squares));
        centroids_ZZ.push_back(centroid_ZZ);
    }

    //Encryption root
    Point_ZZ overall_centroid_ZZ;
    overall_centroid_ZZ.name = overall_centroid.name;
    overall_centroid_ZZ.cluster_id = overall_centroid.cluster_id;
    ZZ sum_of_squares = to_ZZ(0);
    for (const auto& coord : overall_centroid.coordinates) {
        ZZ coord_ZZ = to_ZZ(coord * 10000);     // Multiply by 10000 to convert to integer
        sum_of_squares += coord_ZZ * coord_ZZ;
        overall_centroid_ZZ.coordinates.push_back(she.encrypt(coord_ZZ));
    }
    // Add encrypted 1
    overall_centroid_ZZ.coordinates.push_back(she.encrypt(to_ZZ(1)));
    // Add encrypted sum of squares
    overall_centroid_ZZ.coordinates.push_back(she.encrypt(sum_of_squares));
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Encryption Index Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    //Encryption images
    //TODO: Modify Parameters
    start_time = std::chrono::high_resolution_clock::now();
    int string_length = 3072;       // Size of the encrypted image, this number represents how many characters
    int size_of_images = 2000;      // Number of encrypted images
    string random_str = generate_random_string(string_length);
    for (size_t i = 0; i < size_of_images; i++)
    {
        std::vector<unsigned char> ciphertext = aes.encrypt(random_str, key_AES, iv_AES);
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Encryption images Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    //generate MAC values
    start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size_of_images; i++)
    {
        string mac_random = hmac.compute_mac(random_str);
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "MAC images Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    /*Query Token Generation*/
    start_time = std::chrono::high_resolution_clock::now();
    //TODO: Modify Parameters
    ZZ tau_leaf = conv<ZZ>("700000");    // Leaf node threshold
    ZZ tau_med = conv<ZZ>("800000");        // Intermediate node threshold must be greater than leaf node threshold
    ZZ gamma = conv<ZZ>("710000");       // Gamma must be greater than leaf node threshold

    Point_ZZ query_point_ZZ_leaf;
    query_point_ZZ_leaf.name = query_point.name;
    query_point_ZZ_leaf.cluster_id = query_point.cluster_id;
    Point_ZZ query_point_ZZ_median;
    query_point_ZZ_median.name = query_point.name;
    query_point_ZZ_median.cluster_id = query_point.cluster_id;
    for(const auto& coord : query_point.coordinates)
    {
        ZZ coord_ZZ = to_ZZ(coord * 10000);
        query_point_ZZ_leaf.coordinates.push_back(coord_ZZ);
        query_point_ZZ_median.coordinates.push_back(coord_ZZ);
    }

    transform_point(query_point_ZZ_leaf, tau_leaf, l_size_of_vector, gamma);
    transform_point_median(query_point_ZZ_median, tau_med);

    //Encrypt leaf and median
    Point_ZZ query_point_ZZ_leaf_enc;
    Point_ZZ query_point_ZZ_median_enc;
    query_point_ZZ_leaf_enc.name = query_point_ZZ_leaf.name;
    query_point_ZZ_leaf_enc.cluster_id = query_point_ZZ_leaf.cluster_id;
    for (const auto& coord : query_point_ZZ_leaf.coordinates)
    {
        query_point_ZZ_leaf_enc.coordinates.push_back(she.encrypt(coord));
    }
    
    query_point_ZZ_median_enc.name = query_point_ZZ_median.name;
    query_point_ZZ_median_enc.cluster_id = query_point_ZZ_median.cluster_id;
    for (const auto& coord : query_point_ZZ_median.coordinates)
    {
        query_point_ZZ_median_enc.coordinates.push_back(she.encrypt(coord));
    }
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Query Token Generation Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    /*Query Processing*/
    start_time = std::chrono::high_resolution_clock::now();
    //CS1 computing the inner product
    vector<pair<ZZ, size_t>> inner_products_median = compute_inner_products(centroids_ZZ, query_point_ZZ_median_enc, encryption_of_negative_one, k1, she);
    //CS2 decrypts the inner products
    vector<size_t> cluster_id;
    for (const auto& [inner_product, index] : inner_products_median)
    {
        ZZ decrypt_inner_product = she.decrypt(inner_product);
        // cout << "decrypt_inner_product: " << decrypt_inner_product << endl;
        if (decrypt_inner_product < 0)
        {
            cluster_id.push_back(index);
        }  
    }

    //CS1 computes the inner product between leaf and image
    vector<pair<ZZ, string>> inner_products_leaf = compute_inner_products_for_cluster(clustered_points, cluster_id, query_point_ZZ_leaf_enc, encryption_of_negative_one, k1, she);
    //CS2 decrypts the inner products
    vector<string> search_results;
    for (const auto& [inner_product, name] : inner_products_leaf)
    {
        ZZ decrypt_inner_product = she.decrypt(inner_product);
        // cout << "decrypt_inner_product: " << decrypt_inner_product << endl;
        if (decrypt_inner_product < 0)
        {
            search_results.push_back(name);
        }  
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Query Processing Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    cout << "Construction result size: " << search_results.size() << endl;




    /*Test recall*/
    //TODO:
    const double threshold = 70.0;      // Threshold
    std::vector<DataPoint> data = read_csv_for_recall(input_file);
    if (data.empty()) {
        cerr << "No data found in the file." << std::endl;
        return 1;
    }

     // Select the first data point as the query
    const DataPoint& query = data[query_index];

    // Retrieve data points that meet the criteria
    vector<std::string> result_names = find_within_threshold(data, query, threshold);

    // Output the number of results
    cout << "Number of points within threshold: " << result_names.size() << std::endl;

    double intersection_ratio = calculate_intersection_ratio(search_results, result_names);
    cout << "Intersection ratio: " << intersection_ratio << "%" << std::endl;

    return 0;
}

int main_no_tree()
{
    /*System Initialization*/
    //Initialization of SHE
    auto start_time = std::chrono::high_resolution_clock::now();
    //TODO: Modify Parameters
    int k0 = 1024;
    int k1 = 50;
    int k2 = 160;
    SHE she = SHE(k0, k1, k2);
    ZZ encryption_of_negative_one = she.encrypt(conv<ZZ>("-1"));

    //Initialization of HMAC
    string key_MAC = "secretkey";
    HMACH hmac(key_MAC);

    //Initialization of AES
    vector<unsigned char> key_AES(32); // AES-256 requires a 256-bit key
    vector<unsigned char> iv_AES(16);  // AES block size is 16 bytes
    AES aes= AES();

    if (RAND_bytes(key_AES.data(), key_AES.size()) != 1 || RAND_bytes(iv_AES.data(), iv_AES.size()) != 1) {
        aes.handleErrors();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "System Initialization time: " << duration.count()/1000.0 << " ms" << std::endl;


    /*Data Outsourcing*/
    start_time = std::chrono::high_resolution_clock::now();

    string input_file = "../../testfile/PCA/reduced_features_4000_256.csv";  // Input CSV file name
    string output_file = "../../testfile/clustered_data.csv";  // Output CSV file name

    // Read CSV file and convert data to ZZ type after multiplying by 10000
    vector<Point_ZZ> points = readAndProcessCSV(input_file);

    // Select query vector
    int query_index = 3457;
    Point_ZZ query_point = points[query_index];

    // Encrypt index
    for(Point_ZZ point : points)
    {
        for(auto& coord : point.coordinates)
        {
            coord = she.encrypt(coord);
        }
    }

    // Encryption images
    // TODO: Modify Parameters
    int string_length = 3072;       // Size of the encrypted image, this number represents how many characters
    int size_of_images = 4000;      // Number of encrypted images
    string random_str = generate_random_string(string_length);
    for (size_t i = 0; i < size_of_images; i++)
    {
        std::vector<unsigned char> ciphertext = aes.encrypt(random_str, key_AES, iv_AES);
    }

    // Generate MAC values
    start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < size_of_images; i++)
    {
        string mac_random = hmac.compute_mac(random_str);
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Encrypted Index time: " << duration.count()/1000.0 << " ms" << std::endl;

    /*Query Token Generation*/
    start_time = std::chrono::high_resolution_clock::now();
    //TODO: Modify Parameters
    ZZ tau_leaf = conv<ZZ>("700000");    // Leaf node threshold
    ZZ tau_med = conv<ZZ>("950000");        // Intermediate node threshold must be greater than leaf node threshold
    ZZ gamma = conv<ZZ>("710000");       // Gamma must be greater than leaf node threshold

    query_point.coordinates.pop_back();
    query_point.coordinates.pop_back();
    transform_point(query_point, tau_leaf, 0, gamma);

    // Encrypt leaf and median
    Point_ZZ query_point_ZZ_leaf_enc;
    query_point_ZZ_leaf_enc.name = query_point.name;
    query_point_ZZ_leaf_enc.cluster_id = query_point.cluster_id;
    for (const auto& coord : query_point.coordinates)
    {
        query_point_ZZ_leaf_enc.coordinates.push_back(she.encrypt(coord));
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Query Token Generation Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    /*Query Processing*/
    start_time = std::chrono::high_resolution_clock::now();

    vector<pair<ZZ, string>> inner_products_leaf;
    int count = 0;
    for (Point_ZZ point : points)
    {
        count++;
        ZZ inner_product = calculate_inner_product(query_point_ZZ_leaf_enc, point);
        // Generate random r1 and r2
        ZZ r1, r2;
        r1 = to_ZZ(10);
        r2 = to_ZZ(5);
        // Transform inner product result
        ZZ transformed_inner_product = she.moduloN(inner_product * r1 + r2 * encryption_of_negative_one);

        // Save result and point name
        inner_products_leaf.emplace_back(transformed_inner_product, point.name);
    }
    cout << "calculate numbers: " << count << endl;

    //CS2 decrypts the inner products
    vector<string> search_results;
    for (const auto& [inner_product, name] : inner_products_leaf)
    {
        ZZ decrypt_inner_product = she.decrypt(inner_product);
        // cout << "decrypt_inner_product: " << decrypt_inner_product << endl;
        if (decrypt_inner_product < 0)
        {
            search_results.push_back(name);
        }  
    }

    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Query Processing Execution time: " << duration.count()/1000.0 << " ms" << std::endl;

    cout << "Construction result size: " << search_results.size() << endl;
    


    /*Test recall*/
    //TODO:
    const double threshold = 70.0;      // Threshold
    std::vector<DataPoint> data = read_csv_for_recall(input_file);
    if (data.empty()) {
        cerr << "No data found in the file." << std::endl;
        return 1;
    }

     // Select the first data point as the query
    const DataPoint& query = data[query_index];

    // Retrieve data points that meet the criteria
    vector<std::string> result_names = find_within_threshold(data, query, threshold);

    // Output the number of results
    cout << "Number of points within threshold: " << result_names.size() << std::endl;

    double intersection_ratio = calculate_intersection_ratio(search_results, result_names);
    cout << "Intersection ratio: " << intersection_ratio << "%" << std::endl;

    return 0;
}