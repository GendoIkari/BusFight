#pragma once

#include <QJsonDocument>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QVector>

struct Event
{
    QUuid uuid;
    QString name;
    int timeNS;
};

struct Component
{
    QUuid uuid;
    QString name;
};

struct Section
{
    enum class SectionType
    {
        WaitingInTriState = 0,
        ReadingData,
        WritingData,
        WritingGarbage,
        SignalHigh,
        SignalLow,
        SignalTriState,
        SignalUnknown,
    };

    QUuid uuid;
    QUuid component;
    SectionType type;
    QUuid referenceStartEvent;
    int start;
    QUuid referenceEndEvent;
    int end;
};

struct Bus
{
    enum class BusType
    {
        Parallel = 0,
        Signal,
    };

    QUuid uuid;
    BusType type;
    QString name;
    QVector<Section> sections;
};

class Project : public QObject
{
    Q_OBJECT
public:
    const QString SERIALIZE_VERSION = "0.0.2";

    Project(QObject *parent = nullptr);
    const Event &event(const QUuid &uuid) const;
    const Component &component(const QUuid &uuid) const;
    const Bus &bus(const QUuid &uuid) const;
    const Bus &bus(const Section &section) const;
    const Section &section(const QUuid &uuid) const;
    const QVector<Event> &events() const;
    const QVector<Component> &components() const;
    const QVector<Bus> &buses() const;
    void addEvent(Event event);
    void addComponent(Component component);
    void addSection(const QUuid &busUuid, Section section);
    void addBus(Bus bus);
    void editEvent(const QUuid &uuid, const QString &name, int ns);
    void editBus(const QUuid &uuid, const QString &name);
    void editComponent(const QUuid &uuid, const QString &name);
    void editSection(const QUuid &uuid, const QUuid &component, Section::SectionType type, const QUuid &startEvent, int start, const QUuid &endEvent, int end);
    void removeEvent(const QUuid &uuid);
    void removeBus(const QUuid &uuid);
    void removeSection(const QUuid &uuid);
    void removeComponent(const QUuid &uuid);
    QPair<int, int> absoluteRange(const Section &section) const;
    QPair<int, int> absoluteRange(const QUuid &startEvent, int startOffset, const QUuid &endEvent, int endOffset) const;

    QJsonDocument toJson() const;
    void fromJson(QJsonDocument doc);

private:
    QVector<Event> m_events;
    QVector<Component> m_components;
    QVector<Bus> m_buses;

signals:
    void projectChanged();
};
