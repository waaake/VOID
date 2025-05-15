/* STD */
#include <sstream>
#include <algorithm>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Tools {

    std::string to_trimmed_string(double value, int precision = 1);

    template <typename Ty>
    int index_of(const std::vector<Ty>& vec, const Ty& value);

} // namespace Tools

VOID_NAMESPACE_CLOSE