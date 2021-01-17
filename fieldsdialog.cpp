#include "fieldsdialog.h"
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

QString FieldsDialog::valueAsString(const QString& label) const
{
    return m_fields[label].toString();
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
