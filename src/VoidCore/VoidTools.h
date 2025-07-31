/* STD */
#include <algorithm>
#include <sstream>
#include <vector>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Tools {

    VOID_API std::string to_trimmed_string(double value, int precision = 1);
    /**
     * Copies and returns the lower cased string
     */
    VOID_API std::string to_lower(const std::string& in);
    /**
     * Updates the reference to the provided string to make it lower case
     */
    inline VOID_API void to_lower(std::string& in) { std::transform(in.begin(), in.end(), in.begin(), [](unsigned char c) { return std::tolower(c); }); }

    VOID_API bool find_replace(std::string& text, const std::string& placeholder, const std::string& replacement);

    template <typename Ty>
    int index_of(const std::vector<Ty>& vec, const Ty& value);

} // namespace Tools

VOID_NAMESPACE_CLOSE