#pragma once

#include "Load.hpp"

#include <array>

class FaceDistributedForce : public Load {
public:
    FaceDistributedForce(const json &jsonObject);

    void deserialize() override;
    void serialize() override;

    std::array<double, 6> projections; // ( Fx, Fy, Fz, Mx, My, Mz )
};
