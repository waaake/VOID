// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OBJECT_H
#define _VOID_OBJECT_H

/* Qt*/
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Identity.h"
#include "VoidCore/Serialization.h"

VOID_NAMESPACE_OPEN

class VoidObject : public QObject, public SerializableEntity
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

    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override {}
    virtual void Deserialize(const rapidjson::Value& in) override {}
    virtual const char* TypeName() const override { return "VoidObject"; }

private: /* Members */
    VUID m_Vuid;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OBJECT_H
