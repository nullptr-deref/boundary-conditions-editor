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

std::ostream &bc::operator<<(std::ostream &o, const bc::Restraint &r) {
    o << "{ id = " << r.id
        << ", name = \"" << r.name << "\""
        << ", type = " << static_cast<uint32_t>(r.type)
        << ", data = [ ";
    for (const auto &el : r.data) {
        o << el << " ";
    }
    o << "], flags = [ ";
    for (const auto &f : r.flags) {
        o << f << " ";
    }
    o << "] }\n";

    return o;
}
