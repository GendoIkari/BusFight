#pragma once

#include <QDialog>
#include <QHash>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class FieldsDialog : public QDialog {
    Q_OBJECT
public:
    enum class FieldType {
        String,
        Integer,
    };

    explicit FieldsDialog(const QString& title, QWidget* parent = nullptr);

    void addField(const QString& label, FieldType type);
    QString valueAsString(const QString& label) const;
    int valueAsInt(const QString& label) const;

    bool isAccepted() const;

private:
    void buildUI();

    bool m_accepted = false;
    QVBoxLayout* m_fieldsLayout = nullptr;
    QHash<QString, QVariant> m_fields;
    QHash<QString, bool> m_fieldsValidate;
    QPushButton* m_okButton = nullptr;

private slots:
    void onOkClicked();
    void checkParameters();
};
