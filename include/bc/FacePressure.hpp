#pragma once

#include "Load.hpp"

class FacePressure : public Load {
public:
    FacePressure(const json &jsonObject);

    void deserialize() override;
    void serialize() override;

    double magnitude;
};
