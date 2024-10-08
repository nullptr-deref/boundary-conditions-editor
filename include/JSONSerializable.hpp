#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

template <typename T>
class JSONSerializable {
public:
    JSONSerializable(const json &j) : p_json(j) {}
    virtual ~JSONSerializable() {}

    template<typename U>
    friend struct JSONSerializer;
protected:
    json p_json;
private:
    virtual T deserialize() const = 0;
    virtual const json serialize(const T &data) = 0;
};

template <typename T>
struct JSONSerializer {
    static const T deserialize(const JSONSerializable<T> &j) {
        return j.deserialize();
    }

    static json serialize(JSONSerializable<T> &j, const T &data) {
        return j.serialize(data);
    }
};
