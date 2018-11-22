#pragma once


#include <iostream>
#include <cmath>
#include "core.hpp"

namespace pronto
{
  struct console {

  private:
    enum class color_mode
    {
      fore = 38,
      back = 48
    };

    void set_color(color_mode fore_or_back, u8 r, u8 g, u8 b) const
    {
      int chars_count = /*"\033[38;2;" and "m" and extra 0 */9;

      int r_char_count = 1;
      int g_char_count = 1;
      int b_char_count = 1;

      if (r > 99) r_char_count = 3; elif(r > 9)  r_char_count = 2;
      if (g > 99) g_char_count = 3; elif(g > 9)  g_char_count = 2;
      if (b > 99) b_char_count = 3; elif(b > 9)  b_char_count = 2;

      chars_count += r_char_count + g_char_count + b_char_count + 2/* ";" and ";" */;
      std::string s(chars_count, '\0');
      auto addr = s.data();
      memcpy(addr, "\033[", 2);
      addr += 2;

      _itoa_s((u8)fore_or_back, addr, 3, 10);
      addr += 2;

      memcpy(addr, ";2;", 3);
      addr += 3;

      _itoa_s(r, addr, 4, 10);
      addr += r_char_count;
      addr[0] = ';';
      addr++;

      _itoa_s(g, addr, 4, 10);
      addr += g_char_count;
      addr[0] = ';';
      addr++;

      _itoa_s(b, addr, 4, 10);
      addr += b_char_count;

      addr[0] = 'm';
      addr[1] = 0;

      std::cout.write(s.c_str(), s.length());
    }

    void reset_color() const
    {
      std::cout.write("\033[0m", 4);
    }

  public:
    void err(readonly cstr s) const
    {
      set_color(color_mode::fore, 255u, 148u, 21u);

      std::cout.write(s, std::strlen(s));

      reset_color();
      std::cout.write("\n", 1);
    }

    void inf(readonly cstr s) const
    {
      set_color(color_mode::fore, 255u, 255u, 255u);

      std::cout.write(s, std::strlen(s));

      reset_color();
      std::cout.write("\n", 1);
    }
  };
}