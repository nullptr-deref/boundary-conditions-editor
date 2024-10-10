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

std::ostream &bc::operator<<(std::ostream &o, const bc::Load &l) {
    o << "\t{ id = " << l.id
        << ", name = " << l.name
        << ", type = " << static_cast<uint32_t>(l.type)
        << ", data = [ ";
    for (const auto &el : l.data) {
        o << el << " ";
    }
    o << "] }";

    return o;
}
