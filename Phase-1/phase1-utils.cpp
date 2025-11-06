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
        double _lat = n["lat"].get<double>();
        double _lon = n["lon"].get<double>();

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
        double _len = e["length"].get<double>();
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

        adj_list[_from].push_back({_to, _id});

        if(!one_way) {
            adj_list[_to].push_back({_from, _id});
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

void Graph::remove_edge(unsigned int _id) {
    if(Edge_from_id.count(_id)) {
        Edge_from_id[_id]->forbidden = true;
    }

    // else {
    //     throw std::out_of_range("Edge with this id not present");
    // }
}

void Graph::modify_edge(unsigned int _id, const nlohmann::json &j_query) {
    if(Edge_from_id.count(_id)) {
        Edge* e = Edge_from_id[_id];
        e->forbidden = false;
        for(auto& p : j_query["patch"]) {
            if(p.contains("length")) {
                e->length = p["length"].get<double>();
            }
            if(p.contains("average_time")) {
                e->avg_time = p["average_time"].get<double>();
            }
            if(p.contains("speed_profile")) {
                int i = 0;
                for(auto& sp: p["speed_profile"]) {
                    e->speed_profile[i++] = (sp.get<double>());
                }
            }
            if (p.contains("road_type")) {
                e->type = p["road_type"].get<std::string>();
            }
        }
    }
    // else {
    //     throw std::out_of_range("Edge with this id not present");
    // }
}

std::pair<double, std::vector<unsigned int>> Graph::shortest_path(nlohmann::json &j_query){
    unsigned int source_id = j_query["source"].get<unsigned int>();
    unsigned int target_id = j_query["target"].get<unsigned int>();

    if (!Node_from_id.count(source_id) || !Node_from_id.count(target_id)) {
        throw std::out_of_range("Invalid source or target ID");
    }

    std::string mode = j_query["mode"].get<std::string>();
    Node* source = Node_from_id[source_id];
    Node* target = Node_from_id[target_id];
    std::unordered_map<unsigned int, unsigned int> parent;
    std::priority_queue<std::pair<double, unsigned int>, std::vector<std::pair<double, unsigned int>>, std::greater<>> pq;
    std::unordered_map<unsigned int, double> dist;
    if(mode=="distance") {
        for (auto &n : Node_from_id) {
            dist[n.first] = std::numeric_limits<double>::infinity();
        }
        dist[source_id] = 0.0;
        pq.push({0.0, source_id});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            if (u == target_id) break;
            for (auto &[v, edge_id] : adj_list[Node_from_id[u]]) {
                Edge* e = Edge_from_id[edge_id];
                if (e->forbidden || v->restricted) {
                    continue;
                }
                double w = e->length;
                double tempdist = dist[u] + w;
                if (tempdist < dist[v->id]) {
                    dist[v->id] = tempdist;
                    parent[v->id] = u;
                    pq.push({tempdist, v->id});
                }
            }
        }
    } else if (mode == "time") {
        for (auto &n : Node_from_id) dist[n.first] = std::numeric_limits<double>::infinity();
        dist[source_id] = 0.0;
        pq.push({0.0, source_id});
        while (!pq.empty()) {
            auto [t, u] = pq.top(); pq.pop();
            if (t > dist[u]) continue;
            if (u == target_id) break;
            for (auto &[v, edge_id] : adj_list[Node_from_id[u]]) {
                Edge* e = Edge_from_id[edge_id];
                if (e->forbidden || v->restricted) continue;
                double travel_time = 0.0;
                if (!e->speed_profile.empty()) {
                    double remaining = e->length;
                    double curr_time = t;
                    while (remaining > 0) {
                        int idx = (int)(curr_time / 15) % 96;
                        double speed = e->speed_profile[idx];
                        double time_left = 15.0 - fmod(curr_time, 15.0);
                        double dist_possible = (speed * time_left) / 60.0;
                        if (remaining <= dist_possible) {
                            travel_time += (remaining / speed) * 60.0;
                            remaining = 0;
                        } else {
                            travel_time += time_left;
                            curr_time += time_left;
                            remaining -= dist_possible;
                        }
                    }
                } else {
                    travel_time = e->avg_time;
                }
                double new_time = t + travel_time;
                if (new_time < dist[v->id]) {
                    dist[v->id] = new_time;
                    parent[v->id] = u;
                    pq.push({new_time, v->id});
                }
            }
        }
    }
    if (!parent.count(target_id)) {
        return {std::numeric_limits<double>::infinity(), {}};
    }
    std::vector<unsigned int> path;
    for (unsigned int v = target_id; v != source_id; v = parent[v]){
        path.push_back(v);
    }
    path.push_back(source_id);
    std::reverse(path.begin(), path.end());
    return {dist[target_id], path};
}