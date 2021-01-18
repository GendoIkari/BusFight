#pragma once

#include <QJsonDocument>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QString>
#include <QUuid>
#include <QVector>

struct Event {
    QString name;
    int timeNS;
};

struct Component {
    QString name;
};

struct Section {
    enum class SectionType {
        WaitingInTriState = 0,
        ReadingData,
        WritingData,
        WritingGarbage,
    };

    QUuid uuid;
    QString component;
    SectionType type;
    QString referenceStartEvent;
    int start;
    QString referenceEndEvent;
    int end;
};

struct Bus {
    QString name;
    QVector<Section> sections;
};

class Project : public QObject {
    Q_OBJECT
public:
    const QString SERIALIZE_VERSION = "0.0.1";

    Project(QObject* parent = nullptr);
    const Event& event(const QString& name) const;
    const Component& component(const QString& name) const;
    const Bus& bus(const QString& name) const;
    const QVector<Event>& events() const;
    const QVector<Component>& components() const;
    const QVector<Bus>& buses() const;
    void addEvent(Event event);
    void addComponent(Component component);
    void addSection(const QString& busName, Section section);
    void addBus(Bus bus);
    const QSet<QString> busNames() const;
    const QSet<QString> eventNames() const;
    const QSet<QString> componentNames() const;

    void moveEvent(const QString& name, int ns);
    void removeSection(const QUuid& uuid);
    QPair<int, int> absoluteRange(const Section& section) const;

    QJsonDocument toJson() const;
    void fromJson(QJsonDocument doc);

private:
    QVector<Event> m_events;
    QVector<Component> m_components;
    QVector<Bus> m_buses;

signals:
    void projectChanged();
};
