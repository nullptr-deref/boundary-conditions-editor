#pragma once

#include "../JSONSerializable.hpp"

#include <array>
#include <iostream>

namespace bc {
    enum struct RestraintType : uint32_t {
        Displacement = 1
        // there are some more restraint types but I do not cover them in this
        // utility right now.
    };

    using ProjectionVector = std::array<double, 6>;
    // RestraintFlags marks whether specified DOF is restrained.
    using RestraintFlags = std::array<uint32_t, 6>;

    class Restraint : public JSONSerializable<ProjectionVector> {
    public:
        Restraint(RestraintType t, const json &j) : JSONSerializable<ProjectionVector>(j), type(t) {}
        ~Restraint() override {}

        const RestraintType type;
        std::string name;
        size_t id;

        ProjectionVector deserialize() override;
        const json serialize() const override;

        RestraintFlags flags;

        friend std::ostream &operator<<(std::ostream &, const Restraint &);
    };

    std::ostream &operator<<(std::ostream &, const Restraint &);
} // namespace bc
