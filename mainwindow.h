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
    QString m_projectFileName;
    SectionWidget* m_centralWidget = nullptr;
    ComponentsDock* m_componentsDock = nullptr;
    bool m_dirty = false;

    void buildUI();
    void buildMenus();
    void buildDocks();
    void saveProject();
    void loadProject();
    void updateTitle();

private slots:
    void onProjectChanged();
};
