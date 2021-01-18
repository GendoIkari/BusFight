#include "fieldsdialog.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>

FieldsDialog::FieldsDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    buildUI();
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

void FieldsDialog::addComboBox(const QString& label, QVector<QString> choices)
{
    Q_ASSERT(choices.count() > 0);

    auto rowLayout = new QHBoxLayout();
    rowLayout->addWidget(new QLabel(label));
    auto combo = new QComboBox(this);
    rowLayout->addWidget(combo);
    m_fieldsLayout->addLayout(rowLayout);
    for (auto& choice : choices)
        combo->addItem(choice);

    if (m_fieldsValidate.isEmpty())
        combo->setFocus();

    m_fieldsValidate[label] = true;
    m_fields[label] = choices.first();
    checkParameters();

    connect(combo, &QComboBox::currentTextChanged, this, [=] {
        m_fields[label] = combo->currentText();
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
