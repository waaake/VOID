// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OBJECT_H
#define _VOID_OBJECT_H

/* Qt*/
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Identity.h"

VOID_NAMESPACE_OPEN

class VoidObject : public QObject
{
public:
    VoidObject(QObject* parent = nullptr) : QObject(parent), m_Vuid() {}

    /* Accessible Void Unique Identifier in a string representation */
    inline std::string Vuid() const
    { 
        std::string result = "VUID{";
        result.append(m_Vuid.str()).append("}");
        return result;
    }

private: /* Members */
    VUID m_Vuid;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OBJECT_H
