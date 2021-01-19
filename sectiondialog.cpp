#include "sectiondialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

SectionDialog::SectionDialog(Project& project, const QUuid& sectionUuid, QWidget* parent)
    : QDialog(parent)
    , m_project(project)
    , m_sectionUuid(sectionUuid)
{
    buildUI();
}

void SectionDialog::buildUI()
{
    auto busList = new QComboBox();
    for (auto& b : m_project.buses())
        busList->addItem(b.name, b.uuid);
    busList->setFocus();

    auto componentsList = new QComboBox();
    for (auto& c : m_project.components())
        componentsList->addItem(c.name, c.uuid);

    auto startEventList = new QComboBox();
    auto endEventList = new QComboBox();
    for (auto& e : m_project.events()) {
        startEventList->addItem(e.name, e.uuid);
        endEventList->addItem(e.name, e.uuid);
    }
    auto startNS = new QLineEdit();
    startNS->setValidator(new QIntValidator(startNS));
    auto endNS = new QLineEdit();
    endNS->setValidator(new QIntValidator(endNS));

    auto sectionTypeList = new QComboBox();

    auto okButton = new QPushButton("Add", this);
    auto cancelButton = new QPushButton("Cancel", this);
    okButton->setDefault(true);

    auto componentRow = new QHBoxLayout();
    componentRow->addWidget(new QLabel("Bus"));
    componentRow->addWidget(busList, 1);
    componentRow->addWidget(new QLabel("Component"));
    componentRow->addWidget(componentsList, 1);

    auto startEventRow = new QHBoxLayout();
    startEventRow->addWidget(new QLabel("Start Reference"));
    startEventRow->addWidget(startEventList, 1);
    startEventRow->addWidget(new QLabel("Offset"));
    startEventRow->addWidget(startNS, 1);

    auto endEventRow = new QHBoxLayout();
    endEventRow->addWidget(new QLabel("End Reference"));
    endEventRow->addWidget(endEventList, 1);
    endEventRow->addWidget(new QLabel("Offset"));
    endEventRow->addWidget(endNS, 1);

    auto sectionTypeRow = new QHBoxLayout();
    sectionTypeRow->addWidget(new QLabel("Type"));
    sectionTypeRow->addWidget(sectionTypeList);

    auto buttonsLayoutRow = new QHBoxLayout();
    buttonsLayoutRow->addWidget(okButton);
    buttonsLayoutRow->addWidget(cancelButton);

    auto layout = new QVBoxLayout(this);
    layout->addLayout(componentRow);
    layout->addLayout(startEventRow);
    layout->addLayout(endEventRow);
    layout->addLayout(sectionTypeRow);
    layout->addLayout(buttonsLayoutRow);

    if (m_sectionUuid.isNull()) {
        setWindowTitle("Add Section");
        okButton->setText("Add");
        m_componentUuid = componentsList->currentData().toUuid();
        m_busUuid = busList->currentData().toUuid();
        m_sectionType = Section::SectionType(sectionTypeList->currentData().toInt());
        m_startEventUuid = startEventList->currentData().toUuid();
        m_endEventUuid = endEventList->currentData().toUuid();
        m_start = 0;
        m_end = 5;
    } else {
        setWindowTitle("Edit Section");
        okButton->setText("Edit");
        busList->setEnabled(false);
        auto section = m_project.section(m_sectionUuid);
        auto bus = m_project.bus(section);
        m_componentUuid = section.component;
        m_busUuid = bus.uuid;
        m_sectionType = section.type;
        m_startEventUuid = section.referenceStartEvent;
        m_start = section.start;
        m_endEventUuid = section.referenceEndEvent;
        m_end = section.end;
    }
    auto bus = m_project.bus(m_busUuid);
    setDefault(componentsList, m_componentUuid);
    setDefault(busList, m_busUuid);
    addSectionTypes(sectionTypeList, bus.type == Bus::BusType::Signal);
    setDefault(startEventList, m_startEventUuid);
    setDefault(endEventList, m_endEventUuid);
    startNS->setText(QString::number(m_start));
    endNS->setText(QString::number(m_end));

    connect(componentsList, &QComboBox::currentTextChanged, this, [=] {
        m_componentUuid = componentsList->currentData().toUuid();
    });
    connect(busList, &QComboBox::currentTextChanged, this, [=] {
        m_busUuid = busList->currentData().toUuid();
        auto bus = m_project.bus(m_busUuid);
        addSectionTypes(sectionTypeList, bus.type == Bus::BusType::Signal);
    });
    connect(startEventList, &QComboBox::currentTextChanged, this, [=] {
        m_startEventUuid = startEventList->currentData().toUuid();
        okButton->setEnabled(checkRange());
    });
    connect(endEventList, &QComboBox::currentTextChanged, this, [=] {
        m_endEventUuid = endEventList->currentData().toUuid();
        okButton->setEnabled(checkRange());
    });
    connect(startNS, &QLineEdit::textChanged, this, [=] {
        m_start = startNS->text().toInt();
        okButton->setEnabled(checkRange());
    });
    connect(endNS, &QLineEdit::textChanged, this, [=] {
        m_end = endNS->text().toInt();
        okButton->setEnabled(checkRange());
    });
    connect(sectionTypeList, &QComboBox::currentTextChanged, this, [=] {
        m_sectionType = Section::SectionType(sectionTypeList->currentData().toInt());
    });
    connect(okButton, &QPushButton::clicked, this, [=] {
        if (m_sectionUuid.isNull())
            m_project.addSection(m_busUuid, {
                                                .component = m_componentUuid,
                                                .type = m_sectionType,
                                                .referenceStartEvent = m_startEventUuid,
                                                .start = m_start,
                                                .referenceEndEvent = m_endEventUuid,
                                                .end = m_end,
                                            });
        else
            m_project.editSection(m_sectionUuid, m_componentUuid, m_sectionType, m_startEventUuid, m_start, m_endEventUuid, m_end);
        close();
    });
    connect(cancelButton, &QPushButton::clicked, this, &SectionDialog::close);
}

void SectionDialog::setDefault(QComboBox* combo, QVariant value)
{
    for (int i = 0; i < combo->count(); ++i) {
        if (combo->itemData(i) == value) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

void SectionDialog::addSectionTypes(QComboBox* combo, bool signal)
{
    combo->clear();
    if (signal) {
        combo->addItem("High", int(Section::SectionType::SignalHigh));
        combo->addItem("Tri-State", int(Section::SectionType::SignalTriState));
        combo->addItem("Low", int(Section::SectionType::SignalLow));
        combo->addItem("Unknown", int(Section::SectionType::SignalUnknown));
    } else {
        combo->addItem("Waiting in Tri-State", int(Section::SectionType::WaitingInTriState));
        combo->addItem("Reading from Bus", int(Section::SectionType::ReadingData));
        combo->addItem("Writing to Bus", int(Section::SectionType::WritingData));
        combo->addItem("Writing Garbage to Bus", int(Section::SectionType::WritingGarbage));
    }
    setDefault(combo, int(m_sectionType));
}

bool SectionDialog::checkRange()
{
    auto minMax = m_project.absoluteRange(m_startEventUuid, m_start, m_endEventUuid, m_end);
    return minMax.first < minMax.second;
}
