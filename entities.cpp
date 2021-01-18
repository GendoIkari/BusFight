#include "entities.h"

#include <QJsonArray>
#include <QJsonObject>

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
    event.uuid = QUuid::createUuid();
    m_events.append(event);
    emit projectChanged();
}

void Project::addComponent(Component component)
{
    component.uuid = QUuid::createUuid();
    m_components.append(component);
    emit projectChanged();
}

void Project::addSection(const QString& busName, Section section)
{
    section.uuid = QUuid::createUuid();
    for (auto& bus : m_buses) {
        if (bus.name == busName)
            bus.sections.append(section);
    }
    emit projectChanged();
}

void Project::addBus(Bus bus)
{
    bus.uuid = QUuid::createUuid();
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

void Project::moveEvent(const QString& name, int ns)
{
    for (auto& event : m_events)
        if (event.name == name)
            event.timeNS = ns;
    emit projectChanged();
}

void Project::removeSection(const QUuid& uuid)
{
    for (auto& bus : m_buses)
        bus.sections.erase(std::remove_if(bus.sections.begin(),
                               bus.sections.end(), [=](Section s) { return s.uuid == uuid; }),
            bus.sections.end());
    emit projectChanged();
}

QPair<int, int> Project::absoluteRange(const Section& section) const
{
    int sectionStart = event(section.referenceStartEvent).timeNS + section.start;
    int sectionEnd = event(section.referenceEndEvent).timeNS + section.end;
    return { sectionStart, sectionEnd };
}

QJsonDocument Project::toJson() const
{

    QJsonArray buses;
    for (auto& bus : m_buses) {
        QJsonObject bObj;
        bObj["uuid"] = bus.uuid.toString();
        bObj["name"] = bus.name;
        QJsonArray sectionsObj;
        for (auto& section : bus.sections) {
            QJsonObject sObj;
            sObj["component"] = section.component;
            sObj["type"] = int(section.type);
            sObj["startRef"] = section.referenceStartEvent;
            sObj["startOffset"] = section.start;
            sObj["endRef"] = section.referenceEndEvent;
            sObj["endOffset"] = section.end;
            sObj["uuid"] = section.uuid.toString();
            sectionsObj.append(sObj);
        }
        bObj["sections"] = sectionsObj;
        buses.append(bObj);
    }

    QJsonArray components;
    for (auto& component : m_components) {
        QJsonObject compObj;
        compObj["uuid"] = component.uuid.toString();
        compObj["name"] = component.name;
        components.append(compObj);
    }

    QJsonArray events;
    for (auto& event : m_events) {
        QJsonObject eventObj;
        eventObj["uuid"] = event.uuid.toString();
        eventObj["name"] = event.name;
        eventObj["ns"] = event.timeNS;
        events.append(eventObj);
    }

    QJsonDocument doc;
    QJsonObject root;
    root["version"] = SERIALIZE_VERSION;
    root["buses"] = buses;
    root["components"] = components;
    root["events"] = events;
    doc.setObject(root);
    return doc;
}

void Project::fromJson(QJsonDocument doc)
{
    auto root = doc.object();
    if (root["version"].toString() != SERIALIZE_VERSION)
        return;

    auto events = root["events"].toArray();
    for (const auto& event : qAsConst(events)) {
        m_events.append(Event {
            .uuid = event.toObject()["uuid"].toString(),
            .name = event.toObject()["name"].toString(),
            .timeNS = event.toObject()["ns"].toInt(),
        });
    }

    auto components = root["components"].toArray();
    for (const auto& component : qAsConst(components)) {
        m_components.append(Component {
            .uuid = component.toObject()["uuid"].toString(),
            .name = component.toObject()["name"].toString(),
        });
    }

    auto buses = root["buses"].toArray();
    for (const auto& bus : qAsConst(buses)) {
        auto busObj = bus.toObject();
        auto busEntity = Bus {
            .uuid = busObj["uuid"].toString(),
            .name = busObj["name"].toString(),
        };
        for (const auto& section : busObj["sections"].toArray()) {
            auto sectionObj = section.toObject();
            auto sectionEntity = Section {
                .uuid = sectionObj["uuid"].toString(),
                .component = sectionObj["component"].toString(),
                .type = Section::SectionType(sectionObj["type"].toInt()),
                .referenceStartEvent = sectionObj["startRef"].toString(),
                .start = sectionObj["startOffset"].toInt(),
                .referenceEndEvent = sectionObj["endRef"].toString(),
                .end = sectionObj["endOffset"].toInt(),
            };
            busEntity.sections.append(sectionEntity);
        }
        m_buses.append(busEntity);
    }

    emit projectChanged();
}
