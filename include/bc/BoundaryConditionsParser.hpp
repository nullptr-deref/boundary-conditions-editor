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
    ~BoundaryConditionsParser();

    //template <typename BC>
    //std::vector<BC> getBoundaryConditions() const;
    std::vector<BoundaryCondition *> parse();
    void setJSON(const nlohmann::json &newObject);
private:
    std::vector<BoundaryCondition *> m_boundaryConditions;
    std::unique_ptr<nlohmann::json> m_object;
};

/*
template <typename BC>
std::vector<BC> BoundaryConditionsParser::getBoundaryConditions() const {
    std::vector<BC> bcs;
    if constexpr (std::is_same<BC, Load>()) {
        for (const auto &load : (*m_object)["loads"]) {
        }
    }
}
*/
