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
    Restraint(RestraintType type) : BoundaryCondition(BoundaryConditionType::Restraint), type(type) {}

    virtual void deserialize(const json &) = 0;
    virtual json serialize() const = 0;

    RestraintType type;
};
