#pragma once

#include "bc/BoundaryConditionsParser.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <QAction>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

constexpr const char *WINDOW_NAME = "Boundary Conditions Editor";

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();
private:
    std::string m_filename;
    std::ifstream m_inputFilestream;
    //std::ofstream m_exportFilestream;

    std::shared_ptr<QAction> m_openFileAction = nullptr;
    std::shared_ptr<QAction> m_closeFileAction = nullptr;
    std::shared_ptr<QAction> m_filterAction = nullptr;
    std::shared_ptr<QAction> m_recalculateProjectAction = nullptr;
    std::shared_ptr<QAction> m_exportFileAction = nullptr;
    std::shared_ptr<QAction> m_quitAction = nullptr;

    std::shared_ptr<QMenu> m_fileMenu = nullptr;
    std::shared_ptr<QMenu> m_viewMenu = nullptr;
    std::shared_ptr<QMenu> m_projectMenu = nullptr;

    void prepareInternalActions();
    void prepareMenus();

    QWidget *m_bcView;
    QLabel  *m_bcCounter;
    QPushButton *m_exportButton;
    QPushButton *m_recalculateButton;
    QPushButton *m_filterButton;

    QWidget *createBCBox(BoundaryCondition *bc);

    void cleanupJsonCache();
    //BoundaryConditionsParser m_parser = BoundaryConditionsParser({});
    json m_fileContents;
    std::vector<BoundaryCondition *> m_boundaryConditions;

    bool m_fileCurrentlyOpened = false;

private slots:
    void updateTitleText(const std::string_view &sv);

    // Slots which will be executed upon
    // QAction trigger.
    void selectAndOpenFile();
    void closeFile();
    void filter();
    void recalculateProject();
    void exportFile();
    void quit();

signals:
    void fileOpened(const std::string_view &filename);
};
