#pragma once

#include "entities.h"
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QUuid>
#include <QWidget>

class SectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SectionDialog(Project &project, const QUuid &sectionUuid = QUuid(), QWidget *parent = nullptr);

private:
    void buildUI();
    void setDefault(QComboBox *combo, QVariant value);
    void addSectionTypes(QComboBox *combo, bool signal = false);
    bool checkRange();

    Project &m_project;
    QUuid m_sectionUuid;
    QUuid m_componentUuid;
    Section::SectionType m_sectionType;
    QUuid m_busUuid;
    QUuid m_startEventUuid;
    int m_start;
    QUuid m_endEventUuid;
    int m_end;
};
