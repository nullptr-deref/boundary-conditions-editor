#pragma once

#include "BoundaryCondition.hpp"

#include <array>
#include <iostream>

// Seems like commented out types of loads are not supported in non-binary export.
enum struct LoadType : uint32_t {
    FacePressure = 3,
    // EdgePressure = 2,
    DeadPointForce = 5, // for some reason no matter which mode I select dead point force is always node force
    // EdgeDistributedForce = 32,
    FaceDistributedForce = 35
};

class Load : public BoundaryCondition {
public:
    Load(LoadType type, const json &jsonObject) : BoundaryCondition(BoundaryConditionType::Load, jsonObject), loadType(type) {}

    virtual void deserialize() = 0;
    virtual void serialize() = 0;

    LoadType loadType;
};
