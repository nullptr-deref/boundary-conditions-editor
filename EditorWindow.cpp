#include "./EditorWindow.hpp"

#include <QMenu>
#include <QMenuBar>

#include <iostream>

EditorWindow::EditorWindow(QWidget *parent) {
    QMenu *fileMenu = menuBar()->addMenu("File");
}

EditorWindow::~EditorWindow() {}

void EditorWindow::updateTitleText(const std::string_view &sv) const {
    std::cout << "Not implemented.\n";
}

void EditorWindow::openFile() {}
void EditorWindow::closeFile() {}
void EditorWindow::filter() {}
void EditorWindow::recalculateProject() {}
void EditorWindow::exportFile() {}
void EditorWindow::closeWindow() {}
