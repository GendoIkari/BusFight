#pragma once

#include "componentsdock.h"
#include "entities.h"
#include "sectionwidget.h"
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Project m_project;
    SectionWidget* m_centralWidget = nullptr;
    ComponentsDock* m_componentsDock = nullptr;

    void buildUI();
    void buildMenus();
    void buildDocks();
    void loadProject();

private slots:
    void onProjectChanged();
};
