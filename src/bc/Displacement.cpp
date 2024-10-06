#include "../../include/bc/Displacement.hpp"

using namespace std::literals::string_literals;

Displacement::Displacement() : Restraint(RestraintType::Displacement) {}
Displacement::Displacement(const std::array<double, 6> &magnitudes) : Restraint(RestraintType::Displacement), m_magnitudes(magnitudes) {}

void Displacement::deserialize(const json &object) {
    size_t i = 0;
    for (auto &magnitude : m_magnitudes) {
        if (object["flag"][i] == 1) {
            magnitude = object["data"][i].front();
        }
        else magnitude = 0;
        ++i;
    }
}

json Displacement::serialize() const {
    std::clog << "Not implemented\n";
    return json({});
}

std::string Displacement::toString() const  {
    std::string str = "{ Displacement: [ "s;
    for (const auto &magnitude : m_magnitudes) {
        str += std::to_string(magnitude) + " "s;
    }
    str += "] }";

    return str;
}
