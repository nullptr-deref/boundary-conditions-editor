#include "../../include/bc/FacePressure.hpp"

using namespace std::literals::string_literals;

FacePressure::FacePressure(const json &jsonObject) : Load(LoadType::FacePressure, std::move(jsonObject)) {}

void FacePressure::deserialize() {
    p_id = p_json["id"];
    magnitude = p_json["data"][0].front();
}

void FacePressure::serialize() {
    p_json["data"] = json::array({ json::array({ magnitude }) });
}
