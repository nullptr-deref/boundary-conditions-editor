#include "../../include/bc/Pressure.hpp"

double bc::Pressure::deserialize() {
    data = p_json["data"].front();

    return data;
}
const json bc::Pressure::serialize() const {
    json copy = p_json;
    copy["data"] = json::array({ data });

    return copy;
}
