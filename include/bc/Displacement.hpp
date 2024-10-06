#pragma once

#include "Restraint.hpp"

class Displacement : public Restraint {
public:
    Displacement();
    Displacement(const std::array<double, 6> &magnitudes);

    void deserialize(const json &object) override;
    json serialize() const override;

    std::string toString() const override;

private:
    std::array<double, 6> m_magnitudes;
};
