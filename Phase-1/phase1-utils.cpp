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

Edge::Edge(unsigned int _id, Node* _from, Node* _to, std::string type) {
    id = _id;
    from = _from;
    to = _to;
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