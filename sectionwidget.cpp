#include "sectionwidget.h"

#include <QPainter>

SectionWidget::SectionWidget(int halfCycleNS, QWidget* parent)
    : QWidget(parent)
    , m_halfCycleNS(halfCycleNS)
    , m_cycleNS(halfCycleNS * 2)
{
}

void SectionWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawHeader(painter);

    auto usedY = 40;
}

void SectionWidget::drawHeader(QPainter& painter)
{
    auto width = geometry().width();
    auto height = geometry().height();
    auto headerHeight = 20;
    auto lineY = m_drawMargins + headerHeight;

    painter.setPen("#2A3950");
    painter.setBrush(QColor("#2A3950"));

    painter.drawLine(m_drawMargins, lineY, width - m_drawMargins, lineY);

    drawTimePoint(0, lineY, painter);
    drawTimePoint(m_halfCycleNS, lineY, painter);
    drawTimePoint(m_cycleNS, lineY, painter);
}

void SectionWidget::drawTimePoint(int ns, int y, QPainter& painter)
{
    auto width = geometry().width();
    auto lineLength = width - m_drawMargins * 2;
    auto positionPerc = (ns + m_cycleMarginNS) / float(m_cycleNS + m_cycleMarginNS * 2);
    auto markerX = int(lineLength * positionPerc);
    auto label = QString("%1 ns").arg(ns);

    QFontMetrics fm(painter.font());
    auto textRect = fm.boundingRect(label);

    painter.setPen("#2A3950");
    painter.drawLine(m_drawMargins + markerX, y - 2, m_drawMargins + markerX, y + 2);
    painter.drawText(m_drawMargins + markerX - textRect.width() / 2, y - 8, label);
}
