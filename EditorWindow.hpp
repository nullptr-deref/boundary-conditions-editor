#pragma once

#include <QAction>
#include <QMainWindow>

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
    // std::vector<QAction> m_actions;
    std::string m_filename;

    std::shared_ptr<QAction> m_openFileAct = nullptr;
    std::shared_ptr<QAction> m_closeFileAct = nullptr;
    std::shared_ptr<QAction> m_filterAct = nullptr;
    std::shared_ptr<QAction> m_recalculateProjectAct = nullptr;
    std::shared_ptr<QAction> m_exportFileAct = nullptr;
    std::shared_ptr<QAction> m_quitAct = nullptr;

    std::shared_ptr<QMenu> m_fileMenu;
    std::shared_ptr<QMenu> m_viewMenu;
    std::shared_ptr<QMenu> m_projectMenu;

    void prepareInternalActions();
    void prepareMenus();

private slots:
    void updateTitleText(const std::string_view &sv);

    // Slots which will be executed upon
    // QAction trigger.
    void openFile(const std::string_view &filename);
    void closeFile();
    void filter();
    void recalculateProject();
    void exportFile();
    void quit();

signals:
    void fileOpened(const std::string_view &filename);
};
