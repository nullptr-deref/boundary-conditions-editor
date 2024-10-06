#include "../include/EditorWindow.hpp"

#include "../include/bc/Restraint.hpp"
#include "../include/bc/Load.hpp"

#include <QFileDialog>
#include <QGroupBox>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>
#include <QScrollArea>
#include <QVBoxLayout>

#include <iostream>

RestraintType determineRestraintType(const json &);
json &getCorrespondingObject(BoundaryCondition *, json &);

EditorWindow::EditorWindow(QWidget *parent) : m_bcView(new QWidget), m_bcCounter(new QLabel) {
    prepareInternalActions();
    prepareMenus();

    connect(this, &EditorWindow::fileOpened, this, &EditorWindow::updateTitleText);

    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QScrollArea *scrollArea = new QScrollArea(centralWidget);
    scrollArea->setWidget(m_bcView);
    mainLayout->addWidget(scrollArea);

    QGroupBox *infoAndButtonsBox = new QGroupBox(centralWidget);
    QHBoxLayout *infoBoxLayout = new QHBoxLayout();
    infoBoxLayout->addWidget(m_bcCounter);
    m_exportButton = new QPushButton(tr("Export"));
    connect(m_exportButton, &QPushButton::pressed, this, &EditorWindow::exportFile);

    m_filterButton = new QPushButton(tr("Filter"));

    m_recalculateButton = new QPushButton(tr("Recalculate"));

    infoBoxLayout->addWidget(m_filterButton);
    infoBoxLayout->addWidget(m_exportButton);
    infoBoxLayout->addWidget(m_recalculateButton);
    infoAndButtonsBox->setLayout(infoBoxLayout);

    mainLayout->addWidget(infoAndButtonsBox);
    centralWidget->setLayout(mainLayout);
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
            try {
                m_fileContents = json::parse(m_inputFilestream);
                BoundaryConditionsParser parser(m_fileContents);
                m_boundaryConditions = parser.parse();
                m_fileCurrentlyOpened = true;

                emit fileOpened(m_filename);
            }
            catch (const json::exception &e) {
                std::clog << e.what() << '\n';
                QMessageBox alert;
                alert.setText(tr("The selected file is of unsupported format or ill-formed. Try opening another file."));
                alert.setStandardButtons(QMessageBox::Ok);
                alert.exec();
            }
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

void EditorWindow::exportFile() {
    auto contentsCopy = m_fileContents;

    for (const auto bc : m_boundaryConditions) {
        auto &obj = getCorrespondingObject(bc, contentsCopy);
        bc->serialize();
        obj = bc->getJSON();
    }

    const std::string exportedFileName = QFileDialog::getSaveFileName(
        this,
        tr("Open file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Fidesys Case Files (*.fc);;All files (*.*)")
    ).toStdString();

    if (!exportedFileName.empty()) {
        std::ofstream outputFileStream(exportedFileName);
        const auto pretty = contentsCopy.dump(4);
        outputFileStream << pretty;
    }
}

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
    m_fileContents = json::object();
}

/*
 * Returns appropriate JSON object which represents provided boundary condition.
 * The behavior is undefined if the provided boundary condition was not previously
 * declared in the input file.
 */
json &getCorrespondingObject(BoundaryCondition *bc, json &object) {
    json section = json::object();
    switch(static_cast<uint32_t>(bc->type())) {
        case static_cast<uint32_t>(BoundaryConditionType::Load): {
            section = object["loads"];
            for (auto &bcObj : section) {
                if (bcObj["type"] == dynamic_cast<Load *>(bc)->loadType && bcObj["id"] == bc->id()) {
                    return bcObj;
                }
            }
        } break;
        case static_cast<uint32_t>(BoundaryConditionType::Restraint): {
            section = object["restraints"];
            for (auto &bcObj : section) {
                if (determineRestraintType(bcObj) == dynamic_cast<Restraint *>(bc)->restraintType && bcObj["id"] == bc->id()) {
                    return bcObj;
                }
            }
        } break;
    }
}

RestraintType determineRestraintType(const json &obj) {
    for (uint32_t i = 1; i < 10; i++) {
        if (std::any_of(obj["flag"].begin(), obj["flag"].end(), [i](uint32_t flag) { return flag == i; })) {
            return static_cast<RestraintType>(i);
        }
    }
}

QWidget *EditorWindow::createBCBox(BoundaryCondition *bc) {
}
