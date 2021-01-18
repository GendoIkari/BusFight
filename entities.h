#pragma once

#include <QObject>
#include <QPair>
#include <QSet>
#include <QString>
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
        WaitingInTriState,
        ReadingData,
        WritingData,
        WritingGarbage,
    };

    Component component;
    SectionType type;
    Event referenceStartEvent;
    int start;
    Event referenceEndEvent;
    int end;

    QPair<int, int> absoluteRange() const;
};

struct Bus {
    QString name;
    QVector<Section> sections;
};

class Project : public QObject {
    Q_OBJECT
public:
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

private:
    QVector<Event> m_events;
    QVector<Component> m_components;
    QVector<Bus> m_buses;

signals:
    void projectChanged();
};
