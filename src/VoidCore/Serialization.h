// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_SERIALIZATION_H
#define _VOID_SERIALIZATION_H

/* RapidJSON */
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>

/* Internal */
#include "Definition.h"

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

/**
 * Any Entity in VOID which can be serialized
 */
class VOID_API SerializableEntity
{
public:
    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const = 0;
    virtual void Deserialize(const rapidjson::Value& in) = 0;

    /* The type name in the serialization document */
    virtual const char* TypeName() const = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_SERIALIZATION_H
