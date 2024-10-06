#include "../../include/bc/FaceDistributedForce.hpp"

using namespace std::literals::string_literals;

FaceDistributedForce::FaceDistributedForce(const json &jsonObject)
: Load(LoadType::FaceDistributedForce, jsonObject) {}

void FaceDistributedForce::deserialize() {
    size_t i = 0;
    p_id = p_json["id"];
    for (auto &proj : projections) {
        proj = p_json["data"][i].front();
        ++i;
    }
}

void FaceDistributedForce::serialize() {
    p_json["data"] = json::array();
    size_t i = 0;
    for (const auto &proj : projections) {
        p_json["data"][i] = json::array({ proj });
    }
}
