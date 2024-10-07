#pragma once

#include "../JSONSerializable.hpp"

#include <string>

enum struct BoundaryConditionType {
    Load,
    Restraint
};

class BoundaryCondition : public JSONSerializable {
public:
    BoundaryCondition(BoundaryConditionType type, const json &jsonObject)
        : JSONSerializable(jsonObject), p_bcType(type) {}

    virtual void deserialize() = 0;
    virtual void serialize() = 0;

    virtual std::string toString() const { return p_json.dump(); }
    BoundaryConditionType type() const { return p_bcType; }
    size_t id() const { return p_id; }
    std::string name() const { return p_name; }
protected:
    BoundaryConditionType p_bcType;
    size_t p_id;
    std::string p_name;
};
