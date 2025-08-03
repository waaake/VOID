// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_UNIQUE_IDENTITY_H
#define _VOID_UNIQUE_IDENTITY_H

/* Boost */
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * Describes a Unique Identity for Void Component Instances
 * This is primarily used across MediaClip, Sequence and Track instances
 * Making them distinguishable from the other entities
 */
class VUID
{
public:
    /* Generate a unique identifier */
    VUID() : m_uuid(m_gen()) {}

    /* Equatation */
    bool operator==(const VUID& other) const { return m_uuid == other.m_uuid; }

    /* For Representation */
    inline std::string str() const { return boost::uuids::to_string(m_uuid); }

private: /* Members */
    boost::uuids::random_generator m_gen;
    boost::uuids::uuid m_uuid;
};

// std::ostream& operator<<(std::ostream& stream, const VUID& v)
// {
//     /* Add a VUID identifier */
//     stream << "VUID{"<< v.str() <<"}";
//     return stream;
// }

VOID_NAMESPACE_CLOSE

#endif // _VOID_UNIQUE_IDENTITY_H
