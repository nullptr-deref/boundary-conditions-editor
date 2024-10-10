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

    m_treeView = new QTreeView();
    m_treeView->setModel(m_model);

    QHBoxLayout *upperLayout = new QHBoxLayout;
    upperLayout->addWidget(m_treeView);
    upperLayout->addWidget(m_settings);
    upperLayout->setStretchFactor(m_treeView, 10);
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
    connect(m_recalculateButton, &QPushButton::pressed, this, &EditorWindow::recalculateProject);
    connect(this, &EditorWindow::boundaryConditionsParsed, this, &EditorWindow::updateCounter);
    connect(m_treeView, &QTreeView::clicked, this, &EditorWindow::selectItem);

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
        std::clog << "Opening \"" << m_filename << "\"...\n";
        m_inputFilestream = std::ifstream(m_filename);

        if (m_inputFilestream.good()) {
            try {
                loadParsedData();
                updateTreeModel();

                emit fileOpened(m_filename);
            }
            catch (const json::exception &e) {
                std::clog << e.what() << '\n';
                QMessageBox alert;
                alert.setText(tr("The selected file is of unsupported format, ill-formed or contains unsupported data. Try opening another file."));
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

    json loads = json::array();
    for (auto &force : m_forces) {
        loads.push_back(force.serialize());
    }
    for (auto &pressure : m_pressures) {
        loads.push_back(pressure.serialize());
    }

    json restraints = json::array();
    for (const auto &displ : m_displacements) {
        restraints.push_back(displ.serialize());
    }

    contentsCopy["loads"] = loads;
    contentsCopy["restraints"] = restraints;

    const std::string exportedFileName = QFileDialog::getSaveFileName(
        this,
        tr("Open file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Fidesys Case Files (*.fc);;All files (*.*)")
    ).toStdString();

    if (!exportedFileName.empty()) {
        std::ofstream outputFileStream(exportedFileName);
        const auto prettied = contentsCopy.dump(4);
        outputFileStream << prettied;
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

void EditorWindow::updateCounter(size_t bcCount) {
    if (bcCount == 0) {
        if (m_filename.empty())
            m_bcCounter->setText(tr("No file opened."));
        else m_bcCounter->setText(tr("No supported boundaries found in the file."));
    }
    else {
        m_bcCounter->setText(tr("Number of detected boundary conditions: %1").arg(bcCount));
    }
}

void EditorWindow::updateTreeModel() {
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList() << "Boundary condition" << "ID" << "Type");

    // for each array:
    //   create QStandardItem for BC section
    //   append it to model
    //   for each element in array:
    //     create QStandardItem
    //     set text for QStandardItem
    //     set QStandardItem as a child of the array's QStandardItem
    //     set child which holds id
    //     set data for the item:
    //       - type
    //       - id

    QStandardItem *loadsItem = new QStandardItem("Loads");
    QStandardItem *restraintsItem = new QStandardItem("Restraints");
    m_model->appendRow(loadsItem);
    m_model->appendRow(restraintsItem);

    QStandardItem *forcesItem = new QStandardItem("Forces");
    QStandardItem *pressuresItem = new QStandardItem("Pressures");

    loadsItem->setChild(0, forcesItem);
    loadsItem->setChild(1, pressuresItem);

    QStandardItem *displacementsItem = new QStandardItem("Displacements");

    restraintsItem->setChild(0, displacementsItem);

    for (const auto &force: m_forces) {
        QStandardItem *fItem = new QStandardItem(force.name.c_str());
        forcesItem->setChild(force.id - 1, 0, fItem);
        forcesItem->setChild(force.id - 1, 1, new QStandardItem(QString::number(force.id)));
        forcesItem->setChild(force.id - 1, 2, new QStandardItem(QString::number(static_cast<uint32_t>(force.type))));
        fItem->setData(static_cast<uint32_t>(force.id), static_cast<int>(ItemRole::ID));
        fItem->setData(static_cast<uint32_t>(force.type), static_cast<int>(ItemRole::Type));
        fItem->setData(static_cast<uint32_t>(BCType::Load), static_cast<int>(ItemRole::BType));
    }

    for (const auto &pressure: m_pressures) {
        QStandardItem *fItem = new QStandardItem(pressure.name.c_str());
        forcesItem->setChild(pressure.id - 1, 0, fItem);
        forcesItem->setChild(pressure.id - 1, 1, new QStandardItem(QString::number(pressure.id)));
        forcesItem->setChild(pressure.id - 1, 2, new QStandardItem(QString::number(static_cast<uint32_t>(pressure.type))));
        fItem->setData(static_cast<uint32_t>(pressure.id), static_cast<int>(ItemRole::ID));
        fItem->setData(static_cast<uint32_t>(pressure.type), static_cast<int>(ItemRole::Type));
        fItem->setData(static_cast<uint32_t>(BCType::Pressure), static_cast<int>(ItemRole::BType));
    }
    /* TODO: add pressures & displacements handling */
}

void EditorWindow::selectItem(const QModelIndex &idx) {
    if (!idx.parent().isValid()) { return; }

    auto *item = m_model->itemFromIndex(idx);
    switch(static_cast<uint32_t>(item->data(static_cast<int>(ItemRole::BType)).toInt())) {
        case static_cast<uint32_t>(BCType::Load): {
            bc::ProjectionVector v;
            const auto t = static_cast<bc::LoadType>(item->data(static_cast<int>(ItemRole::Type)).toInt());
            const auto id = static_cast<size_t>(item->data(static_cast<int>(ItemRole::ID)).toInt());
            for (auto &el : m_forces) {
                if (el.type == t && el.id == id) {
                    v = el.data;
                    std::clog
                        << "Selected item: { name = " << el.name
                        << ", id = " << id
                        << ", type = " << static_cast<uint32_t>(t) << ", data = [ ";
                    for (const auto &vel : v) {
                        std::clog << vel << " ";
                    }
                    std::clog << "] }\n";

                    //sendDataToSettingsWidget<bc::ProjectionVector>(v);
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Pressure): {
            double magnitude;
            const auto t = static_cast<bc::PressureType>(item->data(static_cast<int>(ItemRole::Type)).toInt());
            const auto id = static_cast<size_t>(item->data(static_cast<int>(ItemRole::ID)).toInt());
            for (auto &el : m_pressures) {
                if (el.type == t && el.id == id) {
                    magnitude = el.data;
                    std::clog
                        << "Selected item: { name = " << el.name
                        << ", id = " << id
                        << ", type = " << static_cast<uint32_t>(t)
                        << ", data = " << magnitude
                        << " }\n";

                    //sendDataToSettingsWidget<bc::ProjectionVector>(v);
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Displacement): {

        } break;
    }

    /* TODO: rewrite
    auto *item = m_model->itemFromIndex(idx);
    for (const auto *bc : m_boundaryConditions) {
    }
    */
}

void EditorWindow::loadParsedData() {
    m_fileContents = json::parse(m_inputFilestream);
    bc::BoundaryConditionsParser parser(m_fileContents);
    clearBoundaries();

    m_forces = parser.parse<bc::Load, bc::ParsingOrigin::LoadsArray>();
    if (!m_forces.empty()) {
        std::clog << "forces: {\n";
        for (const auto &force : m_forces) {
            std::clog << force << ",\n";
        }
        std::clog << "}\n";
    }

    m_pressures = parser.parse<bc::Pressure, bc::ParsingOrigin::LoadsArray>();
    if (!m_pressures.empty()) {
        std::clog << "pressures: {\n";
        for (const auto &pr : m_pressures) {
            std::clog << pr << ",\n";
        }
        std::clog << "}\n";
    }

    m_displacements = parser.parse<bc::Restraint, bc::ParsingOrigin::RestraintsArray>();
    if (!m_displacements.empty()) {
        std::clog << "displacements: {\n";
        for (const auto &d : m_displacements) {
            std::clog << d << ",\n";
        }
        std::clog << "}\n";
    }

    m_fileCurrentlyOpened = true;

    emit boundaryConditionsParsed(m_forces.size() + m_pressures.size() + m_displacements.size());
}

void EditorWindow::clearBoundaries() {
    m_forces.clear();
    m_pressures.clear();
    m_displacements.clear();
}
