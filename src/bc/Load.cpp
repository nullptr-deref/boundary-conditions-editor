#include "../../include/bc/Load.hpp"

bc::ProjectionVector bc::Load::deserialize() {
    size_t i = 0;
    name = p_json["name"];
    id = p_json["id"].template get<size_t>();
    for (const auto &el : p_json["data"]) {
        data[i++] = el.front();
    }

    return data;
}

const json bc::Load::serialize() const {
    json copy = p_json;

    copy["name"] = name;
    copy["data"] = json::array();
    for (const auto &el : data) {
        copy["data"].push_back(json::array({ el }));
    }

    return copy;
}
