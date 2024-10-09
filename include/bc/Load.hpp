#pragma once

#include "../JSONSerializable.hpp"

#include <array>

namespace bc {
    // Seems like commented out types of loads are not supported in non-binary export.
    enum struct LoadType : uint32_t {
        DeadPointForce = 5, // for some reason no matter which mode I select dead point force is always node force
        // EdgeDistributedForce = 32,
        FaceDistributedForce = 35
    };

    using ProjectionVector = std::array<double, 6>;

    class Load : public JSONSerializable<ProjectionVector> {
    public:
        Load(LoadType t, const json &j) : JSONSerializable(j), type(t) {}
        ~Load() override {}

        const LoadType type;
        std::string name;
        size_t id;

        ProjectionVector deserialize() override;
        const json serialize() const override;
    };
} // namespace bc
