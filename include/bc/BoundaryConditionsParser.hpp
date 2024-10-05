#pragma once

#include "DeadPointForce.hpp"

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

class BoundaryConditionsParser {
public:
    BoundaryConditionsParser(const json &object);
    ~BoundaryConditionsParser();

    template <typename BC>
    std::vector<BC> getBoundaryConditions() const;
    void setJSON(const json &newObject);
private:
    std::vector<BoundaryCondition *> m_boundaryConditions;
    std::vector<BoundaryCondition *> parse();

    std::unique_ptr<json> m_object;
};

template <typename BC>
std::vector<BC> BoundaryConditionsParser::getBoundaryConditions() const {
    std::vector<BC> bcs;
    if constexpr (std::is_same<BC, Load>()) {
        for (const auto &load : (*m_object)["loads"]) {
            //switch(load["type"].get<uint32_t>()) {
            //    case static_cast<uint32_t>(ForceBoundaryConditionType::DeadPointForce):
            //    break;
            //    case static_cast<uint32_t>(ForceBoundaryConditionType::FacePressure):
            //    case static_cast<uint32_t>(ForceBoundaryConditionType::EdgePressure):
            //    break;
            //    case static_cast<uint32_t>(ForceBoundaryConditionType::EdgeDistributedForce):
            //    case static_cast<uint32_t>(ForceBoundaryConditionType::FaceDistributedForce):
            //    break;
            //}
        }
    }
    // if constexpr (std::is_same<BC, Pressure>) {
    // }
    // if constexpr (std::is_same<BC, Restraint>) {
    // }
}
