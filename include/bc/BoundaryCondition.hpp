#pragma once

#include "../JSONSerializable.hpp"

enum struct BoundaryConditionType {
    Load,
    Restraint
};

class BoundaryCondition : public JSONSerializable {
public:
    BoundaryCondition(BoundaryConditionType type) : bcType(type) {}

    virtual void deserialize(const json &) = 0;
    virtual json serialize() const = 0;
protected:
    BoundaryConditionType bcType;
};
