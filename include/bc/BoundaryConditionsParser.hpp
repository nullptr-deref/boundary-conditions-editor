#pragma once

#include "BoundaryCondition.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

class BoundaryConditionsParser {
public:
    BoundaryConditionsParser(const nlohmann::json &object);
    ~BoundaryConditionsParser() = default;

    std::vector<BoundaryCondition *> parse() const;
    void setJSON(const nlohmann::json &newObject);
    nlohmann::json getJSON() const;
    //const std::vector<BoundaryCondition *> &getBoundaryConditions() const;
private:
    //std::vector<BoundaryCondition *> m_boundaryConditions;
    std::unique_ptr<nlohmann::json> m_object;
};
