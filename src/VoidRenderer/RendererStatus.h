// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_RENDERER_STATUS_BAR_H
#define _VOID_RENDERER_STATUS_BAR_H

/* Qt */
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ColorWidget : public QWidget
{
public:
    ColorWidget(QWidget* parent = nullptr);

    void SetColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    QColor m_CurrentColor;

};

class RendererDisplayLabel : public QLabel
{
public:
    RendererDisplayLabel(QWidget* parent = nullptr);
    RendererDisplayLabel(const std::string& text, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void Setup();
};

class RendererStatusBar : public QWidget
{
    Q_OBJECT

public:
    RendererStatusBar(QWidget* parent = nullptr);
    virtual ~RendererStatusBar();

    void SetRenderResolution(const int width, const int height);
    void SetMouseCoordinates(const int x, const int y);
    void SetColourValues(const float r, const float g, const float b, const float a);

private: /* Members */
    /* Main Layout */
    QHBoxLayout* m_Layout;

    /* Layout needs to be grouped so that size changes don't affect the center elements */
    /* Left Side Layout */
    QHBoxLayout* m_LeftLayout;
    /* Right Side Layout */
    QHBoxLayout* m_RightLayout;

    /* Input Details */
    QLabel* m_ResolutionLabel;
    QLabel* m_ResolutionValue;

    /* X-Y Mouse Coords */
    QLabel* m_XLabel;
    QLabel* m_XValue;
    
    QLabel* m_YLabel;
    QLabel* m_YValue;

    /* Color Values */
    ColorWidget* m_ColorPreview;

    QLabel* m_RValue;
    QLabel* m_GValue;
    QLabel* m_BValue;
    QLabel* m_AValue;

private: /* Methods */
    void Build();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_STATUS_BAR_H
