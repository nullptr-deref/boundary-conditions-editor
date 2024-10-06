#include "../../include/bc/BoundaryConditionsParser.hpp"

#include "../../include/bc/DeadPointForce.hpp"
#include "../../include/bc/FaceDistributedForce.hpp"
#include "../../include/bc/FacePressure.hpp"
#include "../../include/bc/Displacement.hpp"

#include <algorithm>

BoundaryConditionsParser::BoundaryConditionsParser(const nlohmann::json &object)
: m_object(std::move(std::make_unique<nlohmann::json>(object))) {
    m_boundaryConditions = parse();
}

/*
 * Parses boundary conditions from the underlying JSON object.
 * Fills private list with parsed data and returns its copy.
 */
std::vector<BoundaryCondition *> BoundaryConditionsParser::parse() {
    if (!m_object->empty()) {
        std::vector<BoundaryCondition *> bcs;
        for (const auto &obj : (*m_object)["loads"]) {
            switch(obj["type"].get<uint32_t>()) {
                case static_cast<uint32_t>(LoadType::DeadPointForce): {
                    auto *force = new DeadPointForce();
                    force->deserialize(obj);
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(force));
                } break;
                case static_cast<uint32_t>(LoadType::FacePressure): {
                    auto *pressure = new FacePressure();
                    pressure->deserialize(obj);
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(pressure));
                } break;
                case static_cast<uint32_t>(LoadType::FaceDistributedForce): {
                    auto *distributedForce = new FaceDistributedForce();
                    distributedForce->deserialize(obj);
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(distributedForce));
                } break;
            }
        }
        for (const auto &obj : (*m_object)["restraints"]) {
            const auto &flags = obj["flag"];
            if (std::any_of(flags.begin(), flags.end(), [&](auto flag) { return flag == 1; })) {
                auto *displacement = new Displacement();
                displacement->deserialize(obj);
                bcs.push_back(dynamic_cast<BoundaryCondition *>(displacement));
            }
        }

        m_boundaryConditions = bcs;
        return bcs;
    }
    else return std::vector<BoundaryCondition *>();
}

void BoundaryConditionsParser::setJSON(const nlohmann::json &newObject) {
    m_object.reset(nullptr);
    m_object = std::make_unique<nlohmann::json>(newObject);
}

BoundaryConditionsParser::~BoundaryConditionsParser() {
    for (auto *bc : m_boundaryConditions) {
        if (bc) {
            delete bc;
        }
    }
}
