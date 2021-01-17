#pragma once

#include "entities.h"
#include <QWidget>

class SectionWidget : public QWidget {
    Q_OBJECT
public:
    explicit SectionWidget(Project& project, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const QColor COLOR_TEXT = QColor("#2A3950");
    const QColor COLOR_BKG_LINE = QColor("#7d92b3");
    const int MARGINS = 10;
    const int HEADER_LINE_Y = MARGINS + 25;
    const int EVENT_NAME_Y = HEADER_LINE_Y + 25;

    int m_nsMargin = 50;
    Project& m_project;

    QPair<int, int> timeWindow();
    int xFromNS(int ns);
    void drawHeader(QPainter& painter);
    void drawEvents(QPainter& painter);
    void drawTimePoint(int ns, QPainter& painter);

private slots:
    void onProjectChanged();
};
