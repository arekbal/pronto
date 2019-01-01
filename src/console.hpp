#pragma once


#include <iostream>
#include <cmath>
#include "core.hpp"

namespace pronto
{
  struct console_impl
  {
  };
    
  class console {

    struct console_indentation_scope
    {
      console* p_console_;

      console_indentation_scope(console* p_console)
        :p_console_(p_console)
      {
        (*p_console_)++;
      }

      ~console_indentation_scope()
      {
        (*p_console_)--;
      }
    };

    int indent_ = 0;

    enum class color_mode
    {
      fore = 38,
      back = 48
    };

    void set_color(color_mode fore_or_back, u8 r, u8 g, u8 b)
    {
      int chars_count = /*"\033[38;2;" and "m" and extra 0 */9;
           
      int r_char_count = 1;
      int g_char_count = 1;
      int b_char_count = 1;

      if (r > 99) r_char_count = 3; else if(r > 9)  r_char_count = 2;
      if (g > 99) g_char_count = 3; else if(g > 9)  g_char_count = 2;
      if (b > 99) b_char_count = 3; else if(b > 9)  b_char_count = 2;

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
    bool no_color;

    int err(const char* s, bool new_line = true)
    {
      if (indent_ > 0)
      {
        std::string indentation(indent_ * 2, ' ');
        std::cout.write(indentation.c_str(), indent_ * 2);
      }

      if(!no_color)
        set_color(color_mode::fore, 255u, 148u, 21u);
            
      std::cout.write(s, std::strlen(s));

      if (!no_color)
        reset_color();

      if(new_line)
        std::cout.write("\n", 1);

      return -2;
    }

    int inf(const char* s, bool new_line = true)
    {
      if (indent_ > 0)
      {
        std::string indentation(indent_ * 2, ' ');
        std::cout.write(indentation.c_str(), indent_ * 2);
      }

      if (!no_color)
        set_color(color_mode::fore, 230u, 230u, 230u);

      std::cout.write(s, std::strlen(s));

      if (!no_color)
        reset_color();

      if (new_line)
        std::cout.write("\n", 1);

      return 0;
    }

    int wrn(const char* s, bool new_line = true)
    {
      if (indent_ > 0)
      {
        std::string indentation(indent_ * 2, ' ');
        std::cout.write(indentation.c_str(), indent_ * 2);
      }

      if (!no_color)
        set_color(color_mode::fore, 255u, 224u, 89u);

      std::cout.write(s, std::strlen(s));

      if (!no_color)
        reset_color();

      if (new_line)
        std::cout.write("\n", 1);

      return -1;
    }

    int ok(const char* s, bool new_line=true)
    {
      if (indent_ > 0)
      {
        std::string indentation(indent_ * 2, ' ');
        std::cout.write(indentation.c_str(), indent_ * 2);
      }

      if (!no_color)
        set_color(color_mode::fore, 72u, 224u, 89u);

      std::cout.write(s, std::strlen(s));

      if (!no_color)
        reset_color();

      if (new_line)
        std::cout.write("\n", 1);

      return 1;
    }

    console& operator++(int)
    {
      indent_++;
      return *this;
    }

    console& operator--(int)
    {
      indent_--;
      return *this;
    }

    console_indentation_scope indent()
    {
      return console_indentation_scope(this);
    }

    // extension overloads...
    int err(const std::string& u8string)
    {
      return err(u8string.c_str());
    }

    int inf(const std::string& u8string)
    {
      return inf(u8string.c_str());
    }

    int wrn(const std::string& u8string) 
    {
      return wrn(u8string.c_str());
    }    

    int ok(const std::string& u8string)
    {
      return ok(u8string.c_str());
    }
  };
}