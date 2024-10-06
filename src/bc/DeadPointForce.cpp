#include "../../include/bc/DeadPointForce.hpp"

DeadPointForce::DeadPointForce(const json &jsonObject) : Load(LoadType::DeadPointForce, jsonObject) {}

void DeadPointForce::deserialize() {
    p_id = p_json["id"];
    size_t i = 0;
    for (auto &proj : projections) {
        proj = p_json["data"][i].front();
        ++i;
    }
}

void DeadPointForce::serialize() {
    p_json["data"] = json::array();
    size_t i = 0;
    for (const auto &proj : projections) {
        p_json["data"][i] = json::array({ proj });
    }
}
