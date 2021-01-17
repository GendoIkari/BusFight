#pragma once

#include <QObject>
#include <QString>
#include <QVector>

struct Event {
    QString name;
    int timeNS;
};

struct Component {
    QString name;
};

class Project : public QObject {
    Q_OBJECT
public:
    Project(QObject* parent = nullptr);
    const QVector<Event>& events() const;
    const QVector<Component>& components() const;
    void addEvent(Event event);
    void addComponent(Component component);

private:
    QVector<Event> m_events;
    QVector<Component> m_components;

signals:
    void projectChanged();
};
