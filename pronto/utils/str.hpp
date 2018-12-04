#pragma once

#include <string>

namespace pronto::str
{
  std::wstring utf8_to_utf16(const std::string& utf8)
  {
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
        unsigned char ch = utf8[i++];
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
    for (size_t i = 0; i < unicode.size(); ++i)
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

  std::wstring str2wstr(const std::string& str)
  {
    //using convert_typeX = std::codecvt_utf8<wchar_t>;
    //std::wstring_convert<convert_typeX, wchar_t> converterX;
    
    //return converterX.from_bytes(str);

    return utf8_to_utf16(str);
  }

  //std::wstring FromUtf8(const std::string& utf8string)
  //{
  //  size_t widesize = utf8string.length();
  //  if (sizeof(wchar_t) == 2)
  //  {
  //    std::wstring resultstring;
  //    resultstring.resize(widesize + 1, L'\0');
  //    const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
  //    const UTF8* sourceend = sourcestart + widesize;
  //    UTF16* targetstart = reinterpret_cast<UTF16*>(&resultstring[0]);
  //    UTF16* targetend = targetstart + widesize;
  //    ConversionResult res = ConvertUTF8toUTF16
  //    (&sourcestart, sourceend, &targetstart, targetend, strictConversion);
  //    if (res != conversionOK)
  //    {
  //      throw std::exception("La falla!");
  //    }
  //    *targetstart = 0;
  //    return resultstring;
  //  }
  //  else if (sizeof(wchar_t) == 4)
  //  {
  //    std::wstring resultstring;
  //    resultstring.resize(widesize + 1, L'\0');
  //    const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
  //    const UTF8* sourceend = sourcestart + widesize;
  //    UTF32* targetstart = reinterpret_cast<UTF32*>(&resultstring[0]);
  //    UTF32* targetend = targetstart + widesize;
  //    ConversionResult res = ConvertUTF8toUTF32
  //    (&sourcestart, sourceend, &targetstart, targetend, strictConversion);
  //    if (res != conversionOK)
  //    {
  //      throw std::exception("La falla!");
  //    }
  //    *targetstart = 0;
  //    return resultstring;
  //  }
  //  else
  //  {
  //    throw std::exception("La falla!");
  //  }
  //  return L"";
  //}

  //std::string ToUtf8(const std::wstring& widestring)
  //{
  //  size_t widesize = widestring.length();

  //  if (sizeof(wchar_t) == 2)
  //  {
  //    size_t utf8size = 3 * widesize + 1;
  //    std::string resultstring;
  //    resultstring.resize(utf8size, '\0');
  //    const wchar_t* sourcestart =
  //      reinterpret_cast<const wchar_t*>(widestring.c_str());
  //    const wchar_t* sourceend = sourcestart + widesize;
  //    UTF8* targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
  //    UTF8* targetend = targetstart + utf8size;
  //    ConversionResult res = ConvertUTF16toUTF8
  //    (&sourcestart, sourceend, &targetstart, targetend, strictConversion);
  //    if (res != conversionOK)
  //    {
  //      throw std::exception("La falla!");
  //    }
  //    *targetstart = 0;
  //    return resultstring;
  //  }
  //  else if (sizeof(wchar_t) == 4)
  //  {
  //    size_t utf8size = 4 * widesize + 1;
  //    std::string resultstring;
  //    resultstring.resize(utf8size, '\0');
  //    const UTF32* sourcestart =
  //      reinterpret_cast<const UTF32*>(widestring.c_str());
  //    const UTF32* sourceend = sourcestart + widesize;
  //    UTF8* targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
  //    UTF8* targetend = targetstart + utf8size;
  //    ConversionResult res = ConvertUTF32toUTF8
  //    (&sourcestart, sourceend, &targetstart, targetend, strictConversion);
  //    if (res != conversionOK)
  //    {
  //      throw std::exception("La falla!");
  //    }
  //    *targetstart = 0;
  //    return resultstring;
  //  }
  //  else
  //  {
  //    throw std::exception("La falla!");
  //  }
  //  return "";
  //}

  //std::string wstr2str(const std::wstring& wstr)
  //{
  //  using convert_typeX = std::codecvt_utf8<wchar_t>;
  //  std::wstring_convert<convert_typeX, wchar_t> converterX;

  //  return converterX.to_bytes(wstr);
  //}
}
