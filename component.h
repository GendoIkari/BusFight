#pragma once

#include <QVector>

struct Component {
    enum class Operation {
        Read,
        Write,
        OccupiedWithGarbage,
    };

    struct Section {
        QString name;
        QString bus;
        QString eventReference;
        int durationNS;
    };

    QString name;
    QVector<Section> sections;
};
