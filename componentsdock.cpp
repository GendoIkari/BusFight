#include "componentsdock.h"
#include "fieldsdialog.h"
#include <QToolBar>
#include <QVBoxLayout>

ComponentsDock::ComponentsDock(QWidget* parent)
    : QDockWidget(parent)
{
    buildUI();
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

    auto actionAddEvent = toolBar->addAction(QIcon(":/assets/event.svg"), "Add Event");
    actionAddEvent->setToolTip("Add event");
    connect(actionAddEvent, &QAction::triggered, this, &ComponentsDock::addEvent);

    auto actionAddSection = toolBar->addAction(QIcon(":/assets/section.svg"), "Add Section");
    actionAddSection->setToolTip("Add section");
    connect(actionAddSection, &QAction::triggered, this, &ComponentsDock::addComponent);

    layout->addWidget(toolBar);

    m_componentTreeWidget = new QTreeWidget(this);
    m_componentTreeWidget->setHeaderHidden(true);
    layout->addWidget(m_componentTreeWidget);

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
}

void ComponentsDock::addComponent()
{
    FieldsDialog dialog("Add Component");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto item = new QTreeWidgetItem(m_componentTreeWidget);
    item->setText(0, dialog.valueAsString("Name"));
    m_componentTreeWidget->addTopLevelItem(item);
}
