#include "../include/EditorWindow.hpp"

#include "../include/bc/Restraint.hpp"
#include "../include/bc/Load.hpp"
#include "../include/bc/BoundaryConditionsParser.hpp"

#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QHeaderView>
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
    m_settings = new QGroupBox("Boundary condition settings", centralWidget);
    m_settings->setLayout(new QVBoxLayout);

    m_treeView = new QTreeView();
    m_treeView->setModel(m_model);
    m_treeView->header()->setSectionResizeMode(QHeaderView::Stretch);
    m_treeView->header()->setCascadingSectionResizes(true);

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

    resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
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

// TODO: implement
void EditorWindow::closeFile() {
    if (m_inputFilestream.is_open()) {
        m_inputFilestream.close();
        m_filename.clear();
    }
}

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

// TODO: implement
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
        QStandardItem *item = new QStandardItem(!force.name.empty() ? force.name.c_str() : "<no name>");
        forcesItem->setChild(force.id - 1, 0, item);
        forcesItem->setChild(force.id - 1, 1, new QStandardItem(QString::number(force.id)));
        forcesItem->setChild(force.id - 1, 2, new QStandardItem(QString::number(static_cast<uint32_t>(force.type))));
        item->setData(static_cast<uint32_t>(force.id), static_cast<int>(ItemRole::ID));
        item->setData(static_cast<uint32_t>(force.type), static_cast<int>(ItemRole::Type));
        item->setData(static_cast<uint32_t>(BCType::Load), static_cast<int>(ItemRole::BType));
    }

    for (const auto &pressure: m_pressures) {
        QStandardItem *item = new QStandardItem(!pressure.name.empty() ? pressure.name.c_str() : "<no name>");
        pressuresItem->setChild(pressure.id - 1, 0, item);
        pressuresItem->setChild(pressure.id - 1, 1, new QStandardItem(QString::number(pressure.id)));
        pressuresItem->setChild(pressure.id - 1, 2, new QStandardItem(QString::number(static_cast<uint32_t>(pressure.type))));
        item->setData(static_cast<uint32_t>(pressure.id), static_cast<int>(ItemRole::ID));
        item->setData(static_cast<uint32_t>(pressure.type), static_cast<int>(ItemRole::Type));
        item->setData(static_cast<uint32_t>(BCType::Pressure), static_cast<int>(ItemRole::BType));
    }

    for (const auto &displ: m_displacements) {
        QStandardItem *item = new QStandardItem(!displ.name.empty() ? displ.name.c_str() : "<no name>");
        displacementsItem->setChild(displ.id - 1, 0, item);
        displacementsItem->setChild(displ.id - 1, 1, new QStandardItem(QString::number(displ.id)));
        displacementsItem->setChild(displ.id - 1, 2, new QStandardItem(QString::number(static_cast<uint32_t>(displ.type))));
        item->setData(static_cast<uint32_t>(displ.id), static_cast<int>(ItemRole::ID));
        item->setData(static_cast<uint32_t>(displ.type), static_cast<int>(ItemRole::Type));
        item->setData(static_cast<uint32_t>(BCType::Displacement), static_cast<int>(ItemRole::BType));
    }
}

