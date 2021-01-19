#include "mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

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
            QString filter = "BusFight Project (*.fight)";
            auto dialog = new QFileDialog(this, "Save Project", ".", filter);
            dialog->setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
            dialog->setFileMode(QFileDialog::FileMode::AnyFile);
            dialog->setDefaultSuffix("fight");
            dialog->setOption(QFileDialog::Option::DontConfirmOverwrite);
            dialog->exec();
            if (dialog->selectedFiles().isEmpty())
                return;
            auto newPath = dialog->selectedFiles().first();
            // We need to do that because Qt QFileDialog is bugged with defaultSuffix.
            if (QFile(newPath).exists()) {
                QMessageBox msgBox;
                msgBox.setText("Do you want to overwrite the file?");
                msgBox.setInformativeText(newPath);
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
                msgBox.setDefaultButton(QMessageBox::Save);
                int choice = msgBox.exec();
                if (choice == QMessageBox::Discard)
                    return;
            }

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
