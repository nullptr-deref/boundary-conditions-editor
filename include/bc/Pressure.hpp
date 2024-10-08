#pragma once

#include "../JSONSerializable.hpp"

#include <array>

namespace bc {
    // Seems like commented out types of loads are not supported in non-binary export.
    enum struct PressureType : uint32_t {
        Face = 3,
        // Edge = 2,
    };

    using ProjectionVector = std::array<double, 6>;

    class Pressure : public JSONSerializable<double> {
    public:
        Pressure(PressureType t, const json &j) : JSONSerializable(j), type(t) {}
        ~Pressure() override {}

        const PressureType type;
    private:
        double deserialize() const override;
        const json serialize(const double &magnitude) override;
    };
} // namespace bc
