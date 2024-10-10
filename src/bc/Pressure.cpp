#include "../../include/bc/Pressure.hpp"

double bc::Pressure::deserialize() {
    name = p_json["name"];
    id = p_json["id"].template get<size_t>();
    data = p_json["data"].front();

    return data;
}
const json bc::Pressure::serialize() const {
    json copy = p_json;

    copy["name"] = name;
    copy["data"] = json::array({ data });

    return copy;
}

std::ostream &bc::operator<<(std::ostream &o, const bc::Pressure &p) {
    o << "\t{ id = " << p.id
        << ", name = " << p.name
        << ", type = " << static_cast<uint32_t>(p.type)
        << ", data = " << p.data
        << " }";

    return o;
}
