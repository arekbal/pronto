#pragma once

#include <string>

namespace pronto::str
{
  bool starts_with(const std::string& s, const std::string& seq)
  {
    return s.compare(0, seq.length(), seq) == 0;
  }

  std::wstring str2wstr(const std::string& str)
  {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
  }

  std::string wstr2str(const std::wstring& wstr)
  {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
  }
}
