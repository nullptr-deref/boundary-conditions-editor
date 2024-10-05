#pragma once

#include "Load.hpp"

#include <array>
#include <iostream>

class DeadPointForce : public Load {
public:
    DeadPointForce();
    DeadPointForce(const std::array<double, 6> &projections);

    void deserialize(const json &object) override;
    json serialize() const override;

    friend std::ostream &operator<<(std::ostream &os, const DeadPointForce &load);
private:
    std::array<double, 6> m_projections; // ( Fx, Fy, Fz, Mx, My, Mz )
};
