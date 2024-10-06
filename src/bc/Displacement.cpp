#include "../../include/bc/Displacement.hpp"

Displacement::Displacement(const json &jsonObject) : Restraint(RestraintType::Displacement, jsonObject) {}

void Displacement::deserialize() {
    p_id = p_json["id"];
    size_t i = 0;
    for (auto &magnitude : magnitudes) {
        if (p_json["flag"][i] == 1) {
            magnitude = p_json["data"][i].front();
        }
        else magnitude = 0;
        ++i;
    }
}

void Displacement::serialize() {
    for (const auto &magnitude : magnitudes) {
        p_json["data"].push_back(std::vector<double>(1, magnitude));
        p_json["flag"].push_back(magnitude == 0 ? 0 : 1);
    }
}
