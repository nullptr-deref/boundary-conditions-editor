#pragma once

#include "../JSONSerializable.hpp"

#include <string>

enum struct BoundaryConditionType {
    Load,
    Restraint
};

class BoundaryCondition : public JSONSerializable {
public:
    BoundaryCondition(BoundaryConditionType type) : bcType(type) {}

    virtual void deserialize(const json &) = 0;
    virtual json serialize() const = 0;

    virtual std::string toString() const = 0;
    BoundaryConditionType type() const { return bcType; }
protected:
    BoundaryConditionType bcType;
};
