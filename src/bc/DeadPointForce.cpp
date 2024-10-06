#include "../../include/bc/DeadPointForce.hpp"

using namespace std::literals::string_literals;

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

std::string DeadPointForce::toString() const  {
    std::string str = "{ DeadPointForce: [ "s;
    for (const auto &proj : m_projections) {
        str += std::to_string(proj) + " "s;
    }
    str += "] }";

    return str;
}
