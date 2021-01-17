#include "sectionwidget.h"

#include <QPainter>
#include <QSet>

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
    drawSections(painter);
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

    for (auto& section : m_project.sections()) {
        int sectionStart = section.absoluteRange().first;
        int sectionEnd = section.absoluteRange().second;
        Q_ASSERT(sectionEnd > sectionStart);

        if (sectionStart < min)
            min = sectionStart;
        if (sectionEnd > max)
            max = sectionEnd;
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

    QSet<int> timePoints;

    for (auto& event : m_project.events())
        timePoints.insert(event.timeNS);
    for (auto& section : m_project.sections()) {
        auto range = section.absoluteRange();
        timePoints.insert(range.first);
        timePoints.insert(range.second);
    }

    for (auto t : timePoints)
        drawTimePoint(t, painter);
}

void SectionWidget::drawEvents(QPainter& painter)
{
    auto bottomY = geometry().bottom() - MARGINS;

    for (auto& event : m_project.events()) {
        QFontMetrics fm(painter.font());
        auto textRect = fm.boundingRect(event.name);
        auto eventX = MARGINS + xFromNS(event.timeNS);

        painter.setPen(COLOR_TEXT);
        painter.drawText(eventX - textRect.width() / 2, EVENT_NAME_Y - 8, event.name);
        painter.setPen(COLOR_BKG_LINE);
        painter.drawLine(eventX, EVENT_NAME_Y, eventX, bottomY);
    }
}

void SectionWidget::drawSections(QPainter& painter)
{
    int sectionI = 0;

    for (auto& section : m_project.sections()) {
        auto range = section.absoluteRange();
        auto startX = MARGINS + xFromNS(range.first);
        auto endX = MARGINS + xFromNS(range.second);
        auto y = EVENT_NAME_Y + SECTION_SPACING * sectionI + SECTION_BOX_HEIGHT * sectionI;

        painter.setPen(COLOR_TEXT);

        switch (section.type) {
        case Section::SectionType::WritingData:
        case Section::SectionType::WritingGarbage:
            painter.setBrush(QBrush(COLOR_BKG_WRITE));
            break;
        case Section::SectionType::WaitingInTriState:
            painter.setBrush(QBrush());
            break;
        case Section::SectionType::ReadingData:
            painter.setBrush(QBrush(COLOR_BKG_READ));
            break;
        }

        painter.drawRect(startX, y, endX - startX, SECTION_BOX_HEIGHT);

        if (section.type == Section::SectionType::WritingGarbage || section.type == Section::SectionType::WaitingInTriState) {
            painter.setPen(COLOR_BKG_LINE);
            painter.setBrush(Qt::BDiagPattern);
            painter.drawRect(startX, y, endX - startX, SECTION_BOX_HEIGHT);
        }

        auto label = QString("");

        switch (section.type) {
        case Section::SectionType::WritingData:
            label = QString("%1 | WR -> %2").arg(section.component.name).arg(section.bus);
            break;
        case Section::SectionType::WritingGarbage:
            label = QString("%1 | WR GARBAGE -> %2").arg(section.component.name).arg(section.bus);
            break;
        case Section::SectionType::WaitingInTriState:
            label = QString("%1 | tri-state <-> %2").arg(section.component.name).arg(section.bus);
            break;
        case Section::SectionType::ReadingData:
            label = QString("%1 | RD <- %2").arg(section.component.name).arg(section.bus);
            break;
        }

        painter.setPen(QColor(Qt::white));
        painter.drawText(startX + 5, y + SECTION_BOX_HEIGHT - 5, label);

        sectionI++;
    }
}

void SectionWidget::drawTimePoint(int ns, QPainter& painter)
{
    auto markerX = MARGINS + xFromNS(ns);
    auto label = QString("%1").arg(ns);

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
