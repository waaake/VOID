// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_SERIALIZATION_H
#define _VOID_SERIALIZATION_H

/* STD */
#include <fstream>

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

    enum class Type
    {
        ASCII,
        BINARY,

        /* Used only to describe the type of file when loading */
        INVALID
    };

    inline bool ValidateHeader(const char* header)
    {
        return std::memcmp(header, ASCII_MAGIC, MAGIC_SIZE) == 0 ||
            std::memcmp(header, BINARY_MAGIC, MAGIC_SIZE) == 0;
    }

    inline Type FileType(const char* header)
    {
        if (std::memcmp(header, ASCII_MAGIC, MAGIC_SIZE) == 0)
            return Type::ASCII;
        else if (std::memcmp(header, BINARY_MAGIC, MAGIC_SIZE) == 0)
            return Type::BINARY;

        return Type::INVALID;
    }
} // namespace EtherFormat

/**
 * Any Entity in VOID which can be serialized
 */
class VOID_API SerializableEntity
{
public:
    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const = 0;
    virtual void Serialize(std::ostream& out) const = 0;
    virtual void Deserialize(const rapidjson::Value& in) = 0;
    virtual void Deserialize(std::istream& in) = 0;

    /* The type name in the serialization document */
    virtual const char* TypeName() const = 0;

    /**
     * Helper:
     * Writes std::strind data to the out buffer
     */
    static void WriteString(std::ostream& out, const std::string& str)
    {
        /* Store the size and the data */
        uint32_t len = static_cast<uint32_t>(str.size());
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(str.data(), len);
    }

    /**
     * Helper:
     * Reads the input stream and returns the string data
     */
    static std::string ReadString(std::istream& in)
    {
        uint32_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        
        std::string str(len, '\0');
        in.read(&str[0], len);

        return str;
    }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_SERIALIZATION_H
