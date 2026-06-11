// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EXPORT_OPTIONS_H
#define _EXPORT_OPTIONS_H

/* Qt */
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QIntValidator>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidUi/Media/Browser.h"
#include "VoidUi/Tools/LogWindow.h"

VOID_NAMESPACE_OPEN

class MovieOptions : public QWidget
{
    Q_OBJECT
public:
    MovieOptions(QWidget* parent = nullptr);
    ~MovieOptions();

    void SetRate(double rate);
    double Rate() const { return m_RateEdit->text().toDouble(); }
    MovieCodec Codec() const { return static_cast<MovieCodec>(m_CodecCombo->currentIndex()); }

private: /* Members */
    QVBoxLayout* m_Layout;
    QComboBox* m_CodecCombo;
    QLineEdit* m_RateEdit;
    // QCheckBox* m_RespeedCheck;
    QDoubleValidator* m_RateValidator;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
};

class ExportOptions : public QDialog
{
    Q_OBJECT
public:
    ExportOptions(QWidget* parent = nullptr);
    virtual ~ExportOptions();

    inline QSize sizeHint() const override { return QSize(350, 350); }

    inline void SetRate(double rate) { m_MovieOptions->SetRate(rate); }
    inline double Rate() const { return m_MovieOptions->Rate(); }

    void SetRange(int start, int end);
    inline MFrameRange Range() const { return {m_StartEdit->text().toInt(), m_EndEdit->text().toInt()}; }
    inline bool RangeOverridden() const { return m_OverrideRangeCheck->isChecked(); }
    inline MovieCodec Codec() const { return m_MovieOptions->Codec(); }
    inline int ScaleIndex() const { return m_ResolutionCombo->currentIndex(); }

    inline MediaExportDescriptor FileDescriptor() const { return m_Descriptor; }
    inline void ClearLog() { m_LogModel->Clear(); }

signals:
    void exported();

private: /* Members */
    QVBoxLayout* m_Layout;
    QLineEdit* m_OutputEdit;
    QLineEdit* m_StartEdit;
    QLineEdit* m_EndEdit;
    QCheckBox* m_OverrideRangeCheck;
    QComboBox* m_ResolutionCombo;
    QComboBox* m_OutProcessorCombo;

    QGroupBox* m_MovieGroup;
    MovieOptions* m_MovieOptions;

    TaskLogModel* m_LogModel;
    QListView* m_Logger;

    QPushButton* m_BrowseButton;
    QPushButton* m_ExportButton;
    QPushButton* m_CancelButton;

    QIntValidator* m_RangeValidator;

protected: /* Members */
    MediaExportDescriptor m_Descriptor;

private: /* Methods */
    void Build();
    void Connect();
    void Setup();

    void Browse();

protected: /* Methods */
    void Log(const QString& text, const TaskLog::Level& level);
};

VOID_NAMESPACE_CLOSE

#endif // _EXPORT_OPTIONS_H