void EditorWindow::selectItem(const QModelIndex &idx) {
    if (!idx.parent().isValid()) { return; }

    auto *item = m_model->itemFromIndex(idx);
    switch(static_cast<uint32_t>(item->data(static_cast<int>(ItemRole::BType)).toInt())) {
        case static_cast<uint32_t>(BCType::Load): {
            const auto t = static_cast<bc::LoadType>(item->data(static_cast<int>(ItemRole::Type)).toInt());
            const auto id = static_cast<size_t>(item->data(static_cast<int>(ItemRole::ID)).toInt());
            for (auto &el : m_forces) {
                if (el.type == t && el.id == id) {
                    std::clog << "Selected item: " << el << '\n';
                    m_selectedItem = Selection {
                        .id = id,
                        .type = static_cast<uint32_t>(t),
                        .btype = static_cast<BCType>(item->data(static_cast<int>(ItemRole::BType)).toInt())
                    };
                    constructSettingsWidget<bc::ProjectionVector>(el.data);
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Pressure): {
            const auto t = static_cast<bc::PressureType>(item->data(static_cast<int>(ItemRole::Type)).toInt());
            const auto id = static_cast<size_t>(item->data(static_cast<int>(ItemRole::ID)).toInt());
            for (auto &el : m_pressures) {
                if (el.type == t && el.id == id) {
                    std::clog << "Selected item: " << el << '\n';
                    m_selectedItem = Selection {
                        .id = id,
                        .type = static_cast<uint32_t>(t),
                        .btype = static_cast<BCType>(item->data(static_cast<int>(ItemRole::BType)).toInt())
                    };
                    constructSettingsWidget<double>(el.data);
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Displacement): {
            const auto t = static_cast<bc::RestraintType>(item->data(static_cast<int>(ItemRole::Type)).toInt());
            const auto id = static_cast<size_t>(item->data(static_cast<int>(ItemRole::ID)).toInt());
            for (auto &el : m_displacements) {
                if (el.type == t && el.id == id) {
                    std::pair<bc::ProjectionVector, bc::RestraintFlags> vs = { el.data, el.flags };
                    std::clog << "Selected item: " << el << '\n';
                    m_selectedItem = Selection {
                        .id = id,
                        .type = static_cast<uint32_t>(t),
                        .btype = static_cast<BCType>(item->data(static_cast<int>(ItemRole::BType)).toInt())
                    };
                    constructSettingsWidget<std::pair<bc::ProjectionVector, bc::RestraintFlags>>(vs);
                }
            }
        } break;
    }
}

void EditorWindow::loadParsedData() {
    m_fileContents = json::parse(m_inputFilestream);
    bc::BoundaryConditionsParser parser(m_fileContents);
    clearBoundaries();

    m_forces = parser.parse<bc::Load, bc::ParsingOrigin::LoadsArray>();
    if (!m_forces.empty()) {
        std::clog << "forces: {\n";
        for (const auto &force : m_forces) {
            std::clog << "\t" << force << ",\n";
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

void EditorWindow::writeChangesToSelectedItem(const std::optional<std::string> &optS, const std::optional<Qt::CheckState> &optCs) {
    switch(static_cast<uint32_t>(m_selectedItem.btype)) {
        case static_cast<uint32_t>(BCType::Load): {
            for (auto &f : m_forces) {
                if (f.id == m_selectedItem.id &&
                    static_cast<uint32_t>(f.type) == m_selectedItem.type
                ) {
                    std::clog << "Updating data for the: force = " << f;
                    if (optS &&
                        std::any_of(optS.value().cbegin(),
                            optS.value().cend(),
                            [](char c) { return std::isdigit(c); })) {
                        for (size_t i = 0; i < 6; i++) {
                            f.data[i] = std::stod(m_editors[i]->text().toStdString());
                        }
                    }
                    std::clog << " -> " << f << '\n';
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Pressure): {
            for (auto &p : m_pressures) {
                if (p.id == m_selectedItem.id &&
                    static_cast<uint32_t>(p.type) == m_selectedItem.type
                ) {
                    std::clog << "Updating data for the: pressure = " << p << '\n';
                    if (optS &&
                        std::any_of(optS.value().cbegin(),
                            optS.value().cend(),
                            [](char c) { return std::isdigit(c); })) {
                        p.data = std::stod(m_editors[0]->text().toStdString());
                    }
                    std::clog << " -> " << p << '\n';
                }
            }
        } break;
        case static_cast<uint32_t>(BCType::Displacement): {
            for (auto &d : m_displacements) {
                if (d.id == m_selectedItem.id &&
                    static_cast<uint32_t>(d.type) == m_selectedItem.type
                ) {
                    std::clog << "Updating data for the: displacement = " << d << '\n';
                    if (optS &&
                        std::any_of(optS.value().cbegin(),
                            optS.value().cend(),
                            [](char c) { return std::isdigit(c); })) {
                        for (size_t i = 0; i < 6; i++) {
                            d.data[i] = std::stod(m_editors[i]->text().toStdString());
                        }
                    }
                    if (optCs) {
                        for (size_t i = 0; i < 6; i++) {
                            d.flags[i] = m_checks[i]->checkState() == Qt::Checked
                                ? static_cast<uint32_t>(d.type)
                                : 0;
                        }
                    }
                    std::clog << " -> " << d << '\n';
                }
            }
        } break;
    }
}

void EditorWindow::constructForcesWidget() {
    m_forcesSettingsWidget = new QWidget();
    QVBoxLayout *l = new QVBoxLayout();
    QGridLayout *grid = new QGridLayout();

    grid->addWidget(new QLabel("Force linear magnitude"), 1, 0);
    grid->addWidget(new QLabel("Moment"), 2, 0);

    if (std::any_of(m_editors.cbegin(), m_editors.cend(), [](auto *e) { return e == nullptr; })) {
        initializeSettingsEditors();
    }

    const std::array<QString, 3> labels = { "X", "Y", "Z" };
    size_t i = 0;
    for (const auto &label : labels) {
        grid->addWidget(new QLabel(label), 0, i + 1);
        grid->addWidget(m_editors[i], 1, i + 1); // linear component
        grid->addWidget(m_editors[i + 3], 2, i + 1); // rotary component
        i++;
    }
    grid->addWidget(new QLabel(tr("mm")), 1, i + 1);
    grid->addWidget(new QLabel(tr("Nm")), 2, i + 1);

    l->addLayout(grid);
    l->addStretch();
    m_forcesSettingsWidget->setLayout(l);
}

void EditorWindow::constructPressuresWidget() {
    m_pressuresSettingsWidget = new QWidget();
    QVBoxLayout *l = new QVBoxLayout();
    QGridLayout *grid = new QGridLayout();

    grid->addWidget(new QLabel("Pressure magnitude"), 0, 0);

    if (std::any_of(m_editors.cbegin(), m_editors.cend(), [](auto *e) { return e == nullptr; })) {
        initializeSettingsEditors();
    }

    grid->addWidget(m_editors[0], 0, 1); // linear component

    l->addLayout(grid);
    l->addStretch();
    m_pressuresSettingsWidget->setLayout(l);
}
void EditorWindow::constructDisplacementsWidget() {
    m_displacementsSettingsWidget = new QWidget();
    QVBoxLayout *l = new QVBoxLayout();
    QGridLayout *grid = new QGridLayout();

    grid->addWidget(new QLabel("Linear"), 1, 0);
    grid->addWidget(new QLabel("Rotary"), 2, 0);

    if (std::any_of(m_editors.cbegin(), m_editors.cend(), [](auto *e) { return e == nullptr; })) {
        initializeSettingsEditors();
    }
    if (std::any_of(m_checks.cbegin(), m_checks.cend(), [](auto *e) { return e == nullptr; })) {
        initializeSettingsCheckboxes();
    }

    const std::array<QString, 3> labels = { "X", "Y", "Z" };
    size_t i = 0;
    for (const auto &label : labels) {
        QHBoxLayout *l1 = new QHBoxLayout;
        l1->addWidget(m_editors[i]);
        l1->addWidget(new QLabel(tr("mm")));
        l1->addSpacing(8);
        l1->addWidget(m_checks[i]);

        QHBoxLayout *l2 = new QHBoxLayout;
        l2->addWidget(m_editors[i + 3]);
        l2->addWidget(new QLabel(tr("deg")));
        l2->addSpacing(8);
        l2->addWidget(m_checks[i + 3]);

        grid->addWidget(new QLabel(label), 0, i + 1);
        grid->addLayout(l1, 1, i + 1); // linear component
        grid->addLayout(l2, 2, i + 1); // rotary component
        i++;
    }

    l->addLayout(grid);
    l->addStretch();
    m_displacementsSettingsWidget->setLayout(l);
}

void EditorWindow::initializeSettingsEditors() {
    for (size_t i = 0; i < m_editors.size(); i++) {
        m_editors[i] = new QLineEdit();
        m_editors[i]->setValidator(new QDoubleValidator);
        connect(
            m_editors[i],
            &QLineEdit::textEdited,
            this,
            [&](const QString &s) {
                writeChangesToSelectedItem(std::make_optional(s.toStdString()), std::optional<Qt::CheckState>());
            }
        );
    }
}

void EditorWindow::initializeSettingsCheckboxes() {
    for (size_t i = 0; i < m_checks.size(); i++) {
        m_checks[i] = new QCheckBox();
        connect(
            m_checks[i],
            &QCheckBox::checkStateChanged,
            this,
            [&](Qt::CheckState cs) {
                writeChangesToSelectedItem(std::optional<std::string>(), std::make_optional(cs));
            }
        );
    }
}
