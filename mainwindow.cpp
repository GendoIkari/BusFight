#include "mainwindow.h"

#include <QFile>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUI();
    connect(&m_project, &Project::projectChanged, this, &MainWindow::onProjectChanged);
    loadProject();
}

MainWindow::~MainWindow()
{
}

void MainWindow::buildUI()
{
    setWindowTitle("Bus Fight!");

    m_centralWidget = new SectionWidget(m_project, this);
    setCentralWidget(m_centralWidget);
    m_componentsDock = new ComponentsDock(m_project, this);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_componentsDock);

    buildMenus();
    buildDocks();
}

void MainWindow::buildMenus()
{
}

void MainWindow::buildDocks()
{
}

void MainWindow::loadProject()
{
    QFile loadFile("project.fight");
    loadFile.open(QFile::ReadOnly);
    auto doc = QJsonDocument::fromJson(loadFile.readAll());
    m_project.fromJson(doc);
}

void MainWindow::onProjectChanged()
{
    auto doc = m_project.toJson();
    QFile saveFile("project.fight");
    saveFile.open(QFile::ReadWrite | QFile::Truncate);
    saveFile.write(doc.toJson());
}
