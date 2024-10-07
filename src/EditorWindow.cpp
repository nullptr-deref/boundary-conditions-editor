#include "../include/EditorWindow.hpp"

#include "../include/bc/Restraint.hpp"
#include "../include/bc/Load.hpp"
#include "../include/bc/BoundaryConditionsParser.hpp"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStandardPaths>
#include <QScrollArea>
#include <QVBoxLayout>

#include <iostream>

RestraintType determineRestraintType(const json &);
json &getCorrespondingObject(BoundaryCondition *, json &);

EditorWindow::EditorWindow(QWidget *parent) {
    prepareInternalActions();
    prepareMenus();

    connect(this, &EditorWindow::fileOpened, this, &EditorWindow::updateTitleText);

    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout  = new QVBoxLayout(centralWidget);

    QGroupBox *upperBox = new QGroupBox(tr("Boundary conditions"), centralWidget);
    m_model = new QStandardItemModel(this);
    m_settings = new QGroupBox();

    m_bcTreeView = new QTreeView();
    m_bcTreeView->setModel(m_model);

    QHBoxLayout *upperLayout = new QHBoxLayout;
    upperLayout->addWidget(m_bcTreeView);
    upperLayout->addWidget(m_settings);
    upperLayout->setStretchFactor(m_bcTreeView, 10);
    upperLayout->setStretchFactor(m_settings, 20);

    upperBox->setLayout(upperLayout);

    mainLayout->addWidget(upperBox);

    QGroupBox *buttonsBox = new QGroupBox(tr("File info and actions"), centralWidget);
    QHBoxLayout *buttonsBoxLayout = new QHBoxLayout();

    m_bcCounter = new QLabel("", buttonsBox);
    buttonsBoxLayout->addWidget(m_bcCounter);
    buttonsBoxLayout->addStretch();

    m_exportButton = new QPushButton(tr("Export"));
    m_recalculateButton = new QPushButton(tr("Recalculate"));

    buttonsBoxLayout->addWidget(m_exportButton);
    buttonsBoxLayout->addWidget(m_recalculateButton);
    buttonsBox->setLayout(buttonsBoxLayout);

    mainLayout->addWidget(buttonsBox);

    connect(m_exportButton, &QPushButton::pressed, this, &EditorWindow::exportFile);
    connect(this, &EditorWindow::boundaryConditionsParsed, this, &EditorWindow::updateCounter);

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

                updateTreeModel();

                emit boundaryConditionsParsed(m_boundaryConditions.size());
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
void EditorWindow::recalculateProject() {
    QMessageBox notImplementedAlert;
    notImplementedAlert.setText(tr("This feature is not yet implemented."));
    notImplementedAlert.setStandardButtons(QMessageBox::Ok);
    notImplementedAlert.exec();
}

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

void EditorWindow::updateCounter(size_t bcCount) {
    if (bcCount == 0) {
        if (m_filename.empty())
            m_bcCounter->setText(tr("No file opened."));
        else m_bcCounter->setText(tr("No boundaries in the file."));
    }
    else {
        m_bcCounter->setText(tr("Number of detected boundary conditions: %1").arg(bcCount));
    }
}

void EditorWindow::updateTreeModel() {
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList() << "Boundary condition" << "Type" << "ID");
    size_t i = 0;
    std::array<QStandardItem *, 2> primaryBCTypes = { nullptr, nullptr };
    for (const auto &[type, name] : supportedBoundaryConditions) {
        if (std::any_of(
                m_boundaryConditions.cbegin(),
                m_boundaryConditions.cend(),
                [type](auto *boundary) { return boundary->type() == type; })
        ) {
            primaryBCTypes[i] = new QStandardItem(name.c_str());
            m_model->appendRow(primaryBCTypes[i]);
        }
        i++;
    }

    auto *loads = primaryBCTypes[0];
    if (loads) {
        std::array<QStandardItem *, 3> loadGroups;
        i = 0;
        for (const auto &[supLoadType, supLoadName] : supportedLoads) {
            loadGroups[i] = new QStandardItem(supLoadName.c_str());
            for (auto *bc : m_boundaryConditions) {
                if (bc->type() == BoundaryConditionType::Load && dynamic_cast<Load *>(bc)->loadType == supLoadType) {
                    QStandardItem *boundary = new QStandardItem();
                    boundary->setText(!bc->name().empty() ? bc->name().c_str() : "<no name>");
                    loadGroups[i]->setChild(bc->id() - 1, boundary);
                    loadGroups[i]->setChild(bc->id() - 1, 1, new QStandardItem(QString::number(bc->id())));

                    boundary->setData((qulonglong)bc->id(), ID);
                    boundary->setData(static_cast<uint32_t>(bc->type()), BCType);
                    boundary->setData(QString::number(static_cast<uint32_t>(dynamic_cast<Load *>(bc)->loadType)), SpecificType);
                }
            }
            loads->setChild(i, loadGroups[i]);
            i++;
        }
    }

    auto *restraints = primaryBCTypes[1];
    if (restraints) {
        std::array<QStandardItem *, 1> restraintGroups = { nullptr };
        i = 0;
        for (const auto &[supRestraintType, supRestraintName] : supportedRestraints) {
            restraintGroups[i] = new QStandardItem(supRestraintName.c_str());
            for (auto *bc : m_boundaryConditions) {
                if (bc->type() == BoundaryConditionType::Restraint && dynamic_cast<Restraint *>(bc)->restraintType == supRestraintType) {
                    QStandardItem *boundary = new QStandardItem();
                    boundary->setText(!bc->name().empty() ? bc->name().c_str() : "<no name>");

                    boundary->setData((qulonglong)bc->id(), ID);
                    boundary->setData(static_cast<uint32_t>(bc->type()), BCType);
                    boundary->setData(static_cast<uint32_t>(dynamic_cast<Restraint *>(bc)->restraintType), SpecificType);

                    restraintGroups[i]->setChild(bc->id() - 1, boundary);
                    restraintGroups[i]->setChild(bc->id() - 1, 1, new QStandardItem(QString::number(bc->id())));
                }
            }
            restraints->setChild(i, restraintGroups[i]);
            i++;
        }
    }
}

void EditorWindow::selectItem(const QModelIndex &idx) {
    if (!idx.parent().isValid()) { return; }

    auto *item = m_model->itemFromIndex(idx);
    for (const auto *bc : m_boundaryConditions) {
    }
}
