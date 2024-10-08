#include "../../include/bc/BoundaryConditionsParser.hpp"

bc::BoundaryConditionsParser::BoundaryConditionsParser(const nlohmann::json &object)
: m_object(std::move(std::make_unique<nlohmann::json>(object))) {}


void bc::BoundaryConditionsParser::setJSON(const nlohmann::json &newObject) {
    m_object.reset(nullptr);
    m_object = std::make_unique<nlohmann::json>(newObject);
}

nlohmann::json bc::BoundaryConditionsParser::getJSON() const {
    return *m_object;
}
