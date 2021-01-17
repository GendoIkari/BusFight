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

const QVector<Section>& Project::sections() const
{
    return m_sections;
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

void Project::addSection(Section section)
{
    m_sections.append(section);
    emit projectChanged();
}

QPair<int, int> Section::absoluteRange() const
{
    int sectionStart = referenceStartEvent.timeNS + start;
    int sectionEnd = referenceEndEvent.timeNS + end;
    return { sectionStart, sectionEnd };
}
