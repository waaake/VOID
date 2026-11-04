// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "CachePreferences.h"
#include "VoidCore/Logging.h"
#include "VoidCore/VoidTools.h"
#include "VoidObjects/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

CachePreferences::CachePreferences(QWidget* parent)
    : BasicPreference(parent)
{
    /* Build UI */
    Build();

    /* Setup Default values */
    Setup();
}

CachePreferences::~CachePreferences()
{
}

void CachePreferences::Reset()
{
    unsigned int memory = VoidPreferences::Instance().GetSetting(Settings::CacheMemory).toUInt();
    m_CacheBox->setValue(memory);

    unsigned int threads = VoidPreferences::Instance().GetSetting(Settings::CacheThreads).toUInt();
    m_ThreadsBox->setValue(threads);
}

void CachePreferences::Save()
{
    /* Get and save the value of the Cache Memory size and Thread Count */
    VoidPreferences::Instance().Set(Settings::CacheMemory, QVariant(m_CacheBox->value()));
    VoidPreferences::Instance().Set(Settings::CacheThreads, QVariant(m_ThreadsBox->value()));
}

void CachePreferences::Build()
{
    /* The internal layout */
    m_Layout = new QGridLayout(this);

    /* Add Preferences */
    m_CacheDescription = new QLabel("Controls the amount of memory (RAM) reserved for temporary data (cache) storage during processing.\n\n\
A larger cache can improve performance by reducing the need to recompute or reload frequently accessed data.\n\
 Lower Values: Running on a low memory system or want to conserve for other processes.\n\
 Higher Values: If you have plenty of RAM or viewing High resolution content.");

    m_CacheLabel = new QLabel("Cache Memory Size");
    m_CacheBox = new QSpinBox;

    m_ThreadsDescription = new QLabel("Sets the maximum number of threads that can run concurrently in the thread pool.\n\n\
 Lower Count: Running on a lower power device with lesser overall cores.\n\
 Higher Count: Want faster throughput for cache operations and have plenty cores available for multiprocessing.");

    m_ThreadsLabel = new QLabel("Read Threads");
    m_ThreadsBox = new QSpinBox;

    /* Add to the layout */
    m_Layout->addWidget(m_CacheDescription, 0, 0, 1, 5);
    m_Layout->addWidget(m_CacheLabel, 1, 0);
    m_Layout->addWidget(m_CacheBox, 1, 1);

    m_Layout->addItem(new QSpacerItem(10, 20), 2, 3);

    m_Layout->addWidget(m_ThreadsDescription, 3, 0, 1, 5);
    m_Layout->addWidget(m_ThreadsLabel, 4, 0);
    m_Layout->addWidget(m_ThreadsBox, 4, 1);

    /* Spacer */
    m_Layout->setRowStretch(5, 1);
}

void CachePreferences::Setup()
{
    // Max Values
    unsigned int maxMem = static_cast<unsigned int>(Tools::total_memory() / (1024UL * 1024UL * 1024UL));
    unsigned int maxThreads = Tools::processor_count();

    VOID_LOG_INFO("Maximum Memory Available: {0} GB --- {1}", maxMem, Tools::total_memory());
    VOID_LOG_INFO("Maximum Processor Based Threads: {0}", maxThreads);

    m_CacheBox->setMinimum(1);
    m_CacheBox->setMaximum(maxMem);

    m_ThreadsBox->setMinimum(1);
    m_ThreadsBox->setMaximum(maxThreads);

    // Default values
    m_CacheBox->setValue(1);
    m_ThreadsBox->setValue(maxThreads * 0.5);
}

VOID_NAMESPACE_CLOSE
