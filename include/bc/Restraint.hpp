#pragma once

#include "../JSONSerializable.hpp"

#include <array>

namespace bc {
    enum struct RestraintType : uint32_t {
        Displacement = 1
        // there are some more restraint types but I do not cover them in this
        // utility right now.
    };

    using ProjectionVector = std::array<double, 6>;

    class Restraint : public JSONSerializable<ProjectionVector> {
    public:
        Restraint(RestraintType t, const json &j) : JSONSerializable<ProjectionVector>(j), type(t) {}
        ~Restraint() override {}

        const RestraintType type;
    private:
        ProjectionVector deserialize() const override;
        const json serialize(const ProjectionVector &data) override;
    };
} // namespace bc
