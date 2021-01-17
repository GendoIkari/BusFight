#include "componentsdock.h"
#include "fieldsdialog.h"
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>

ComponentsDock::ComponentsDock(Project& project, QWidget* parent)
    : QDockWidget(parent)
    , m_project(project)
{
    buildUI();
    connect(&m_project, &Project::projectChanged, this, &ComponentsDock::onProjectChanged);
    onProjectChanged();
    onSelectionChanged(nullptr, 0);
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
    connect(m_actionAddSection, &QAction::triggered, this, &ComponentsDock::addComponent);

    layout->addWidget(toolBar);

    layout->addWidget(new QLabel("Components"));

    m_componentTreeWidget = new QTreeWidget(this);
    m_componentTreeWidget->setHeaderHidden(true);
    layout->addWidget(m_componentTreeWidget);
    connect(m_componentTreeWidget, &QTreeWidget::itemChanged, this, &ComponentsDock::onSelectionChanged);

    layout->addWidget(new QLabel("Buses"));

    m_busListWidget = new QListWidget(this);
    layout->addWidget(m_busListWidget);

    layout->addWidget(new QLabel("Events"));

    m_eventListWidget = new QListWidget(this);
    layout->addWidget(m_eventListWidget);
}

void ComponentsDock::addBus()
{
    FieldsDialog dialog("Add Bus");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    m_project.addBus({ .name = name });
}

void ComponentsDock::addEvent()
{
    FieldsDialog dialog("Add Event");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.addField("Time(ns)", FieldsDialog::FieldType::Integer);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    auto ns = dialog.valueAsInt("Time(ns)");
    m_project.addEvent({ .name = name, .timeNS = ns });
}

void ComponentsDock::addComponent()
{
    FieldsDialog dialog("Add Component");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    m_project.addComponent({ .name = name });
}

void ComponentsDock::onProjectChanged()
{
    m_componentTreeWidget->clear();
    m_busListWidget->clear();
    m_eventListWidget->clear();

    for (auto& bus : m_project.buses()) {
        auto item = new QListWidgetItem(m_busListWidget, TypeBus);
        item->setText(bus.name);
        item->setData(DataBusName, bus.name);
        m_busListWidget->addItem(item);
    }

    for (auto& event : m_project.events()) {
        auto item = new QListWidgetItem(m_eventListWidget, TypeEvent);
        item->setText(QString("%1 - %2 ns").arg(event.name).arg(event.timeNS));
        item->setData(DataEventName, event.name);
        item->setData(DataEventNS, event.timeNS);
        m_eventListWidget->addItem(item);
    }

    for (auto& component : m_project.components()) {
        auto item = new QTreeWidgetItem(m_componentTreeWidget, TypeComponent);
        item->setText(0, component.name);
        item->setData(0, DataComponentName, component.name);
        m_componentTreeWidget->addTopLevelItem(item);
    }
}

void ComponentsDock::onSelectionChanged(QTreeWidgetItem* item, int)
{
    if (!item) {
        m_actionAddSection->setEnabled(false);
        return;
    }

    if (item->type() == TypeComponent) {
        m_actionAddSection->setEnabled(true);
    }
}
