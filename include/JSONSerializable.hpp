#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

template <typename T>
class JSONSerializable {
public:
    JSONSerializable(const json &j) : p_json(j) { }
    virtual ~JSONSerializable() {}

    template<typename U>
    friend struct JSONSerializer;

    T data;
    virtual T deserialize() = 0;
    virtual const json serialize() const = 0;
protected:
    json p_json;
};
