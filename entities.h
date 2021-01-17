#pragma once

#include <QObject>
#include <QPair>
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
    QString bus;
    Event referenceStartEvent;
    int start;
    Event referenceEndEvent;
    int end;

    QPair<int, int> absoluteRange() const;
};

class Project : public QObject {
    Q_OBJECT
public:
    Project(QObject* parent = nullptr);
    const QVector<Event>& events() const;
    const QVector<Component>& components() const;
    const QVector<Section>& sections() const;
    void addEvent(Event event);
    void addComponent(Component component);
    void addSection(Section section);

private:
    QVector<Event> m_events;
    QVector<Component> m_components;
    QVector<Section> m_sections;

signals:
    void projectChanged();
};
