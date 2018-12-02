#pragma once

#include <filesystem>
#include <optional>
#include <stdlib.h>
#include <optional>

#include "deps/cpptoml.hpp"

#include "compiler_toolchains.hpp"

#include "utils/result.hpp"
#include "env.hpp"
#include "console.hpp"

namespace pronto
{
  namespace fs = std::filesystem;

  struct config
  {
    static constexpr cstr const TOML_NOT_FOUND = "_pronto.toml have not been found.";
    static constexpr cstr const TOML_ALREADY_LOADED = "_pronto.toml have already been loaded.";
    static constexpr cstr const TOML_LOADED = "_pronto.toml have been loaded.";

    env env_;
    console console_;

    std::filesystem::path path_;

    bool loaded_;
       
    bool loaded() const { return loaded_; }

    const std::filesystem::path& toml_path() const { return path_; }

    config()    
    {
    }  
    
    void set_toolchain(compiler_toolchains toolchain)
    {
      auto str = compiler_toolchain_to_str(toolchain);

      env_.set_toolchain(str);
    }

    std::vector<std::string>&& list_toolchains()
    {
      return { "msvc", "intel", "clang", "gcc", "borland" };
    }

    result_err<std::string> load(const std::filesystem::path& path)
    {
      if (loaded_)
      {
        console_.inf(TOML_ALREADY_LOADED);
        return ok();
      }

      auto toml_path_l = find_toml(path);

      if (!toml_path_l.has_value())
      {
        console_.err(TOML_NOT_FOUND);
        return fail(std::string(TOML_NOT_FOUND));
      }

      path_ = toml_path_l.value();

      try
      {
        auto config_file = cpptoml::parse_file(path_.string());
        auto gl_version = config_file->get_qualified_as<std::string>("dependencies.gl");
      }
      catch (cpptoml::parse_exception& ex)
      {
        std::string s = "toml config: ";
        s += ex.what();
        console_.err(s.c_str());
        return fail(s);
      }

      loaded_ = true;
      console_.inf(TOML_LOADED);
      return ok();
    }

    result_err<std::string> load() { return load(std::filesystem::current_path()); }

    compiler_toolchains toolchain() {

      auto o_toolchain = compiler_toolchain_from(env_.toolchain());
      if (o_toolchain.has_value())
        return o_toolchain.value();
      else
        return compiler_toolchains::clang;
    }

  private:

    std::optional<fs::path> find_toml(const std::filesystem::path& path)
    {
      auto path_l = path;
      do
      {        
        auto filepath = path_l;
        filepath /= "_pronto.toml";

        if (fs::exists(filepath))
        {
          return filepath;
        }

        if (!path_l.has_parent_path())
        {
          return {};
        }

        path_l = path_l.parent_path();

      } while(true);
    }
  };
}