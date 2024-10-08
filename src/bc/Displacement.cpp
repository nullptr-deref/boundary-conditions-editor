#include "../../include/bc/Displacement.hpp"

Displacement::Displacement(const json &jsonObject) : Restraint(RestraintType::Displacement, jsonObject) {}

void Displacement::deserialize() {
    p_id = p_json["id"];
    p_name = p_json["name"];
    size_t i = 0;
    for (auto &magnitude : magnitudes) {
        directions[i] = p_json["flag"][i];
        if (directions[i] == 1) {
            magnitude = p_json["data"][i].front();
        }
        else magnitude = 0;
        ++i;
    }
}

void Displacement::serialize() {
    size_t i = 0;
    for (const auto &magnitude : magnitudes) {
        p_json["data"].push_back(std::vector<double>(1, magnitude));
        p_json["flag"].push_back(directions[i]);
        i++;
    }
}
