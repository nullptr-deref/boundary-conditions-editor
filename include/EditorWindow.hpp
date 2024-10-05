#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <QAction>
#include <QMainWindow>

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
    std::ifstream m_exportFilestream;

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

    void cleanupJsonCache();
    std::unique_ptr<json> m_cache = nullptr;

    std::vector<JsonProxy>

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
