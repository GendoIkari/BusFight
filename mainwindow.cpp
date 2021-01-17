#include "mainwindow.h"

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
