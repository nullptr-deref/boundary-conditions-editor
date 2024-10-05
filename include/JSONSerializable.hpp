#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JSONSerializable {
public:
    JSONSerializable() = default;

    virtual void deserialize(const json &) = 0;
    virtual json serialize() const = 0;
};
