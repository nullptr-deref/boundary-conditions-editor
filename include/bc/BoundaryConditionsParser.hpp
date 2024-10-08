#pragma once

#include "Pressure.hpp"
#include "Load.hpp"
#include "Restraint.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

namespace bc {
    enum struct ParsingOrigin {
        LoadsArray,
        RestraintsArray
    };

    class BoundaryConditionsParser {
    public:
        BoundaryConditionsParser(const nlohmann::json &object);
        ~BoundaryConditionsParser() = default;

        template<typename T, ParsingOrigin from>
        std::vector<T> parse() const;

        void setJSON(const nlohmann::json &newObject);
        nlohmann::json getJSON() const;
    private:
        std::unique_ptr<nlohmann::json> m_object;
    };
} // namespace bc

/*
 * Parses boundary conditions from the underlying JSON object.
 */
template <typename T, bc::ParsingOrigin from>
std::vector<T> bc::BoundaryConditionsParser::parse() const {
    std::vector<T> boundaries;
    if (!m_object->empty()) {
        if constexpr (from == ParsingOrigin::LoadsArray) {
            const auto accessor = "loads";
            if (m_object->contains(accessor)) {
                for (const auto &obj : (*m_object)[accessor]) {
                    if constexpr (std::is_same<T, Pressure>()) {
                        boundaries.push_back(Pressure(PressureType::Face, obj));
                    }
                    else {
                        switch(obj["type"].get<uint32_t>()) {
                            case static_cast<uint32_t>(LoadType::DeadPointForce): {
                                boundaries.push_back(Load(LoadType::DeadPointForce, obj));
                            } break;
                            case static_cast<uint32_t>(LoadType::FaceDistributedForce): {
                                boundaries.push_back(Load(LoadType::FaceDistributedForce, obj));
                            } break;
                        }
                    }
                }
            }
        }
        if constexpr (from == ParsingOrigin::RestraintsArray) {
            const auto accessor = "restraints";
            if (m_object->contains(accessor)) {
                for (const auto &obj : (*m_object)[accessor]) {
                    const auto &flags = obj["flag"];
                    switch((*std::find_if(flags.cbegin(), flags.cend(), [](auto &val) { return val != 0; })).template get<uint32_t>()) {
                        case static_cast<uint32_t>(RestraintType::Displacement):
                            boundaries.push_back(Restraint(RestraintType::Displacement, obj));
                            break;
                        // some place for further check (if there will ever be)
                    }
                }
            }
        }
    }

    return boundaries;
}
