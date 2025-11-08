#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <array>

class Node {

public:
    unsigned int id;
    double lat;
    double lon;
    std::unordered_map<std::string, bool> pois;
    bool restricted = false;

    Node(unsigned int _id);
    Node(unsigned int _id, double _lat, double _lon);
    
    // void update_restriction(bool _restricted);

    // void update_pois(std::string place, bool val);
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
    // std::vector<double> speed_profile;
    std::array<double, 96> speed_profile;

    Edge(unsigned int _id, Node* from, Node* to, double _len, std::string type, double _avg_time, bool _one_way = false);

    // void update_restriction(bool _forbidden);

    // void update_length(double _len);

    // void update_type(std::string _type);
};

class Graph {

public:
    // std::vector<Edge*> edge_list;
    std::unordered_map<Node*, std::vector<std::pair<Node*, unsigned int>>> adj_list;
    std::unordered_map<unsigned int, Node*> Node_from_id;
    std::unordered_map<unsigned int, Edge*> Edge_from_id;
    std::vector<Node*> nodes; // for knn euclidean
    
    void graph_load_json(const nlohmann::json& jsn);
    nlohmann::json process_query(const nlohmann::json& _query);

    // Phase 1 Queries
    bool remove_edge(unsigned int _id);

    bool modify_edge(unsigned int _id, const nlohmann::json &j_query);

    std::pair<double, std::vector<unsigned int>> shortest_path(const nlohmann::json &j_query);
    
    std::vector<unsigned int> knn_euclidean(const nlohmann::json &j_query);
    std::vector<unsigned int> knn_shortest_path(const nlohmann::json &j_query);

    void dijkstra_dist(const unsigned int& source_id, std::unordered_map<unsigned int, double>& dist, std::unordered_map<unsigned int, unsigned int>& parent, unsigned int target_id = -1);
    void dijkstra_time(const unsigned int& source_id, std::unordered_map<unsigned int, double>& dist, std::unordered_map<unsigned int, unsigned int>& parent, unsigned int target_id = -1);
};

double euclidean_distance(double lat1, double lat2, double lon1, double lon2);