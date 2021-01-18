#include "componentsdock.h"
#include "fieldsdialog.h"
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QVBoxLayout>

ComponentsDock::ComponentsDock(Project& project, QWidget* parent)
    : QDockWidget(parent)
    , m_project(project)
{
    buildUI();
    connect(&m_project, &Project::projectChanged, this, &ComponentsDock::onProjectChanged);
    onProjectChanged();
}

void ComponentsDock::buildUI()
{
    setWindowTitle("Let's Fight!");

    auto subWidget = new QWidget(this);
    setWidget(subWidget);

    auto layout = new QVBoxLayout(subWidget);

    auto toolBar = new QToolBar(this);
    auto actionAddComponent = toolBar->addAction(QIcon(":/assets/processor.svg"), "Add Component");
    actionAddComponent->setToolTip("Add component");
    connect(actionAddComponent, &QAction::triggered, this, &ComponentsDock::addComponent);

    m_actionAddBus = toolBar->addAction(QIcon(":/assets/bus.svg"), "Add Bus");
    m_actionAddBus->setToolTip("Add bus");
    connect(m_actionAddBus, &QAction::triggered, this, &ComponentsDock::addBus);

    m_actionAddEvent = toolBar->addAction(QIcon(":/assets/event.svg"), "Add Event");
    m_actionAddEvent->setToolTip("Add event");
    connect(m_actionAddEvent, &QAction::triggered, this, &ComponentsDock::addEvent);

    m_actionAddSection = toolBar->addAction(QIcon(":/assets/section.svg"), "Add Section");
    m_actionAddSection->setToolTip("Add section");
    connect(m_actionAddSection, &QAction::triggered, this, &ComponentsDock::addSection);

    layout->addWidget(toolBar);

    layout->addWidget(new QLabel("Components"));
    m_componentListWidget = new QListWidget(this);
    m_componentListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_componentListWidget, &QListWidget::customContextMenuRequested, this, &ComponentsDock::onComponentMenuRequested);
    layout->addWidget(m_componentListWidget);

    layout->addWidget(new QLabel("Buses"));
    m_busListWidget = new QListWidget(this);
    m_busListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_busListWidget, &QListWidget::customContextMenuRequested, this, &ComponentsDock::onBusMenuRequested);
    layout->addWidget(m_busListWidget);

    layout->addWidget(new QLabel("Events"));
    m_eventListWidget = new QListWidget(this);
    m_eventListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_eventListWidget, &QListWidget::customContextMenuRequested, this, &ComponentsDock::onEventMenuRequested);
    layout->addWidget(m_eventListWidget);
}

void ComponentsDock::addBus()
{
    FieldsDialog::addBusDialog(m_project);
}

void ComponentsDock::addEvent()
{
    FieldsDialog::addEventDialog(m_project);
}

void ComponentsDock::addComponent()
{
    FieldsDialog::addComponentDialog(m_project);
}

void ComponentsDock::addSection()
{
    FieldsDialog::addSectionDialog(m_project);
}

void ComponentsDock::onProjectChanged()
{
    m_componentListWidget->clear();
    m_busListWidget->clear();
    m_eventListWidget->clear();

    for (auto& bus : m_project.buses()) {
        auto item = new QListWidgetItem(m_busListWidget, TypeBus);
        item->setText(bus.name);
        item->setData(DataBusUuid, bus.uuid);
        m_busListWidget->addItem(item);
    }

    for (auto& event : m_project.events()) {
        auto item = new QListWidgetItem(m_eventListWidget, TypeEvent);
        item->setText(QString("%1 - %2 ns").arg(event.name).arg(event.timeNS));
        item->setData(DataEventUuid, event.uuid);
        m_eventListWidget->addItem(item);
    }

    for (auto& component : m_project.components()) {
        auto item = new QListWidgetItem(m_componentListWidget, TypeComponent);
        item->setText(component.name);
        item->setData(DataComponentUuid, component.uuid);
        m_componentListWidget->addItem(item);
    }

    m_actionAddSection->setEnabled(m_project.events().count() > 0 && m_project.components().count() > 0 && m_project.buses().count() > 0);
}

void ComponentsDock::onBusMenuRequested(const QPointF& point)
{
    QPoint p = m_busListWidget->mapToGlobal(point.toPoint());
    QMenu busMenu;
    busMenu.move(p);

    busMenu.addAction("Add", this, &ComponentsDock::addBus);
    auto editAction = busMenu.addAction("Edit", this, [&] {
        FieldsDialog::editBusDialog(m_project, m_busListWidget->currentItem()->data(DataBusUuid).toString());
    });
    editAction->setEnabled(m_busListWidget->currentItem() != nullptr);
    busMenu.addSeparator();
    auto removeAction = busMenu.addAction("Remove", this, [&] {
        m_project.removeBus(m_busListWidget->currentItem()->data(DataBusUuid).toString());
    });
    removeAction->setEnabled(m_busListWidget->currentItem() != nullptr);
    busMenu.exec();
}

void ComponentsDock::onEventMenuRequested(const QPointF& point)
{
    QPoint p = m_eventListWidget->mapToGlobal(point.toPoint());
    QMenu eventMenu;
    eventMenu.move(p);

    eventMenu.addAction("Add", this, &ComponentsDock::addEvent);
    auto editAction = eventMenu.addAction("Edit", this, [&] {
        FieldsDialog::editEventDialog(m_project, m_eventListWidget->currentItem()->data(DataEventUuid).toString());
    });
    editAction->setEnabled(m_eventListWidget->currentItem() != nullptr);
    eventMenu.addSeparator();
    auto removeAction = eventMenu.addAction("Remove", this, [&] {
        m_project.removeEvent(m_eventListWidget->currentItem()->data(DataEventUuid).toString());
    });
    removeAction->setEnabled(m_eventListWidget->currentItem() != nullptr);
    eventMenu.exec();
}

void ComponentsDock::onComponentMenuRequested(const QPointF& point)
{
    QPoint p = m_componentListWidget->mapToGlobal(point.toPoint());
    QMenu componentMenu;
    componentMenu.move(p);

    componentMenu.addAction("Add", this, &ComponentsDock::addComponent);
    auto editAction = componentMenu.addAction("Edit", this, [&] {
        FieldsDialog::editComponentDialog(m_project, m_componentListWidget->currentItem()->data(DataComponentUuid).toString());
    });
    editAction->setEnabled(m_componentListWidget->currentItem() != nullptr);
    componentMenu.addSeparator();
    auto removeAction = componentMenu.addAction("Remove", this, [&] {
        m_project.removeComponent(m_componentListWidget->currentItem()->data(DataComponentUuid).toString());
    });
    removeAction->setEnabled(m_componentListWidget->currentItem() != nullptr);
    componentMenu.exec();
}
