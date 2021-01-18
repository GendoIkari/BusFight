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
    static void editEventDialog(Project& project, const QUuid& eventUuid);
    static void editBusDialog(Project& project, const QUuid& busUuid);
    static void editComponentDialog(Project& project, const QUuid& componentUuid);
    static void editSectionDialog(Project& project, const QUuid& sectionUuid);

    void addField(const QString& label, FieldType type, QVariant defaultValue = QVariant());
    void addComboBox(const QString& label, QVector<QPair<QString, QVariant>> choices, QVariant defaultValue = QVariant());

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
