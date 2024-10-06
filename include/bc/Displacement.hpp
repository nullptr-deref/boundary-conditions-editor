#pragma once

#include "Restraint.hpp"

class Displacement : public Restraint {
public:
    Displacement(const json &jsonObject);

    void deserialize() override;
    void serialize() override;

    std::array<double, 6> magnitudes;
};
