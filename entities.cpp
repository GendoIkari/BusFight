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

QPair<int, int> Section::absoluteRange() const
{
    int sectionStart = referenceStartEvent.timeNS + start;
    int sectionEnd = referenceEndEvent.timeNS + end;
    return { sectionStart, sectionEnd };
}
