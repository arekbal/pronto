#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include "utils/str.hpp"

namespace pronto
{
  struct env
  {
    constexpr static const char* const TOOLCHAIN = "PRONTO_TOOLCHAIN";
    constexpr static const char* const TARGET = "PRONTO_TARGET";
    constexpr static const char* const INSTALL_DIR = "PRONTO_INSTALL_DIR";

    void set_toolchain(std::string val)
    {
      set(TOOLCHAIN, val.c_str(), true);
    }
    std::string toolchain()
    {
      return get(TOOLCHAIN);
    }

    void set_target(std::string val)
    {
      set(TARGET, val.c_str(), true);
    }
    std::string target()
    {
      return get(TARGET);
    }

    void set_install_dir(std::string val)
    {
      set(INSTALL_DIR, val.c_str(), true);
    }

    std::string install_dir()
    {
      return get(INSTALL_DIR);
    }
    
  private:
    // not thread-safe could use 'getenv' internally which could invalidate returned ptr on subsequent calls.
    std::string get(const char* name)
    {

#if (_dupenv_s)
      {
        char *pValue;
        size_t len;
        errno_t err = _dupenv_s(&pValue, &len, name);

        if (err)
          return -1;
               
        free(pValue);
      }
#else  
      const char* p = ::std::getenv(name);
      if(p != nullptr)
        return std::string(p);

      return str::empty_string;
#endif
    }

    int set(const char* name, const char* value, int overwrite)
    {
#if !setenv
      {
        int errcode = 0;
        if (!overwrite) {
          size_t envsize = 0;
          errcode = getenv_s(&envsize, nullptr, 0, name);
          if (errcode || envsize) return errcode;
        }

        return _putenv_s(name, value);
      }
#else
      return setenv(name, value, overwrite);
#endif
    }
  };
}
