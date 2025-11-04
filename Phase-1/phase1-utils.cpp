#include "phase1-utils.hpp"

Node::Node(unsigned long long _id, double _lat, double _lon) {
    id = _id;
    lat = _lat;
    lon = _lon;
    };

void Node::update_restriction(bool _restricted) {
    restricted = _restricted;
}

void Node::update_pois(std::string place) {
    pois.push_back(place);
}
