#pragma once

#include <string>
#include <optional>

#include <stdio.h>


#include <unicode/utypes.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>


namespace pronto::str
{
  std::string empty_string = "";

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
    //std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conversion;
    //return conversion.to_bytes(u"\u4f60\u597d");
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
