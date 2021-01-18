#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::buildUI()
{
    setWindowTitle("Bus Fight!");

    m_centralWidget = new SectionWidget(m_project, this);
    setCentralWidget(m_centralWidget);

    buildMenus();
    buildDocks();
}

void MainWindow::buildMenus()
{
    auto projectMenu = new QMenu("Project", this);
    auto saveAction = projectMenu->addAction(
        "&Save", this, [=] {
            saveProject();
        },
        QKeySequence("Ctrl+S"));
    saveAction->setEnabled(!m_projectFileName.isEmpty());
    projectMenu->addAction(
        "Save As", this, [=] {
            auto newPath = QFileDialog::getSaveFileName(this, "Save Project", "", "*.fight");
            if (newPath.isEmpty())
                return;
            m_projectFileName = newPath;
            saveAction->setEnabled(true);
            saveProject();
        },
        QKeySequence("Ctrl+Shift+S"));
    projectMenu->addSeparator();
    projectMenu->addAction(
        "&Open", this, [=] {
            auto newPath = QFileDialog::getOpenFileName(this, "Open Project", "", "*.fight");
            if (newPath.isEmpty())
                return;
            m_projectFileName = newPath;
            saveAction->setEnabled(true);
            loadProject();
        },
        QKeySequence("Ctrl+O"));

    menuBar()->addMenu(projectMenu);
}

void MainWindow::buildDocks()
{
    m_componentsDock = new ComponentsDock(m_project, this);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, m_componentsDock);
}

void MainWindow::saveProject()
{
    auto doc = m_project.toJson();
    QFile saveFile(m_projectFileName);
    saveFile.open(QFile::ReadWrite | QFile::Truncate);
    saveFile.write(doc.toJson());
}

void MainWindow::loadProject()
{
    QFile loadFile(m_projectFileName);
    loadFile.open(QFile::ReadOnly);
    auto doc = QJsonDocument::fromJson(loadFile.readAll());
    m_project.fromJson(doc);
}
