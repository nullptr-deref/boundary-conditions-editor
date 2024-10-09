#include "../../include/bc/Restraint.hpp"

bc::ProjectionVector bc::Restraint::deserialize() {
    name = p_json["name"];
    id = p_json["id"].template get<size_t>();
    for (size_t i = 0; i < 6; i++) {
        data[i] = p_json["data"][i].front();
        flags[i] = p_json["flag"][i].template get<uint32_t>();
    }

    return data;
}

const json bc::Restraint::serialize() const {
    json copy = p_json;

    copy["name"] = name;
    for (size_t i = 0; i < 6; i++) {
        copy["data"][i] = json::array({ data[i] });
        copy["flag"][i] = flags[i];
    }

    return copy;
}
