#pragma once

#include <string>
#include <optional>
#include <regex>
#include <algorithm>
#include <filesystem>

#include <stdio.h>

#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>

namespace pronto::str
{
  const std::string empty_string = "";

  const std::string reserved = "#$reserved$#";

#if PLATFORM_WINDOWS
  const std::string new_line = "\r\n";
#else
  const std::string new_line = "\n";
#endif

  std::wstring utf8_to_utf16(const std::string& utf8)
  {
    // TODO: replace with icu code

    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
      unsigned long uni;
      size_t todo;
     // bool error = false;
      unsigned char ch = utf8[i++];
      if (ch <= 0x7F)
      {
        uni = ch;
        todo = 0;
      }
      else if (ch <= 0xBF)
      {
        throw std::logic_error("not a UTF-8 string");
      }
      else if (ch <= 0xDF)
      {
        uni = ch & 0x1F;
        todo = 1;
      }
      else if (ch <= 0xEF)
      {
        uni = ch & 0x0F;
        todo = 2;
      }
      else if (ch <= 0xF7)
      {
        uni = ch & 0x07;
        todo = 3;
      }
      else
      {
        throw std::logic_error("not a UTF-8 string");
      }
      for (size_t j = 0; j < todo; ++j)
      {
        if (i == utf8.size())
          throw std::logic_error("not a UTF-8 string");
        ch = utf8[i++];
        if (ch < 0x80 || ch > 0xBF)
          throw std::logic_error("not a UTF-8 string");
        uni <<= 6;
        uni += ch & 0x3F;
      }
      if (uni >= 0xD800 && uni <= 0xDFFF)
        throw std::logic_error("not a UTF-8 string");
      if (uni > 0x10FFFF)
        throw std::logic_error("not a UTF-8 string");
      unicode.push_back(uni);
    }

    std::wstring utf16;
    for (i = 0; i < unicode.size(); ++i)
    {
      unsigned long uni = unicode[i];
      if (uni <= 0xFFFF)
      {
        utf16 += (wchar_t)uni;
      }
      else
      {
        uni -= 0x10000;
        utf16 += (wchar_t)((uni >> 10) + 0xD800);
        utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
      }
    }
    return utf16;
  }

  void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t pos = str.find(from);

    while (true)
    {
      if (pos == std::string::npos)
        return;

      str.replace(pos, from.length(), to);
      pos += to.length();

      pos = str.find(from, pos);
    }
  }

  std::string wildcard_to_regex(const char* wildcard)
  {
    std::string inner_expr = wildcard;  

    //replace_all(inner_expr, "\\", "\\\\");
    replace_all(inner_expr, "|", "\\|");
    replace_all(inner_expr, "?", "\\?");
    replace_all(inner_expr, "+", "\\+");
    replace_all(inner_expr, ".", "\\.");
    replace_all(inner_expr, "^", "\\^");
    replace_all(inner_expr, "$", "\\$");
    replace_all(inner_expr, "(", "\\(");
    replace_all(inner_expr, ")", "\\)");
    
    replace_all(inner_expr, "*", "(.*)");

    if (inner_expr.find(reserved) != std::string::npos)
      throw std::runtime_error("using reserved character sequence");

    replace_all(inner_expr, "\\]", reserved);
    replace_all(inner_expr, "]", "]*");
    replace_all(inner_expr, reserved, "\\]");


    std::string expr = "^";
    expr += inner_expr;
    expr += "$";
    return expr;
  }

  bool wildcard_match(const char* wildcard, const std::string& c_str)
  {
    std::regex reg_expr(wildcard_to_regex(wildcard));

    std::smatch m;
    return std::regex_match(c_str, m, reg_expr);
  }

  bool wildcard_match(const char *wildcard, const char* c_str)
  {
    auto str = std::string(c_str);
    return wildcard_match(wildcard, str);
  }

  bool wildcard_path_match(const char *wildcard, const std::filesystem::path& filepath)
  {
    auto str = filepath.u8string();

    replace_all(str, "\\", "/");

    return wildcard_match(wildcard, str);
  }

  bool starts_with(const std::string& s, const std::string& seq)
  {
    return s.compare(0, seq.length(), seq) == 0;
  }

  std::string utf16_to_utf8(const std::wstring& widestring)
  {
    (widestring);
    
    icu::UnicodeString str(widestring.c_str());    
    std::string s;
    str.toUTF8String(s);

    return s;
  }

  std::optional<double> str_to_double(std::string& s)
  {
    std::istringstream i(s);
    double x;
    if (!(i >> x))
      return {};
    return x;
  }

  //std::string wstr2str(const std::wstring& wstr)
  //{
  //  using convert_typeX = std::codecvt_utf8<wchar_t>;
  //  std::wstring_convert<convert_typeX, wchar_t> converterX;

  //  return converterX.to_bytes(wstr);
  //}
}
