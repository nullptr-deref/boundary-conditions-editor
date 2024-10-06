#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JSONSerializable {
public:
    JSONSerializable(const json &jsonObject) : p_json(jsonObject) {}

    virtual void deserialize() = 0;
    virtual void serialize() = 0;
    const json &getJSON() const { return p_json; }
protected:
    json p_json;
};
