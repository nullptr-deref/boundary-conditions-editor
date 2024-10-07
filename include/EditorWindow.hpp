#pragma once

#include "bc/Restraint.hpp"
#include "bc/Load.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <QAction>
#include <QGroupBox>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

constexpr const char *WINDOW_NAME = "Boundary Conditions Editor";

const std::map<BoundaryConditionType, std::string> supportedBoundaryConditions = {
    { BoundaryConditionType::Load, "Loads" },
    { BoundaryConditionType::Restraint, "Restraints" }
};

const std::map<LoadType, std::string> supportedLoads = {
    { LoadType::DeadPointForce, "Dead point force" },
    { LoadType::FaceDistributedForce, "Distributed force" },
    { LoadType::FacePressure, "Pressure" }
};

const std::map<RestraintType, std::string> supportedRestraints = {
    { RestraintType::Displacement, "Displacement" }
};

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();
private:
    std::string m_filename = "";
    std::ifstream m_inputFilestream;

    std::shared_ptr<QAction> m_openFileAction = nullptr;
    std::shared_ptr<QAction> m_closeFileAction = nullptr;
    std::shared_ptr<QAction> m_recalculateProjectAction = nullptr;
    std::shared_ptr<QAction> m_exportFileAction = nullptr;
    std::shared_ptr<QAction> m_quitAction = nullptr;

    std::shared_ptr<QMenu> m_fileMenu = nullptr;
    std::shared_ptr<QMenu> m_projectMenu = nullptr;

    void prepareInternalActions();
    void prepareMenus();

    QLabel      *m_bcCounter = nullptr;

    QPushButton *m_exportButton = nullptr;
    QPushButton *m_recalculateButton = nullptr;

    void cleanupJsonCache();
    json m_fileContents;
    std::vector<BoundaryCondition *> m_boundaryConditions;
    size_t m_selectedItemIndex;

    QTreeView *m_bcTreeView = nullptr;
    QGroupBox *m_settings = nullptr;
    QStandardItemModel *m_model = nullptr;

    enum ItemRole {
        ID = Qt::UserRole,
        BCType,
        SpecificType
    };

    void updateTreeModel();

    bool m_fileCurrentlyOpened = false;

private slots:
    void updateTitleText(const std::string_view &sv);
    void updateCounter(size_t bcCount = 0);

    void selectItem(const QModelIndex &idx);

    // Slots which will be executed upon
    // QAction trigger.
    void selectAndOpenFile();
    void closeFile();
    void recalculateProject();
    void exportFile();
    void quit();

signals:
    void fileOpened(const std::string_view &filename);
    void boundaryConditionsParsed(size_t count);
    void fileClosed();
};
