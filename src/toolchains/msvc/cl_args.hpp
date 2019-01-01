#pragma once

#include <vector>
#include <filesystem>
#include <string>
#include "../../utils/str.hpp"

namespace pronto::toolchains::msvc
{
  struct cl_args
  {
    // /nologo
    bool no_logo = true;

    // /GR 	Enables run-time type information (RTTI). 'on' by default
    // /GR- disables
    bool rtti = false;

    // /link
    bool link = false;
    
    // /Fo"pathname"
    //If you do not use this option, the object file uses the base name of the source file and the .obj extension. 
    // You can use any name and extension you want, but the recommended convention is to use .obj.
    std::string object_file_name = str::empty_string;

    //  /Fe[pathname] /Fe: pathname
    // Specifies a name and a directory for the .exe file or DLL created by the compiler.
    // The /Fe option allows you to specify the output directory, output executable name, or both, for the generated executable file.
    // If pathname ends in a path separator(\), it is assumed to specify only the output directory.
    // Otherwise, the last component of pathname is used as the output file base name, and the rest of pathname specifies the output directory. 
    // If pathname does not have any path separators, it's assumed to specify the output file name in the current directory. 
    // The pathname must be enclosed in double quotes (") if it contains any characters that can't be in a short path,
    // such as spaces, extended characters, or path components more than eight characters long.
    //
    // When the /Fe option is not specified, or when a file base name is not specified in pathname,
    // the compiler gives the output file a default name using the base name of the first source or object file specified on the command line and the extension.exe or .dll.
    //
    // If you specify the /c(Compile Without Linking) option, /Fe has no effect.
    std::string exe_file_name = str::empty_string;

    // libs only makes sense with link
    std::vector<std::string> file_or_lib_paths;

    // /libpath requires /link first
    std::vector<std::string> lib_paths;

    std::filesystem::path cl_path;

    // /Za to turn off
    bool enable_microsoft_extensions = false;

    cl_args(std::filesystem::path cl_path_)
      :cl_path(cl_path_)
    {
    }

    std::string cmd_line()
    {
      std::string s = "";
      s += str::utf16_to_utf8(cl_path);

      if (no_logo)
        s += " /nologo";

      if (!rtti)
        s += " /GR-";
      
      if (object_file_name.length() > 0)
      {
        s += " /Fo: \"";
        s += object_file_name;
        s += "\"";
      }

      for (auto& file_or_lib_path : file_or_lib_paths)
      {
        s += " \"";
        s += file_or_lib_path;
        s += "\"";
      }

      if (!enable_microsoft_extensions)
        s += " /Za";

      if (link)
      {
        if (exe_file_name.length() > 0)
        {
          s += " /Fe: \"";
          s += exe_file_name;
          s += "\"";
        }

        s += " /link";
        for (auto& lib_path : lib_paths)
        {
          s += " /LIBPATH:\"";
          s += lib_path;
          s += "\"";
        }

        s += " /MACHINE:X64";
      }
      else
        s += " /c";


      return s;
    }
  };
}