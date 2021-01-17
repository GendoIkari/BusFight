#include "componentsdock.h"
#include "fieldsdialog.h"
#include <QToolBar>
#include <QVBoxLayout>

ComponentsDock::ComponentsDock(Project& project, QWidget* parent)
    : QDockWidget(parent)
    , m_project(project)
{
    buildUI();
    onSelectionChanged(nullptr, 0);
}

void ComponentsDock::buildUI()
{
    setWindowTitle("Components");

    auto subWidget = new QWidget(this);
    setWidget(subWidget);

    auto layout = new QVBoxLayout(subWidget);

    auto toolBar = new QToolBar(this);
    auto actionAddComponent = toolBar->addAction(QIcon(":/assets/processor.svg"), "Add Component");
    actionAddComponent->setToolTip("Add component");
    connect(actionAddComponent, &QAction::triggered, this, &ComponentsDock::addComponent);

    m_actionAddEvent = toolBar->addAction(QIcon(":/assets/event.svg"), "Add Event");
    m_actionAddEvent->setToolTip("Add event");
    connect(m_actionAddEvent, &QAction::triggered, this, &ComponentsDock::addEvent);

    m_actionAddSection = toolBar->addAction(QIcon(":/assets/section.svg"), "Add Section");
    m_actionAddSection->setToolTip("Add section");
    connect(m_actionAddSection, &QAction::triggered, this, &ComponentsDock::addComponent);

    layout->addWidget(toolBar);

    m_componentTreeWidget = new QTreeWidget(this);
    m_componentTreeWidget->setHeaderHidden(true);
    layout->addWidget(m_componentTreeWidget);
    connect(m_componentTreeWidget, &QTreeWidget::itemChanged, this, &ComponentsDock::onSelectionChanged);

    m_eventListWidget = new QListWidget(this);
    layout->addWidget(m_eventListWidget);
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
    auto item = new QListWidgetItem(m_eventListWidget, TypeEvent);
    item->setText(QString("%1 - %2 ns").arg(name).arg(ns));
    item->setData(DataEventName, name);
    item->setData(DataEventNS, ns);
    m_eventListWidget->addItem(item);
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
    auto item = new QTreeWidgetItem(m_componentTreeWidget, TypeComponent);
    item->setText(0, name);
    item->setData(0, DataComponentName, name);
    m_componentTreeWidget->addTopLevelItem(item);
    m_project.addComponent({ .name = name });
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
