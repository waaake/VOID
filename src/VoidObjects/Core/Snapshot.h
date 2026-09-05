// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _ENTITY_SNAPSHOT_H
#define _ENTITY_SNAPSHOT_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"
#include "VoidCore/Serialization.h"

VOID_NAMESPACE_OPEN

/**
 * @brief Snapshot refers to an entity state stored at a point in time.
 * This state is locked and can be reused to go back to how the entity was back in the time.
 * 
 * To make it very generic and still serializable, this needs to hold binary data (or could be deserializable ascii too) 
 * Provides generic serialization by storing the data as a standard string and it's name and description/comment if any
 * 
 */
struct Snapshot : SerializableEntity
{
    std::string name;
    std::string description; // Comment or anything attached to the snapshot

    /** TODO: For now I'm going with the same binary serialized data which gets saved as the nether format
     *Later we can see to even optimise the amount of data that is needed for the snapshots
     * This may become applicable to the playlists as well
     * 
     */
    std::string data; // binary serialized data for the entity

    Snapshot() {}
    Snapshot(const std::string& name, const std::string& data)
        : name(name), data(data) {}
    Snapshot(const std::string& name, const std::string& description, std::string&& data)
        : name(name), description(description), data(std::move(data)) {}

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    void Serialize(std::ostream& out) const override;

    void Deserialize(const rapidjson::Value& in) override;
    void Deserialize(std::istream& in) override;

    const char* TypeName() const override { return "Snapshot"; }
};

VOID_NAMESPACE_CLOSE

#endif // _ENTITY_SNAPSHOT_H
