// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <chrono>
#include <type_traits>

/* Internal */
#include "Definition.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

namespace Tools
{
    template <typename DurationType = std::chrono::seconds>
    class VOID_API VoidProfiler
    {
    public:
        VoidProfiler(const std::string& name)
            : m_Name(name)
            , m_Start(std::chrono::high_resolution_clock::now()) { }
        ~VoidProfiler() { TimeIt(m_Name); }

        void TimeIt(const std::string& name)
        {
            /* The Delta */
            DurationType delta = std::chrono::duration_cast<DurationType>(std::chrono::high_resolution_clock::now() - m_Start);

            VOID_LOG_INFO("Profiler::{0}::Time::{1} {2}", name, delta.count(), Unit());
        }

    private:
        std::string m_Name;
        std::chrono::high_resolution_clock::time_point m_Start;
    
    private: /* Methods */
        std::string Unit() const
        {
            if constexpr( std::is_same_v<DurationType, std::chrono::seconds> )
                return "s";
            if constexpr ( std::is_same_v<DurationType, std::chrono::microseconds> )
                return "μs";
            if constexpr ( std::is_same_v<DurationType, std::chrono::milliseconds> )
                return "ms";
            if constexpr ( std::is_same_v<DurationType, std::chrono::nanoseconds> )
                return "ns";

            // duration<double>
            return "s";
        }
    };

} // namespace Tools

VOID_NAMESPACE_CLOSE
