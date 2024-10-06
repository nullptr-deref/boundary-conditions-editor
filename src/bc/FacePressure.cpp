#include "../../include/bc/FacePressure.hpp"

using namespace std::literals::string_literals;

FacePressure::FacePressure() : Load(LoadType::FacePressure) {}
FacePressure::FacePressure(double magnitude)
: m_magnitude(magnitude), Load(LoadType::FacePressure) {}

void FacePressure::deserialize(const json &object) {
    m_magnitude = object["data"][0].front();
}

json FacePressure::serialize() const {
    std::clog << "Not implemented\n";
    return json({});
}

std::string FacePressure::toString() const  {
    return "{ FacePressure: "s + std::to_string(m_magnitude) + " }"s;
}
