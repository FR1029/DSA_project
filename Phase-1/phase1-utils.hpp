#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <utility>
#include <algorithm>

class Node {

public:
    unsigned long long id;
    double lat;
    double lon;
    std::vector<std::string> pois;
    bool restricted;

    Node(unsigned long long _id, double _lat, double _lon);

    void update_restriction(bool _restricted);

    void update_pois(std::string place);

};

class Graph {

};