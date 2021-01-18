#include "entities.h"

Project::Project(QObject* parent)
    : QObject(parent)
{
}

const Event& Project::event(const QString& name) const
{
    for (auto& event : m_events)
        if (event.name == name)
            return event;
    Q_ASSERT(false);
}

const Component& Project::component(const QString& name) const
{
    for (auto& component : m_components)
        if (component.name == name)
            return component;
    Q_ASSERT(false);
}

const Bus& Project::bus(const QString& name) const
{
    for (auto& bus : m_buses)
        if (bus.name == name)
            return bus;
    Q_ASSERT(false);
}

const QVector<Event>& Project::events() const
{
    return m_events;
}

const QVector<Component>& Project::components() const
{
    return m_components;
}

const QVector<Bus>& Project::buses() const
{
    return m_buses;
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

void Project::addSection(const QString& busName, Section section)
{
    for (auto& bus : m_buses) {
        if (bus.name == busName)
            bus.sections.append(section);
    }

    emit projectChanged();
}

void Project::addBus(Bus bus)
{
    m_buses.append(bus);
    emit projectChanged();
}

const QSet<QString> Project::busNames() const
{
    QSet<QString> names;
    for (auto& bus : m_buses)
        names.insert(bus.name);
    return names;
}

const QSet<QString> Project::eventNames() const
{
    QSet<QString> names;
    for (auto& event : m_events)
        names.insert(event.name);
    return names;
}

const QSet<QString> Project::componentNames() const
{
    QSet<QString> names;
    for (auto& component : m_components)
        names.insert(component.name);
    return names;
}

QPair<int, int> Section::absoluteRange() const
{
    int sectionStart = referenceStartEvent.timeNS + start;
    int sectionEnd = referenceEndEvent.timeNS + end;
    return { sectionStart, sectionEnd };
}
