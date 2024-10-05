#include "../include/EditorWindow.hpp"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>

EditorWindow::EditorWindow(QWidget *parent) {
    prepareInternalActions();
    prepareMenus();

    connect(this, &EditorWindow::fileOpened, this, &EditorWindow::updateTitleText);

    m_cache = std::make_unique<json>();
}

EditorWindow::~EditorWindow() {}

void EditorWindow::updateTitleText(const std::string_view &sv) {
    const std::string updatedTitle = std::string(WINDOW_NAME) + std::string(" | ") + std::string(sv);
    this->setWindowTitle(updatedTitle.c_str());
}

void EditorWindow::selectAndOpenFile() {
    if (m_fileCurrentlyOpened) {
        cleanupJsonCache();
        m_inputFilestream.close();
    }

    m_filename = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Fidesys Case Files (*.fc);;All files (*.*)")
    ).toStdString();

    if (!m_filename.empty()) {
        m_inputFilestream = std::ifstream(m_filename);

        if (m_inputFilestream.good()) {
            emit fileOpened(m_filename);
            m_fileCurrentlyOpened = true;

            *m_cache = json::parse(m_inputFilestream);
        }
        else {
            QMessageBox badFileAlert;
            badFileAlert.setText(tr("The selected file could not be opened due to read error. Try selecting another file or checking the selected file integrity."));
            badFileAlert.setStandardButtons(QMessageBox::Ok);
            badFileAlert.exec();

            m_inputFilestream.close();
        }
    }
}

void EditorWindow::closeFile() {}
void EditorWindow::filter() {}
void EditorWindow::recalculateProject() {}
void EditorWindow::exportFile() {}
void EditorWindow::quit() {}

void EditorWindow::prepareInternalActions() {
    m_openFileAction = std::make_shared<QAction>();
    m_openFileAction->setText(tr("&Open file"));
    connect(m_openFileAction.get(), &QAction::triggered, this, &EditorWindow::selectAndOpenFile);

    m_closeFileAction = std::make_shared<QAction>();
    m_closeFileAction->setText(tr("&Close"));

    m_filterAction = std::make_shared<QAction>();
    m_filterAction->setText(tr("&Filter"));

    m_recalculateProjectAction = std::make_shared<QAction>();
    m_recalculateProjectAction->setText(tr("&Recalculate project"));

    m_exportFileAction = std::make_shared<QAction>();
    m_exportFileAction->setText(tr("&Export"));

    m_quitAction = std::make_shared<QAction>();
    m_quitAction->setText(tr("&Quit"));
}

void EditorWindow::prepareMenus() {
    m_fileMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&File")));
    m_fileMenu->addAction(m_openFileAction.get());
    m_fileMenu->addAction(m_exportFileAction.get());
    m_fileMenu->addAction(m_closeFileAction.get());
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_quitAction.get());

    m_viewMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&View")));
    m_viewMenu->addAction(m_filterAction.get());

    m_projectMenu = std::shared_ptr<QMenu>(menuBar()->addMenu(tr("&Project")));
    m_projectMenu->addAction(m_recalculateProjectAction.get());
}

void EditorWindow::cleanupJsonCache() {
    m_cache.reset();
}
