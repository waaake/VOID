// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string_view>

/* Internal */
#include "VoidTools.h"

VOID_NAMESPACE_OPEN

namespace Tools {

    #define _PADDING = '='
    static constexpr std::string_view s_Base64Table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string to_trimmed_string(double value, int precision)
    {
        std::stringstream ss;
        ss.precision(precision);
        ss << std::fixed << value;
        return ss.str();
    }

    std::string to_lower(const std::string& in)
    {
        std::string copied = in;
        to_lower(copied);
        return copied;
    }

    bool find_replace(std::string& text, const std::string& placeholder, const std::string& replacement)
    {
        std::string_view view = text;
        size_t pos = view.find(placeholder);
        if (pos != std::string_view::npos)
        {
            text.replace(pos, placeholder.size(), replacement.c_str());
            return true;
        }

        return false;
    }

    std::string timestamp()
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream os;
        os << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return os.str();
    }

    std::string timestamp(const std::string& prefix)
    {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream os;
        os << prefix << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return os.str();
    }

    std::string b64_encode(const std::string& binary)
    {
        std::string out;
        out.reserve(4 * ((binary.size() + 2) / 3));

        int value = 0;
        int bits = -6;

        for (unsigned char c : binary)
        {
            value = (value << 8) + c;
            bits += 8;

            while (bits >= 0)
            {
                out.push_back(s_Base64Table[(value >> bits) & 0x3F]);
                bits -=6;
            }
        }

        // remaining
        if (bits > -6)
            out.push_back(s_Base64Table[((value << 8) >> (bits + 8)) & 0x3F]);

        while (out.size() % 4)
            out.push_back(_PADDING);

        return out;
    }

    std::string b64_decode(const std::string& encoded)
    {
        std::string out;

        int value = 0;
        int bits = -8;

        for (unsigned char c : encoded)
        {
            if (c == _PADDING) break;

            std::size_t pos = s_Base64Table.find(c);
            if (pos == std::string_view::npos)
                continue;

            value = (value << 6) + static_cast<int>(pos);
            bits += 6;

            if (bits >= 0)
            {
                out.push_back(static_cast<char>((value >> bits) & 0xFF));
                bits -= 8;
            }
        }

        return out;
    }

    template <typename Ty>
    int index_of(const std::vector<Ty>& vec, const Ty& value)
    {
        auto it = std::find_if(vec.begin(), vec.end(), value);
        return it == vec.end() ? -1 : std::distance(vec.begin(), it);
    }

} // namespace Tools

VOID_NAMESPACE_CLOSE
