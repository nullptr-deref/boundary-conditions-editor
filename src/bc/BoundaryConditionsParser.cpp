#include "../../include/bc/BoundaryConditionsParser.hpp"

#include "../../include/bc/DeadPointForce.hpp"
#include "../../include/bc/FaceDistributedForce.hpp"
#include "../../include/bc/FacePressure.hpp"
#include "../../include/bc/Displacement.hpp"

#include <algorithm>

BoundaryConditionsParser::BoundaryConditionsParser(const nlohmann::json &object)
: m_object(std::move(std::make_unique<nlohmann::json>(object))) {}

/*
 * Parses boundary conditions from the underlying JSON object.
 * Fills private list with parsed data and returns its copy.
 */
std::vector<BoundaryCondition *> BoundaryConditionsParser::parse() const {
    std::vector<BoundaryCondition *> bcs;
    if (!m_object->empty()) {
        for (const auto &obj : (*m_object)["loads"]) {
            switch(obj["type"].get<uint32_t>()) {
                case static_cast<uint32_t>(LoadType::DeadPointForce): {
                    auto *force = new DeadPointForce(obj);
                    force->deserialize();
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(force));
                } break;
                case static_cast<uint32_t>(LoadType::FacePressure): {
                    auto *pressure = new FacePressure(obj);
                    pressure->deserialize();
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(pressure));
                } break;
                case static_cast<uint32_t>(LoadType::FaceDistributedForce): {
                    auto *distributedForce = new FaceDistributedForce(obj);
                    distributedForce->deserialize();
                    bcs.push_back(dynamic_cast<BoundaryCondition *>(distributedForce));
                } break;
            }
        }
        for (const auto &obj : (*m_object)["restraints"]) {
            const auto &flags = obj["flag"];
            if (std::any_of(flags.begin(), flags.end(), [&](auto flag) { return flag == 1; })) {
                auto *displacement = new Displacement(obj);
                displacement->deserialize();
                bcs.push_back(dynamic_cast<BoundaryCondition *>(displacement));
            }
        }
    }

    return bcs;
}

void BoundaryConditionsParser::setJSON(const nlohmann::json &newObject) {
    m_object.reset(nullptr);
    m_object = std::make_unique<nlohmann::json>(newObject);
}

nlohmann::json BoundaryConditionsParser::getJSON() const {
    return *m_object;
}

//const std::vector<BoundaryCondition *> &BoundaryConditionsParser::getBoundaryConditions() const {
//    return m_boundaryConditions;
//}

//BoundaryConditionsParser::~BoundaryConditionsParser() {
//    for (auto *bc : m_boundaryConditions) {
//        if (bc) {
//            delete bc;
//        }
//    }
//}
