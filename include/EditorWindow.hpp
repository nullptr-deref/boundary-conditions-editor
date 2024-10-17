#pragma once

#include "bc/Restraint.hpp"
#include "bc/Load.hpp"
#include "bc/Pressure.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <QAction>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QGridLayout>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QTreeView>

#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

constexpr const char *WINDOW_NAME = "Boundary Conditions Editor";

constexpr size_t DEFAULT_WIDTH = 1080;
constexpr size_t DEFAULT_HEIGHT = 720;

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();
private:
    std::string m_filename = "";
    std::ifstream m_inputFilestream;

    std::array<std::shared_ptr<QAction>, 5> m_actions = { nullptr };
    std::array<std::string, 5> m_actionNames {
        "&Open file",
        "&Close",
        "&Recalculate project",
        "&Export",
        "&Quit"
    };
    std::array<QKeySequence, 5> m_shortcuts = {
        QKeySequence(QKeySequence::Open),
        QKeySequence(QKeySequence::Close),
        QKeySequence(QKeySequence(tr("Ctrl+R"))),
        QKeySequence(QKeySequence::SaveAs),
        QKeySequence(QKeySequence::Quit)
    };

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

    std::vector<bc::Load> m_forces;
    std::vector<bc::Pressure> m_pressures;
    std::vector<bc::Restraint> m_displacements;
    void clearBoundaries();

    enum struct ItemRole : int {
        ID = Qt::UserRole,
        Type,
        BType
    };

    enum struct BCType : uint32_t {
        Load,
        Pressure,
        Displacement
    };

    struct Selection {
        size_t id;
        uint32_t type;
        BCType btype;
    } m_selectedItem;

    void loadParsedData();

    QTreeView *m_treeView = nullptr;
    QGroupBox *m_settings = nullptr;

    QWidget *m_forcesSettingsWidget = nullptr;
    QWidget *m_pressuresSettingsWidget = nullptr;
    QWidget *m_displacementsSettingsWidget = nullptr;
    QWidget *m_currentSettingsWidget = nullptr;

    QStandardItemModel *m_model = nullptr;

    void constructForcesWidget();
    void constructPressuresWidget();
    void constructDisplacementsWidget();

    void initializeSettingsEditors();
    void initializeSettingsCheckboxes();

    std::array<QMetaObject::Connection, 6> m_checkBoxesConnections;
    void disconnectCheckBoxes();
    void connectCheckBoxes();

    void updateTreeModel();
    template <typename T>
    void constructSettingsWidget(const T &data);

    std::array<QLineEdit *, 13> m_editors = { nullptr };
    static constexpr size_t FORCE_EDITORS_IDX = 0;
    static constexpr size_t DISPLACEMENT_EDITORS_IDX = 6;
    static constexpr size_t PRESSURE_EDITOR_IDX = 12;
    std::array<QCheckBox *, 6> m_checks = { nullptr };

    bool m_fileCurrentlyOpened = false;

private slots:
    void updateTitleText(const std::string_view &sv = std::string_view(""));
    void updateCounter(size_t bcCount = 0);

    void selectItem(const QModelIndex &idx);

    void writeChangesToSelectedItem(const std::optional<std::string> &optS, const std::optional<Qt::CheckState> &optCs);

    // Slots which will be executed upon
    // QAction trigger.
    void selectAndOpenFile();
    void closeFile();
    void recalculateProject();
    void exportFile();
    void quit();

private:
    std::array<void (EditorWindow::*)(void), 5> m_signals = {
        &EditorWindow::selectAndOpenFile,
        &EditorWindow::closeFile,
        &EditorWindow::recalculateProject,
        &EditorWindow::exportFile,
        &EditorWindow::quit
    };

signals:
    void fileOpened(const std::string_view &filename);
    void boundaryConditionsParsed(size_t count);
    void fileClosed();
};

template <typename T>
void EditorWindow::constructSettingsWidget(const T &data) {
    if (m_currentSettingsWidget) {
        m_currentSettingsWidget->hide();
        m_settings->layout()->removeWidget(m_currentSettingsWidget);
    }
    if constexpr (std::is_same<T, bc::ProjectionVector>()) {
        if (!m_forcesSettingsWidget) {
            constructForcesWidget();
        }
        for (size_t i = 0; i < 6; i++) {
            m_editors[i]->setText(QString::number(data[FORCE_EDITORS_IDX + i]));
        }
        m_currentSettingsWidget = m_forcesSettingsWidget;
    }
    if constexpr (std::is_same<T, double>()) {
        if (!m_pressuresSettingsWidget) {
            constructPressuresWidget();
        }
        m_editors[PRESSURE_EDITOR_IDX]->setText(QString::number(data));
        m_currentSettingsWidget = m_pressuresSettingsWidget;
    }
    // Passing two 6-element vectors to deal with displacements
    if constexpr (std::is_same<T, std::pair<bc::ProjectionVector, bc::RestraintFlags>>()) {
        if (!m_displacementsSettingsWidget) {
            constructDisplacementsWidget();
        }
        disconnectCheckBoxes();
        for (size_t i = 0; i < 6; i++) {
            m_editors[DISPLACEMENT_EDITORS_IDX + i]->setText(QString::number(data.first[i]));
        }

        size_t i = 0;
        for (auto *c: m_checks) {
            c->setCheckState(data.second[i] == 0 ? Qt::Unchecked : Qt::Checked);
            i++;
        }
        connectCheckBoxes();
        m_currentSettingsWidget = m_displacementsSettingsWidget;
    }

    m_settings->layout()->addWidget(m_currentSettingsWidget);
    m_currentSettingsWidget->show();
}
