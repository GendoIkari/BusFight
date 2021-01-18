#pragma once

#include "entities.h"
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
        ComboBox,
    };

    explicit FieldsDialog(const QString& title, QWidget* parent = nullptr);

    static void addEventDialog(Project& project);
    static void addComponentDialog(Project& project);
    static void addBusDialog(Project& project);
    static void addSectionDialog(Project& project);
    static void editEventDialog(Project& project, const QString& eventName);

    void addField(const QString& label, FieldType type);
    void addComboBox(const QString& label, QVector<QString> choices);

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
