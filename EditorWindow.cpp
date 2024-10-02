#include "./EditorWindow.hpp"

#include <QMenu>
#include <QMenuBar>

#include <iostream>

EditorWindow::EditorWindow(QWidget *parent) {
    prepareInternalActions();
    prepareMenus();
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
void EditorWindow::quit() {}

void EditorWindow::prepareInternalActions() {
    m_openFileAct = std::make_shared<QAction>();
    m_openFileAct->setText(tr("&Open file"));

    m_closeFileAct = std::make_shared<QAction>();
    m_closeFileAct->setText(tr("&Close"));

    m_filterAct = std::make_shared<QAction>();
    m_filterAct->setText(tr("&Filter"));

    m_recalculateProjectAct = std::make_shared<QAction>();
    m_recalculateProjectAct->setText(tr("&Recalculate project"));

    m_exportFileAct = std::make_shared<QAction>();
    m_exportFileAct->setText(tr("&Export"));

    m_quitAct = std::make_shared<QAction>();
    m_quitAct->setText(tr("&Quit"));
}

void EditorWindow::prepareMenus() {
    m_fileMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&File")));
    m_fileMenu->addAction(m_openFileAct.get());
    m_fileMenu->addAction(m_exportFileAct.get());
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_closeFileAct.get());

    m_viewMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&View")));
    m_viewMenu->addAction(m_filterAct.get());

    m_projectMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&Project")));
    m_projectMenu->addAction(m_recalculateProjectAct.get());
}
