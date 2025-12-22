// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QThreadPool>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API ThreadPool : public QThreadPool
{
    ThreadPool(QObject* parent = nullptr);
public:
    static ThreadPool& Instance();
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    inline static void Init() { Instance().Initialize(); }
    inline static void Destroy() { Instance().Uninitialize(); }

protected: /* Methods */
    void Initialize();
    void Uninitialize();
};

VOID_NAMESPACE_CLOSE
