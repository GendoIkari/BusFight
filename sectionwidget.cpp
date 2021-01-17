#include "sectionwidget.h"

#include <QPainter>

SectionWidget::SectionWidget(Project& project, QWidget* parent)
    : QWidget(parent)
    , m_project(project)
{
    connect(&project, &Project::projectChanged, this, &SectionWidget::onProjectChanged);
    onProjectChanged();
}

void SectionWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawHeader(painter);
    drawEvents(painter);
}

QPair<int, int> SectionWidget::timeWindow()
{
    int min = 0;
    int max = 0;

    for (auto& event : m_project.events()) {
        if (event.timeNS < min)
            min = event.timeNS;
        if (event.timeNS > max)
            max = event.timeNS;
    }

    return { min - m_nsMargin, max + m_nsMargin };
}

int SectionWidget::xFromNS(int ns)
{
    auto range = timeWindow();
    auto timeMin = range.first;
    auto timeMax = range.second;
    auto timeLength = timeMax - timeMin;
    Q_ASSERT(ns >= timeMin && ns <= timeMax);

    auto width = geometry().width();
    auto lineLength = width - MARGINS * 2;
    auto positionPerc = (ns - timeMin) / float(timeLength);
    return int(lineLength * positionPerc);
}

void SectionWidget::drawHeader(QPainter& painter)
{
    auto width = geometry().width();

    painter.setPen(COLOR_TEXT);
    painter.drawLine(MARGINS, HEADER_LINE_Y, width - MARGINS, HEADER_LINE_Y);

    drawTimePoint(0, painter);
}

void SectionWidget::drawEvents(QPainter& painter)
{
    auto bottomY = geometry().bottom() - MARGINS;

    for (auto& event : m_project.events()) {
        drawTimePoint(event.timeNS, painter);

        QFontMetrics fm(painter.font());
        auto textRect = fm.boundingRect(event.name);
        auto eventX = MARGINS + xFromNS(event.timeNS);

        painter.setPen(COLOR_TEXT);
        painter.drawText(eventX - textRect.width() / 2, EVENT_NAME_Y - 8, event.name);
        painter.setPen(COLOR_BKG_LINE);
        painter.drawLine(eventX, EVENT_NAME_Y, eventX, bottomY);
    }
}

void SectionWidget::drawTimePoint(int ns, QPainter& painter)
{
    auto markerX = MARGINS + xFromNS(ns);
    auto label = QString("%1 ns").arg(ns);

    QFontMetrics fm(painter.font());
    auto textRect = fm.boundingRect(label);

    painter.setPen(COLOR_TEXT);
    painter.drawText(markerX - textRect.width() / 2, HEADER_LINE_Y - 8, label);
    painter.drawLine(markerX, HEADER_LINE_Y - 2, markerX, HEADER_LINE_Y + 2);
}

void SectionWidget::onProjectChanged()
{
    update();
}
