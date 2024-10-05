#include "../../include/bc/DeadPointForce.hpp"

DeadPointForce::DeadPointForce() : Load(LoadType::DeadPointForce) {}
DeadPointForce::DeadPointForce(const std::array<double, 6> &projections)
: m_projections(projections), Load(LoadType::DeadPointForce) {}

void DeadPointForce::deserialize(const json &object) {
    size_t i = 0;
    for (auto &proj : m_projections) {
        proj = object["data"][i].front();
        ++i;
    }
}

json DeadPointForce::serialize() const {
    std::clog << "Not implemented\n";
    return json({});
}

std::ostream &operator<<(std::ostream &os, const DeadPointForce &load) {
    os << "[ ";
    for (const auto &proj : load.m_projections) {
        os << proj << ' ';
    }
    os << ']';

    return os;
}
