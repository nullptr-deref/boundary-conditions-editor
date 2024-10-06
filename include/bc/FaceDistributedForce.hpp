#pragma once

#include "Load.hpp"

#include <array>

class FaceDistributedForce : public Load {
public:
    FaceDistributedForce();
    FaceDistributedForce(const std::array<double, 6> &projections);

    void deserialize(const json &object) override;
    json serialize() const override;

    std::string toString() const override;

private:
    std::array<double, 6> m_projections; // ( Fx, Fy, Fz, Mx, My, Mz )
};
