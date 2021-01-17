#pragma once

#include "entities.h"
#include <QDockWidget>
#include <QListWidget>
#include <QTreeWidget>

class ComponentsDock : public QDockWidget {
    Q_OBJECT
public:
    explicit ComponentsDock(Project& project, QWidget* parent = nullptr);

private:
    enum TreeItemType {
        TypeComponent = QTreeWidgetItem::UserType,
        TypeSection,
        TypeEvent,
    };

    enum DataItemType {
        DataEventName = Qt::UserRole,
        DataEventNS,
        DataComponentName,
    };

    void buildUI();

    Project& m_project;
    QListWidget* m_eventListWidget = nullptr;
    QTreeWidget* m_componentTreeWidget = nullptr;
    QAction* m_actionAddEvent = nullptr;
    QAction* m_actionAddSection = nullptr;

private slots:
    void addEvent();
    void addComponent();
    void onSelectionChanged(QTreeWidgetItem* item, int column);
};
