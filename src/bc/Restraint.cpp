#include "../../include/bc/Restraint.hpp"

bc::ProjectionVector bc::Restraint::deserialize() {
    for (size_t i = 0; i < 6; i++) {
        data[i] = p_json["data"][i].front();
        flags[i] = p_json["flag"][i];
    }

    return data;
}

const json bc::Restraint::serialize() const {
    json copy = p_json;
    for (size_t i = 0; i < 6; i++) {
        copy["data"][i] = json::array({ data[i] });
        copy["flag"][i] = flags[i];
    }

    return copy;
}
