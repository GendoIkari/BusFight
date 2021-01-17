#pragma once

#include <QWidget>

class SectionWidget : public QWidget {
    Q_OBJECT
public:
    explicit SectionWidget(int halfCycleNS, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_halfCycleNS = 250;
    int m_cycleNS = 500;
    int m_cycleMarginNS = 50;
    int m_drawMargins = 10;

    void drawHeader(QPainter& painter);
    void drawTimePoint(int ns, int y, QPainter& painter);
};
