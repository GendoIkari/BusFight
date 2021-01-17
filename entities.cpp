#include "entities.h"

Project::Project(QObject* parent)
    : QObject(parent)
{
}

const QVector<Event>& Project::events() const
{
    return m_events;
}

const QVector<Component>& Project::components() const
{
    return m_components;
}

void Project::addEvent(Event event)
{
    m_events.append(event);
    emit projectChanged();
}

void Project::addComponent(Component component)
{
    m_components.append(component);
    emit projectChanged();
}
