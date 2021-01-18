#include "fieldsdialog.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QMessageBox>

FieldsDialog::FieldsDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    buildUI();
}

void FieldsDialog::addEventDialog(Project& project)
{
    FieldsDialog dialog("Add Event");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.addField("Time(ns)", FieldsDialog::FieldType::Integer);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    auto ns = dialog.valueAsInt("Time(ns)");
    project.addEvent({ .name = name, .timeNS = ns });
}

void FieldsDialog::addComponentDialog(Project& project)
{
    FieldsDialog dialog("Add Component");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    project.addComponent({ .name = name });
}

void FieldsDialog::addBusDialog(Project& project)
{
    FieldsDialog dialog("Add Bus");
    dialog.addField("Name", FieldsDialog::FieldType::String);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto name = dialog.valueAsString("Name");
    project.addBus({ .name = name });
}

void FieldsDialog::addSectionDialog(Project& project)
{
    FieldsDialog dialog("Add Section");

    QVector<QPair<QString, QVariant>> componentsItems;
    for (auto& c : project.components())
        componentsItems.append({ c.name, c.uuid });
    dialog.addComboBox("Component", componentsItems);

    dialog.addComboBox("Type", {
                                   { "Waiting in Tri-State", int(Section::SectionType::WaitingInTriState) },
                                   { "Reading from Bus", int(Section::SectionType::ReadingData) },
                                   { "Writing to Bus", int(Section::SectionType::WritingData) },
                                   { "Writing Garbage to Bus", int(Section::SectionType::WritingGarbage) },
                               });

    QVector<QPair<QString, QVariant>> busItems;
    for (auto& b : project.buses())
        busItems.append({ b.name, b.uuid });
    dialog.addComboBox("Bus", busItems);

    QVector<QPair<QString, QVariant>> eventItems;
    for (auto& e : project.events())
        eventItems.append({ e.name, e.uuid });

    dialog.addComboBox("Start Reference", eventItems);
    dialog.addField("Start Offset", FieldsDialog::FieldType::Integer);
    dialog.addComboBox("End Reference", eventItems);
    dialog.addField("End Offset", FieldsDialog::FieldType::Integer);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto componentName = dialog.valueAsString("Component");
    auto busName = dialog.valueAsString("Bus");
    auto startEventName = dialog.valueAsString("Start Reference");
    auto startOffset = dialog.valueAsInt("Start Offset");
    auto endEventName = dialog.valueAsString("End Reference");
    auto endOffset = dialog.valueAsInt("End Offset");
    auto type = Section::SectionType(dialog.valueAsInt("Type"));

    Section s {
        .component = componentName,
        .type = type,
        .referenceStartEvent = startEventName,
        .start = startOffset,
        .referenceEndEvent = endEventName,
        .end = endOffset,
    };
    auto range = project.absoluteRange(s);
    if (range.first >= range.second) {
        QMessageBox message;
        message.setWindowTitle("Error");
        message.setText("Cannot create a section with start > end");
        message.exec();
        return;
    }

    project.addSection(busName, s);
}

void FieldsDialog::editEventDialog(Project& project, const QString& eventName)
{
    FieldsDialog dialog("Edit Event");
    dialog.addField("Time", FieldsDialog::FieldType::Integer);
    dialog.exec();
    if (!dialog.isAccepted())
        return;

    auto newTime = dialog.valueAsInt("Time");
    project.moveEvent(eventName, newTime);
}

void FieldsDialog::addField(const QString& label, FieldsDialog::FieldType type)
{
    Q_ASSERT(type != FieldType::ComboBox);

    auto rowLayout = new QHBoxLayout();
    rowLayout->addWidget(new QLabel(label));
    auto lineEdit = new QLineEdit(this);
    rowLayout->addWidget(lineEdit);
    if (type == FieldType::Integer)
        lineEdit->setValidator(new QIntValidator(lineEdit));
    m_fieldsLayout->addLayout(rowLayout);

    if (m_fieldsValidate.isEmpty())
        lineEdit->setFocus();

    m_fieldsValidate[label] = false;

    connect(lineEdit, &QLineEdit::textChanged, this, [=] {
        m_fields[label] = lineEdit->text();
        m_fieldsValidate[label] = !lineEdit->text().isEmpty() && lineEdit->hasAcceptableInput();
        checkParameters();
    });
}

void FieldsDialog::addComboBox(const QString& label, QVector<QPair<QString, QVariant>> choices)
{
    Q_ASSERT(choices.count() > 0);

    auto rowLayout = new QHBoxLayout();
    rowLayout->addWidget(new QLabel(label));
    auto combo = new QComboBox(this);
    rowLayout->addWidget(combo);
    m_fieldsLayout->addLayout(rowLayout);
    for (auto& choice : choices)
        combo->addItem(choice.first, choice.second);

    if (m_fieldsValidate.isEmpty())
        combo->setFocus();

    m_fieldsValidate[label] = true;
    m_fields[label] = choices.first().second;
    checkParameters();

    connect(combo, &QComboBox::currentTextChanged, this, [=] {
        m_fields[label] = combo->currentData();
        m_fieldsValidate[label] = combo->currentIndex() != -1;
        checkParameters();
    });
}

QString FieldsDialog::valueAsString(const QString& label) const
{
    return m_fields[label].toString();
}

int FieldsDialog::valueAsInt(const QString& label) const
{
    return m_fields[label].toInt();
}

bool FieldsDialog::isAccepted() const
{
    return m_accepted;
}

void FieldsDialog::buildUI()
{
    auto layout = new QVBoxLayout(this);
    m_fieldsLayout = new QVBoxLayout();
    layout->addLayout(m_fieldsLayout);

    auto buttonsLayoutRow = new QHBoxLayout();
    m_okButton = new QPushButton("Add", this);
    m_okButton->setDefault(true);
    m_okButton->setEnabled(false);
    connect(m_okButton, &QPushButton::clicked, this, &FieldsDialog::onOkClicked);
    buttonsLayoutRow->addWidget(m_okButton);
    auto cancelButton = new QPushButton("Cancel", this);
    buttonsLayoutRow->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, this, &FieldsDialog::close);
    layout->addLayout(buttonsLayoutRow);
}

void FieldsDialog::onOkClicked()
{
    m_accepted = true;
    close();
}

void FieldsDialog::checkParameters()
{
    for (bool ok : qAsConst(m_fieldsValidate))
        if (!ok) {
            m_okButton->setEnabled(false);
            return;
        }

    m_okButton->setEnabled(true);
}
