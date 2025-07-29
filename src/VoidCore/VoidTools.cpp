/* STD */
#include <string_view>

/* Internal */
#include "VoidTools.h"

VOID_NAMESPACE_OPEN

namespace Tools {

    std::string to_trimmed_string(double value, int precision)
    {
        std::stringstream ss;

        /* Set the Precison for the out value*/
        ss.precision(precision);

        /* Add the value to the stream */
        ss << std::fixed << value;
        return ss.str();
    }

    std::string to_lower(const std::string& in)
    {
        /* Copy */
        std::string copied = in;

        /* Update the copied string */
        to_lower(copied);

        /* And return the transformed copied string*/
        return copied;
    }

    bool find_replace(std::string& text, const std::string& placeholder, const std::string& replacement)
    {
        /* String veiw for faster search */
        std::string_view view = text;
        size_t pos = view.find(placeholder);

        /* Placeholder found!! */
        if (pos != std::string_view::npos)
        {
            text.replace(pos, placeholder.size(), replacement.c_str());

            return true;
        }

        return false;
    }

    template <typename Ty>
    int index_of(const std::vector<Ty>& vec, const Ty& value)
    {
        auto it = std::find_if(vec.begin(), vec.end(), value);

        if (it != vec.end())
        {
            /* Return the index at which the iterator was able to find the value */
            return std::distance(vec.begin(), it);
        }

        /* Not able to find the value in the vector */
        return -1;
    }

} // namespace Tools

VOID_NAMESPACE_CLOSE
