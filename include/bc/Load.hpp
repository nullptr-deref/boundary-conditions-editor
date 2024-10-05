#pragma once

#include "BoundaryCondition.hpp"

#include <array>
#include <iostream>

enum struct LoadType : uint32_t {
    FacePressure = 1,
    EdgePressure = 2,
    DeadPointForce = 6,
    EdgeDistributedForce = 32,
    FaceDistributedForce = 35
};

class Load : public BoundaryCondition {
public:
    Load(LoadType type) : BoundaryCondition(BoundaryConditionType::Load), type(type) {}

    virtual void deserialize(const json &) = 0;
    virtual json serialize() const = 0;

    LoadType type;
};
