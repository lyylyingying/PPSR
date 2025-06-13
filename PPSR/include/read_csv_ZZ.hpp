#ifndef READ_CSV_ZZ_HPP
#define READ_CSV_ZZ_HPP

#include "config.h"

struct Point_ZZ{
    string name;
    vector<ZZ> coordinates;
    int cluster_id = -1;
};

map<int, std::vector<Point_ZZ>> read_csv_and_group_by_cluster(const std::string& filename);
void extend_points(std::map<int, std::vector<Point_ZZ>>& clustered_points, int l);

#endif