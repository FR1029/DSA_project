#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>

class Node {

public:
    unsigned int id;
    double lat;
    double lon;
    std::unordered_map<std::string, bool> pois;
    bool restricted;

    Node(unsigned int _id, double _lat, double _lon);

    void update_restriction(bool _restricted);

    void update_pois(std::string place, bool val);
};

class Edge {

public:
    unsigned int id;
    Node* from;
    Node* to;
    double length;
    bool directed;
    bool forbidden = false;
    double time;
    std::string type;

    Edge(unsigned int _id, Node* from, Node* to, std::string type);

    void update_restriction(bool _forbidden);

    void update_length(double _len);

    void update_type(std::string _type);
};

class Graph {

public:
    // int N; // No. of vertices
    std::vector<Edge*> edge_list;
    std::vector<Node*> nodes; // is it needed?
    std::unordered_map<Node*, std::vector<Node*>> adj_list;

    // void CreateNode(unsigned int _id, double _lat, double )
    

};