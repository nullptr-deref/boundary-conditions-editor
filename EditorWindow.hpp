#pragma once

#include <QAction>
#include <QMainWindow>

#include <string>
#include <vector>

class EditorWindow : public QMainWindow {
    Q_OBJECT
public:
    EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();
private:
    std::vector<QAction> m_actions;

private slots:
    void updateTitleText(const std::string_view &sv) const;

    // Slots which will be executed upon
    // QAction trigger.
    void openFile();
    void closeFile();
    void filter();
    void recalculateProject();
    void exportFile();
    void closeWindow();
};
