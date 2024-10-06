#pragma once

#include "BoundaryCondition.hpp"

#include <array>
#include <iostream>

// Seems like commented out types of loads are not supported in non-binary export.
enum struct RestraintType : uint32_t {
    Displacement = 1
    // there are some more restraint types but I do not cover them in this
    // utility right now.
};

class Restraint : public BoundaryCondition {
public:
    Restraint(RestraintType type, const json &jsonObject) : BoundaryCondition(BoundaryConditionType::Restraint, jsonObject), restraintType(type) {}

    virtual void deserialize() = 0;
    virtual void serialize() = 0;

    RestraintType restraintType;
};
