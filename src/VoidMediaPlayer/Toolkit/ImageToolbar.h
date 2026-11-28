// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_IMAGE_TOOLKIT_H
#define _VOID_IMAGE_TOOLKIT_H

/* Qt */
#include <QLayout>
#include <QWidget>

#include "LayerController.h"

/* Internal */
#include "Definition.h"
#include "ColorController.h"

VOID_NAMESPACE_OPEN

class ImageToolbar : public QWidget
{
    Q_OBJECT
public:
    ImageToolbar(QWidget* parent = nullptr);
    ~ImageToolbar();

    void SetImageLayers(const std::vector<std::string>& layers) { m_LayerController->SetLayers(layers); }
    void SetImageLayers(const std::vector<std::string>& layers, const std::string& current)
    {
        m_LayerController->SetLayers(layers, current);
    }
    void ResetImageLayers() { m_LayerController->clear(); }
    void SetupColor(ColorEngine* engine) { m_ColorViews->Setup(engine); }

signals:
    void exposureChanged(const float exposure);
    void gammaChanged(const float gamma);
    void gainChanged(const float gain);
    void imageLayerChangeRequested(const QString&);

private:
    QHBoxLayout* m_Layout;
    ImageLayerController* m_LayerController;
    ColorController2* m_ColorViews;

    QuickSpinner* m_ExposureSpinner;
    QuickSpinner* m_GammaSpinner;
    QuickSpinner* m_GainSpinner;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_TOOLKIT_H
