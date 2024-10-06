#pragma once

#include "Load.hpp"

class FacePressure : public Load {
public:
    FacePressure();
    FacePressure(double magnitude);

    void deserialize(const json &object) override;
    json serialize() const override;

    std::string toString() const override;
private:
    double m_magnitude;
};
