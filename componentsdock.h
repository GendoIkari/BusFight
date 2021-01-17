#pragma once

#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>

class ComponentsDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ComponentsDock(QWidget* parent = nullptr);

private:
    void buildUI();

    QListWidget* m_eventListWidget = nullptr;
    QTreeWidget* m_componentTreeWidget = nullptr;

private slots:
    void addEvent();
    void addComponent();
};
