#include "sectionwidget.h"

#include "fieldsdialog.h"
#include <QMenu>
#include <QPainter>
#include <QSet>

SectionWidget::SectionWidget(Project& project, QWidget* parent)
    : QWidget(parent)
    , m_project(project)
{
    buildUI();
    connect(&project, &Project::projectChanged, this, &SectionWidget::onProjectChanged);
    onProjectChanged();
}

void SectionWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawHeader(painter);
    drawSections(painter);
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

    for (auto& bus : m_project.buses())
        for (auto& section : bus.sections) {
            int sectionStart = m_project.absoluteRange(section).first;
            int sectionEnd = m_project.absoluteRange(section).second;
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

void SectionWidget::buildUI()
{
    setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(this, &SectionWidget::customContextMenuRequested, this, &SectionWidget::onContextMenuRequested);
}

void SectionWidget::drawHeader(QPainter& painter)
{
    auto width = geometry().width();

    painter.setPen(COLOR_TEXT);
    painter.drawLine(MARGINS, HEADER_LINE_Y, width - MARGINS, HEADER_LINE_Y);

    QSet<int> timePoints;

    for (auto& event : m_project.events())
        timePoints.insert(event.timeNS);
    for (auto& bus : m_project.buses())
        for (auto& section : bus.sections) {
            auto range = m_project.absoluteRange(section);
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
    int usedY = EVENT_NAME_Y;

    for (auto& bus : m_project.buses()) {
        if (bus.sections.isEmpty())
            continue;

        QHash<QUuid, QVector<Section>> sectionsByComponent;
        for (auto& section : bus.sections)
            sectionsByComponent[section.component].append(section);

        auto firstSectionY = usedY;
        auto totalSectionHeight = (SECTION_BOX_HEIGHT + SECTION_SPACING) * sectionsByComponent.keys().count() + SECTION_SPACING;
        usedY += totalSectionHeight + MARGINS;

        painter.setPen(COLOR_BKG_LINE);
        painter.setBrush(COLOR_BKG_BUS);
        painter.drawRect(MARGINS, firstSectionY, geometry().width() - MARGINS * 2, totalSectionHeight);

        painter.setPen(COLOR_BKG_LINE);
        painter.drawText(MARGINS + 5, firstSectionY + 15, bus.name);

        int sectionI = 0;
        for (auto& componentUuid : sectionsByComponent.keys()) {
            auto component = m_project.component(componentUuid);
            auto y = firstSectionY + SECTION_SPACING + SECTION_SPACING * sectionI + SECTION_BOX_HEIGHT * sectionI;
            int minimumX = 9999;

            for (auto& section : sectionsByComponent[componentUuid]) {
                auto range = m_project.absoluteRange(section);
                auto startX = MARGINS + xFromNS(range.first);
                auto endX = MARGINS + xFromNS(range.second);
                minimumX = std::min(startX, minimumX);

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

                painter.setPen(COLOR_TEXT);
                painter.drawRect(startX, y, endX - startX, SECTION_BOX_HEIGHT);
                m_sectionsRects.append({ { startX, y, endX - startX, SECTION_BOX_HEIGHT }, section });

                if (section.type == Section::SectionType::WritingGarbage || section.type == Section::SectionType::WaitingInTriState) {
                    painter.setPen(COLOR_BKG_LINE);
                    painter.setBrush(Qt::BDiagPattern);
                    painter.drawRect(startX, y, endX - startX, SECTION_BOX_HEIGHT);
                }

                auto label = QString("");
                painter.setPen(QColor(Qt::white));

                switch (section.type) {
                case Section::SectionType::WritingData:
                    label = QString("WR");
                    break;
                case Section::SectionType::WritingGarbage:
                    label = QString("WR GARBAGE");
                    break;
                case Section::SectionType::WaitingInTriState:
                    painter.setPen(COLOR_TEXT);
                    label = QString("HZ");
                    break;
                case Section::SectionType::ReadingData:
                    label = QString("RD");
                    break;
                }

                painter.drawText(startX + 5, y + SECTION_BOX_HEIGHT - 5, label);
            }

            QFontMetrics fm(painter.font());
            auto textRect = fm.boundingRect(component.name);
            painter.setPen(COLOR_BKG_LINE);
            painter.drawText(minimumX - textRect.width() - 5, y + SECTION_BOX_HEIGHT - 5, component.name);

            sectionI++;
        }
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
    m_sectionsRects.clear();
    update();
}

void SectionWidget::onContextMenuRequested(QPointF point)
{
    QRect r;
    Section s;
    bool onSection = false;
    for (auto& rect : m_sectionsRects)
        if (rect.first.contains(point.toPoint())) {
            r = rect.first;
            s = rect.second;
            onSection = true;
        }

    QMenu sectionMenu;
    sectionMenu.move(mapToGlobal(point.toPoint()));
    auto editAction = sectionMenu.addAction("Edit", this, [&] {
        FieldsDialog::editSectionDialog(m_project, s.uuid);
    });
    editAction->setEnabled(onSection);
    sectionMenu.addSeparator();
    auto removeAction = sectionMenu.addAction("Remove", this, [=] {
        m_project.removeSection(s.uuid);
    });
    removeAction->setEnabled(onSection);
    sectionMenu.exec();
}
