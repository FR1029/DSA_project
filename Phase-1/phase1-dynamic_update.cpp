#include "phase1-utils.hpp"

bool Graph::remove_edge(unsigned int _id) {
    if(Edge_from_id.count(_id)) {
        if(Edge_from_id[_id]->forbidden) return false;

        Edge_from_id[_id]->forbidden = true;
        return true;
    }

    return false;
    // else {
    //     throw std::out_of_range("Edge with this id not present");
    // }
}

bool Graph::modify_edge(unsigned int _id, const nlohmann::json &j_query) {
    if(Edge_from_id.count(_id)) {
        Edge* e = Edge_from_id[_id];
        e->forbidden = false;
        if(!j_query["patch"].empty()) {
            for(auto& p : j_query["patch"]) {
                if(p.contains("length")) {
                    e->length = p["length"].get<double>();
                }
                if(p.contains("average_time")) {
                    e->avg_time = p["average_time"].get<double>();
                }
                if(p.contains("speed_profile")) {
                    unsigned i = 0;
                    for(auto& sp: p["speed_profile"]) {
                        e->speed_profile[i++] = (sp.get<double>());
                    }
                }
                if (p.contains("road_type")) {
                    e->type = p["road_type"].get<std::string>();
                }
            }
            return true;
        }
        else {
            return false;
        }
    }
    
    return false;
    // else {
    //     throw std::out_of_range("Edge with this id not present");
    // }
}