#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Engine
{
class String
{
  public:
    static bool BeginsWith(const std::string &string, const std::string &begin);
    static bool EndsWith(const std::string &string, const std::string_view &end);
    static bool IsDigit(const char &character);
    static std::string RemoveWhiteSpace(const std::string &string);
    static std::string Sanitize(const std::string &keyword);
    static std::vector<std::string> Split(const std::string &string, char delimiter);

    static float ToFloat(const std::string &string);
    static std::string ToUpper(const std::string &string);
    static std::string ToLower(const std::string &string);
};
} // namespace Engine