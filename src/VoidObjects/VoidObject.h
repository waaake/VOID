// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_OBJECT_H
#define _VOID_OBJECT_H

/* JSON */
#include <rapidjson/document.h>
#include <rapidjson/allocators.h>

/* Qt*/
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidCore/Identity.h"

VOID_NAMESPACE_OPEN

namespace EtherFormat {

    constexpr auto AsciiExt = ".ether";
    constexpr auto BinaryExt = ".nether";

    constexpr char ASCII_MAGIC[] = { 'E', 'T', 'H', 'A', };
    constexpr char BINARY_MAGIC[] = { 'N', 'E', 'T', 'H' };
    
    constexpr size_t MAGIC_SIZE = sizeof(ASCII_MAGIC);

    // constexpr uint8_t file_version = 1;

    inline bool ValidateHeader(const char* header)
    {
        return std::memcmp(header, ASCII_MAGIC, MAGIC_SIZE) == 0 ||
            std::memcmp(header, BINARY_MAGIC, MAGIC_SIZE) == 0;
    }

} // namespace EtherFormat

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

    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const {}
    virtual void Deserialize(const rapidjson::Value& in) {}
    /* The type name in the serialization document */
    virtual const char* TypeName() const { return "VoidObject"; }

private: /* Members */
    VUID m_Vuid;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_OBJECT_H
