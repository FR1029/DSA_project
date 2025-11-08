#include "phase1-utils.hpp"

void Graph::dijkstra_dist(const unsigned int& source_id, 
    std::unordered_map<unsigned int, double>& dist, 
    std::unordered_map<unsigned int, unsigned int>& parent,
    unsigned int target_id = -1)
{   
    for (auto &n : Node_from_id) {
        dist[n.first] = std::numeric_limits<double>::infinity();
    }
    dist[source_id] = 0.0;
    std::priority_queue<std::pair<double, unsigned int>, std::vector<std::pair<double, unsigned int>>, std::greater<std::pair<double, unsigned int>>> pq;
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
}

void Graph::dijkstra_time(const unsigned int& source_id,
    std::unordered_map<unsigned int, double>& dist,
    std::unordered_map<unsigned int, unsigned int>& parent,
    unsigned int target_id = -1) 
{
    for (auto &n : Node_from_id) dist[n.first] = std::numeric_limits<double>::infinity();
    dist[source_id] = 0.0;
    std::priority_queue<std::pair<double, unsigned int>, std::vector<std::pair<double, unsigned int>>, std::greater<std::pair<double, unsigned int>>> pq;
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

std::pair<double, std::vector<unsigned int>> Graph::shortest_path(const nlohmann::json &j_query){
    unsigned int source_id = j_query["source"].get<unsigned int>();
    unsigned int target_id = j_query["target"].get<unsigned int>();

    if (!Node_from_id.count(source_id) || !Node_from_id.count(target_id)) {
        throw std::out_of_range("Invalid source or target ID");
    }
    // We have to make a vector for forbidden road types and forbidden_nodes
    if (source_id == target_id) return {0.0, {source_id}};
    std::string mode = j_query["mode"].get<std::string>();
    Node* source = Node_from_id[source_id];
    Node* target = Node_from_id[target_id];
    std::unordered_map<unsigned int, unsigned int> parent;
    std::priority_queue<std::pair<double, unsigned int>, std::vector<std::pair<double, unsigned int>>, std::greater<>> pq;
    std::unordered_map<unsigned int, double> dist;
    if(mode=="distance") {
        dijkstra_dist(source_id, dist, parent, target_id);
    } else if (mode == "time") {
        dijkstra_time(source_id, dist, parent, target_id);
    } else {
        return {};
    }
    if (dist[target_id] == std::numeric_limits<double>::infinity()) return {std::numeric_limits<double>::infinity(), {}};
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
