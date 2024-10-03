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
    m_openFileAct = std::make_shared<QAction>();
    m_openFileAct->setText(tr("&Open file"));
    connect(m_openFileAct.get(), &QAction::triggered, this, &EditorWindow::selectAndOpenFile);

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

void EditorWindow::cleanupJsonCache() {
    m_cache.reset();
}
