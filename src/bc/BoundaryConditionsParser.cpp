#include "../../include/bc/BoundaryConditionsParser.hpp"

BoundaryConditionsParser::BoundaryConditionsParser(const json &object)
: m_object(std::move(std::make_unique<json>(object))) {
    m_boundaryConditions = parse();
}

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
                case static_cast<uint32_t>(LoadType::FacePressure):
                case static_cast<uint32_t>(LoadType::EdgePressure):
                break;
                case static_cast<uint32_t>(LoadType::EdgeDistributedForce):
                case static_cast<uint32_t>(LoadType::FaceDistributedForce):
                break;
            }
        }

        return bcs;
    }
}

void BoundaryConditionsParser::setJSON(const json &newObject) {
    m_object.reset(nullptr);
    m_object = std::make_unique<json>(newObject);
}

BoundaryConditionsParser::~BoundaryConditionsParser() {
    for (auto *bc : m_boundaryConditions) {
        if (bc) {
            delete bc;
        }
    }
}
