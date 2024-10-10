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

    void updateTreeModel();
    template <typename T>
    void constructSettingsWidget(T &data);

    std::array<QLineEdit *, 6> m_editors = { nullptr };
    std::array<QCheckBox *, 6> m_checks = { nullptr };

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

    void writeChangesToSelectedItem(const std::optional<std::string> &optS, const std::optional<Qt::CheckState> &optCs);

signals:
    void fileOpened(const std::string_view &filename);
    void boundaryConditionsParsed(size_t count);
    void fileClosed();
};

template <typename T>
void EditorWindow::constructSettingsWidget(T &data) {
    if (m_currentSettingsWidget) {
        m_currentSettingsWidget->hide();
        m_settings->layout()->removeWidget(m_currentSettingsWidget);
    }
    if constexpr (std::is_same<T, bc::ProjectionVector>()) {
        if (!m_forcesSettingsWidget) {
            constructForcesWidget();
        }
        size_t i = 0;
        for (auto *e : m_editors) {
            e->setText(QString::number(data[i]));
            i++;
        }
        m_currentSettingsWidget = m_forcesSettingsWidget;
    }
    if constexpr (std::is_same<T, double>()) {
        if (!m_pressuresSettingsWidget) {
            constructPressuresWidget();
        }
        m_currentSettingsWidget = m_pressuresSettingsWidget;
    }
    // Passing two 6-element vectors to deal with displacements
    if constexpr (std::is_same<T, std::pair<bc::ProjectionVector, bc::RestraintFlags>>()) {
        if (!m_displacementsSettingsWidget) {
            constructDisplacementsWidget();
        }
        size_t i = 0;
        for (auto *e : m_editors) {
            e->setText(QString::number(data.first[i]));
            i++;
        }
        m_currentSettingsWidget = m_displacementsSettingsWidget;
    }

    m_settings->layout()->addWidget(m_currentSettingsWidget);
    m_currentSettingsWidget->show();
}
