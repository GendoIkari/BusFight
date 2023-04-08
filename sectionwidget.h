#pragma once

#include "entities.h"
#include <QWidget>

class SectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SectionWidget(Project &project, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    const QColor COLOR_TEXT = QColor("#2A3950");
    const QColor COLOR_BKG_LINE = QColor("#7D92B3");
    const QColor COLOR_BKG_GARBAGE = QColor("#E66363");
    const QColor COLOR_BKG_WRITE = QColor("#E6CA63");
    const QColor COLOR_BKG_READ = QColor("#8ED1A4");
    const QColor COLOR_BKG_BUS = QColor("#E8EDD3");
    const int MARGINS = 10;
    const int HEADER_LINE_Y = MARGINS + 25;
    const int EVENT_NAME_Y = HEADER_LINE_Y + 25;
    const int SECTION_BOX_HEIGHT = 20;
    const int SECTION_SPACING = 5;

    int m_nsMargin = 50;
    QVector<QPair<QRect, Section>> m_sectionsRects;
    Project &m_project;

    QPair<int, int> timeWindow();
    int xFromNS(int ns);
    void buildUI();
    void drawHeader(QPainter &painter);
    void drawEvents(QPainter &painter);
    void drawSections(QPainter &painter);
    void drawSection(QPainter &painter, const Section &section, QRect rect);
    void drawTimePoint(int ns, QPainter &painter);

private slots:
    void onProjectChanged();
    void onContextMenuRequested(QPointF point);
};
