#include "phase1-utils.hpp"

Node::Node(unsigned int _id, double _lat, double _lon) {
    id = _id;
    lat = _lat;
    lon = _lon;
}

void Node::update_restriction(bool _restricted) {
    restricted = _restricted;
}

void Node::update_pois(std::string place, bool val) {
    pois[place] = val;
}

Edge::Edge(unsigned int _id, Node* _from, Node* _to, double _len, std::string type, double _avg_time, bool _one_way) {
    id = _id;
    from = _from;
    to = _to;
    length = _len;
    avg_time = _avg_time;
    directed = _one_way;
}

void Edge::update_restriction(bool _forbidden) {
    forbidden = _forbidden;
}

void Edge::update_length(double _len) {
    if(_len > 0) {
        length = _len;
    }
    else {
        throw std::invalid_argument("Length should be positive");
    }
}

void Edge::update_type(std::string _type) {
    type = _type;
}

void Graph::json_load(const nlohmann::json& jsn) {
    for(auto& n : jsn["nodes"]) {
        unsigned int _id = n["id"].get<unsigned int>();
        double _lat = n["lat"];
        double _lon = n["lon"];

        Node* node = new Node(_id, _lat, _lon);

        if(n.contains("pois")) {
            for (auto& place : n["pois"]) {
                node->pois[place.get<std::string>()] = true;
            }
        }

        // nodes.push_back(node);
        Node_from_id[_id] = node;
    }

    for(auto& e : jsn["edges"]) {
        unsigned int _id = e["id"].get<unsigned int>();
        Node* _from;
        Node* _to;
        double _len = e["length"];
        double _avg_time = e["average_time"];

        if(!Node_from_id[e["u"]]) {
            throw std::out_of_range("Node does not exist");
        }
        else {
            _from = Node_from_id[e["u"]];
        }

        if(!Node_from_id[e["v"]]) {
            throw std::out_of_range("Node does not exist");
        }
        else {
            _to = Node_from_id[e["v"]];
        }

        std::string r_type = e["road_type"].get<std::string>();

        bool one_way = e["oneway"];

        Edge* edg = new Edge(_id, _from, _to, _len, r_type, _avg_time, one_way);

        if(one_way) {
            adj_list[_from].push_back(_to);
        }
        else {
            adj_list[_from].push_back(_to);
            adj_list[_to].push_back(_from);
        }

        if(e.contains("speed_profile")) {
            for(auto& sp: e["speed_profile"]) {
                edg->speed_profile.push_back(sp.get<double>());
            }
        }

        Edge_from_id[_id] = edg;
        // edge_list.push_back(edg);
    }
}