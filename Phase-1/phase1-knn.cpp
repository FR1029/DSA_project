#include "phase1-utils.hpp"

std::vector<unsigned int> Graph::knn_euclidean(const nlohmann::json &j_query) {
    std::string poi = j_query["poi"].get<std::string>();
    double q_lat = j_query["query_point"]["lat"].get<double>();
    double q_lon = j_query["query_point"]["lon"].get<double>();
    unsigned int k = j_query["k"].get<unsigned int>();
    std::vector<unsigned int> ans;

    // Precomputation for faster sorting //

    if(k <= nodes.size()) {
        std::partial_sort(nodes.begin(), nodes.begin()+k, nodes.end(), [&](Node* a, Node* b) {
            double temp_a = euclidean_distance(a->lat, q_lat, a->lon, q_lon);
            double temp_b = euclidean_distance(b->lat, q_lat, b->lon, q_lon);
            bool x1 = a->pois[poi];
            bool x2 = b->pois[poi];

            if(x1 != x2) {
                return x1 > x2;
            }
            return temp_a < temp_b;
        });

        for(unsigned int i = 0; i<k; i++) {
            if(nodes[i]->pois[poi]) ans.push_back(nodes[i]->id);
        }
    }
    else {
        std::sort(nodes.begin(), nodes.end(), [&] (Node* a, Node* b) {
            double temp_a = euclidean_distance(a->lat, q_lat, a->lon, q_lon);
            double temp_b = euclidean_distance(b->lat, q_lat, b->lon, q_lon);
            bool x1 = a->pois[poi];
            bool x2 = b->pois[poi];

            if(x1 != x2) {
                return x1 > x2;
            }
            return temp_a > temp_b;
        });

        for(unsigned int i = 0; i<nodes.size(); i++) {
            if(nodes[i]->pois[poi]) ans.push_back(nodes[i]->id);
        }
    }

    return ans;
}

std::vector<unsigned int> Graph::knn_shortest_path(const nlohmann::json &j_query) {
    std::string poi = j_query["poi"].get<std::string>();
    double q_lat = j_query["query_point"]["lat"].get<double>();
    double q_lon = j_query["query_point"]["lon"].get<double>();
    unsigned int k = j_query["k"].get<unsigned int>();
    std::vector<unsigned int> ans;
    unsigned int nearest_node_id = 0;
    double nearest_dist = std::numeric_limits<double>::infinity();
    for (auto &n : Node_from_id) {
        Node* node = n.second;
        double d = euclidean_distance(node->lat, q_lat, node->lon, q_lon);
        if (d < nearest_dist) {
            nearest_node_id = n.first;
            nearest_dist = d;
        }
    }
    std::unordered_map<unsigned int, double> dist;
    std::unordered_map<unsigned int, unsigned int> parent;
    dijkstra_dist(nearest_node_id, dist, parent);
    std::vector<std::pair<double, unsigned int>> poi_nodes;
    poi_nodes.reserve(Node_from_id.size());
    for (auto& [id, node] : Node_from_id) {
        if (node->pois.count(poi) && node->pois.at(poi)) {
            double d = dist[id];
            if (d < std::numeric_limits<double>::infinity()) {
                poi_nodes.emplace_back(d, id);
            }
        }
    }
    if (poi_nodes.size() <= k) {
        std::sort(poi_nodes.begin(), poi_nodes.end(),
                  [](auto &a, auto &b) { return a.first < b.first; });
    } else {
        std::partial_sort(poi_nodes.begin(), poi_nodes.begin() + k, poi_nodes.end(),
                          [](auto &a, auto &b) { return a.first < b.first; });
        poi_nodes.resize(k);
    }
    ans.reserve(k);
    for (auto &p : poi_nodes)
        ans.push_back(p.second);
    return ans;
}