#ifndef __FORMATTERS_HPP__
#define __FORMATTERS_HPP__

#include <format>
#include <exception>


template <>
struct std::formatter<std::vector<double>>
{
    std::string format_str = "{:.2f}"; // Default format string (now std::string)

    constexpr auto parse(std::format_parse_context &ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();

        if (it != end && *it == ':')
        {
            ++it;
            format_str.clear(); // Clear any previous format string
            while (it != end && *it != '}')
            {
                format_str += *it; // Now correct: appending to std::string
                ++it;
            }
        }
        if (it != end && *it == '}')
        {
            ++it;
        }
        else
        {
            throw std::format_error("invalid format string for std::vector<double>");
        }

        ctx.advance_to(it);
        return it;
    }

    auto format(const std::vector<double>& vec, std::format_context& ctx) {
        auto out = ctx.out();

        if (!vec.empty()) {
            for (size_t i = 0; i < vec.size() - 1; ++i) {
                out = std::vformat_to(out, std::string_view(format_str), std::make_format_args(vec[i])); // Correct usage!
                out = std::format_to(out, ", ");
            }
            out = std::vformat_to(out, std::string_view(format_str), std::make_format_args(vec.back())); // Correct usage!
        }
        return out;
    }
};



#endif