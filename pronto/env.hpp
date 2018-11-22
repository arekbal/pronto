#pragma once

#include <stdio.h>

namespace pronto
{
  struct env
  {
    constexpr static const char* const toolchain = "PRONTO_TOOLCHAIN";
    constexpr static const char* const target = "PRONTO_TARGET";
    constexpr static const char* const install_dir = "PRONTO_INSTALL_DIR";

    bool set_toolchain()
    {
      return true;
    }


  private:

    int get(const char* name)
    {
      auto ::std::getenv(name);

#if (::_dupenv_s)
      
#endif
    }

    int set(const char* name, const char* value, int overwrite)
    {     

#if !(::setenv)
      int errcode = 0;
      if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) return errcode;
      }

      return _putenv_s(name, value);
#else
      return ::std::setenv(name, value, overwrite);
#endif
    }
  };
}
