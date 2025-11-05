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
    bool restricted = false;

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
    double avg_time;
    std::string type;
    std::vector<double> speed_profile;

    Edge(unsigned int _id, Node* from, Node* to, double _len, std::string type, double _avg_time, bool _one_way = false);

    void update_restriction(bool _forbidden);

    void update_length(double _len);

    void update_type(std::string _type);
};

class Graph {

public:
    // std::vector<Edge*> edge_list;
    // std::vector<Node*> nodes; // is it needed?
    std::unordered_map<Node*, std::vector<Node*>> adj_list;
    std::unordered_map<unsigned int, Node*> Node_from_id;
    std::unordered_map<unsigned int, Edge*> Edge_from_id;

    // void CreateNode(unsigned int _id, double _lat, double )
    
    void json_load(const nlohmann::json &j);

};