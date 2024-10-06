#include "../../include/bc/FaceDistributedForce.hpp"

using namespace std::literals::string_literals;

FaceDistributedForce::FaceDistributedForce() : Load(LoadType::FaceDistributedForce) {}
FaceDistributedForce::FaceDistributedForce(const std::array<double, 6> &projections)
: m_projections(projections), Load(LoadType::FaceDistributedForce) {}

void FaceDistributedForce::deserialize(const json &object) {
    size_t i = 0;
    for (auto &proj : m_projections) {
        proj = object["data"][i].front();
        ++i;
    }
}

json FaceDistributedForce::serialize() const {
    std::clog << "Not implemented\n";
    return json({});
}

std::string FaceDistributedForce::toString() const  {
    std::string str = "{ FaceDistributedForce: [ "s;
    for (const auto &proj : m_projections) {
        str += std::to_string(proj) + " "s;
    }
    str += "] }";

    return str;
}
