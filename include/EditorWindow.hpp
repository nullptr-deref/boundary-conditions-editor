#pragma once

#include "bc/Restraint.hpp"
#include "bc/Load.hpp"
#include "bc/Pressure.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <QAction>
#include <QGroupBox>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>

#include <array>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

constexpr const char *WINDOW_NAME = "Boundary Conditions Editor";

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

    size_t m_selectedItemIndex;

    void loadParsedData();

    QTreeView *m_treeView = nullptr;
    QGroupBox *m_settings = nullptr;
    QStandardItemModel *m_model = nullptr;

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

    void updateTreeModel();
    template <typename T>
    void sendDataToSettingsWidget(T &data);

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

template <typename T>
void EditorWindow::sendDataToSettingsWidget(T &data) {
    if constexpr (std::is_same<T, bc::ProjectionVector>()) {
    }
    if constexpr (std::is_same<T, double>()) {
    }
    // Passing two 6-element vectors to deal with displacements
    if constexpr (std::is_same<T, std::array<bc::ProjectionVector, 2>>()) {
    }
}
