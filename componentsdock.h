#pragma once

#include "entities.h"
#include <QDockWidget>
#include <QListWidget>

class ComponentsDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ComponentsDock(Project &project, QWidget *parent = nullptr);

private:
    enum ItemType
    {
        TypeComponent = QListWidgetItem::ItemType::UserType,
        TypeSection,
        TypeEvent,
        TypeBus,
    };

    enum DataItemType
    {
        DataEventUuid = Qt::UserRole,
        DataComponentUuid,
        DataBusUuid,
    };

    void buildUI();

    Project &m_project;
    QListWidget *m_eventListWidget = nullptr;
    QListWidget *m_busListWidget = nullptr;
    QListWidget *m_componentListWidget = nullptr;
    QAction *m_actionAddEvent = nullptr;
    QAction *m_actionAddSection = nullptr;
    QAction *m_actionAddBus = nullptr;

private slots:
    void addBus();
    void addEvent();
    void addComponent();
    void addSection();
    void onProjectChanged();
    void onBusMenuRequested(const QPointF &point);
    void onEventMenuRequested(const QPointF &point);
    void onComponentMenuRequested(const QPointF &point);
};
