#pragma once

#include <filesystem>
#include <optional>
#include <stdlib.h>
#include <optional>

#include "deps/cpptoml.hpp"

#include "toolchains/toolchain_names.hpp"

#include "result.hpp"
#include "env.hpp"
#include "console.hpp"

namespace pronto
{
  namespace fs = std::filesystem;

  template<class env_t=env, class console_t=console>
  struct config
  {
    static constexpr const char* const TOML_NOT_FOUND = "_pronto.toml have not been found.";
    static constexpr const char* const TOML_ALREADY_LOADED = "_pronto.toml have already been loaded.";
    static constexpr const char* const TOML_LOADED = "_pronto.toml have been loaded.";

    static constexpr const char* const COMPILATION_TOOLCHAIN = "compilation.toolchain";
    static constexpr const char* const COMPILATION_SOURCES = "compilation.sources";
    static constexpr const char* const COMPILATION_EXCLUDES = "compilation.excludes";
    static constexpr const char* const COMPILATION_TARGET_DIR = "compilation.target_dir";

    static constexpr const char* const COMPILATION_TARGET_DIR_DEFAULT = "target";

    static constexpr const char* const PACKAGE_NAME = "package.name";
    static constexpr const char* const PACKAGE_VERSION = "package.version";
    static constexpr const char* const PACKAGE_AUTHORS = "package.authors";
    static constexpr const char* const PACKAGE_PUBLISH = "package.publish";

    static constexpr const char* const TOML_FILE_NAME = "_pronto.toml";

  private:
    env_t env_;
    console_t console_;

    std::filesystem::path path_;

    std::shared_ptr<cpptoml::table> config_file_;

    bool loaded_;

  public:
    bool loaded() const { return loaded_; }

    const fs::path& toml_path() const { return path_; }

    fs::path root_path() const { return path_.parent_path(); }

    config()    
    {
    }
    
    void set_toolchain(toolchains::toolchain_names toolchain)
    {
      auto str = toolchains::toolchain_to_str(toolchain);

      env_.set_toolchain(str);
    }

    const std::vector<std::string> list_toolchains() const
    {
      return { "msvc", "intel", "clang", "gcc", "borland" };
    }

    result_err<std::string> load(const std::filesystem::path& path)
    {
      if (loaded_)
      {
        console_.wrn(TOML_ALREADY_LOADED);
        return ok();
      }

      auto toml_path_l = find_toml(path);

      if (!toml_path_l.has_value())
      {
        return fail<std::string>(TOML_NOT_FOUND);
      }

      path_ = toml_path_l.value();

      try
      {
        config_file_ = cpptoml::parse_file(path_.string());
      }
      catch (cpptoml::parse_exception& ex)
      {
        std::string s = "toml config: ";
        s += ex.what();

        return fail<std::string>(s);
      }

      loaded_ = true;
      console_.inf(TOML_LOADED);
      return ok();
    }

    result_err<std::string> load() { return load(std::filesystem::current_path()); }
    
    toolchains::toolchain_names toolchain() 
    {
      auto o_toolchain_entry = config_file_->get_qualified_as<std::string>(COMPILATION_TOOLCHAIN);

      if (o_toolchain_entry)
      {
        auto o_toolchain = toolchains::toolchain_from(*o_toolchain_entry);

        if (o_toolchain.has_value())
          return o_toolchain.value();
      }        

      {
        auto o_toolchain = toolchains::toolchain_from(env_.toolchain());

        if (o_toolchain.has_value())
          return o_toolchain.value();
        else
          return toolchains::toolchain_names::clang;
      }
    }

    const std::vector<std::string> sources() 
    {
      auto o_sources_entry = config_file_->get_qualified_array_of<std::string>(COMPILATION_SOURCES);
      
      if (o_sources_entry && !o_sources_entry->empty())
        return *o_sources_entry;
       
      return { std::string("*.cpp"), std::string("*.cc") };
    }

    const std::vector<std::string> excludes() 
    {
      auto o_excludes_entry = config_file_->get_qualified_array_of<std::string>(COMPILATION_EXCLUDES);      

      if (o_excludes_entry && !o_excludes_entry->empty())
        return *o_excludes_entry;

      return {};
    }

    const std::vector<std::string> package_authors() 
    {
      auto o_package_authors_entry = config_file_->get_qualified_array_of<std::string>(PACKAGE_AUTHORS);

      if (o_package_authors_entry && !o_package_authors_entry->empty())
        return *o_package_authors_entry;

      return {};
    }

    const std::filesystem::path target_dir()
    {
      auto o_target_dir_entry = config_file_->get_qualified_as<std::string>(COMPILATION_TARGET_DIR);

      if (o_target_dir_entry && !o_target_dir_entry->empty())
        return std::filesystem::path(*o_target_dir_entry);

      return std::filesystem::path(COMPILATION_TARGET_DIR_DEFAULT);
    }

    const std::string package_name()
    {
      auto o_package_name_entry = config_file_->get_qualified_as<std::string>(PACKAGE_NAME);
    
      if (o_package_name_entry && !o_package_name_entry->empty())
        return *o_package_name_entry;

      return "";
    }

    const std::string package_version() 
    {
      auto o_package_version_entry = config_file_->get_qualified_as<std::string>(PACKAGE_VERSION);

      if (o_package_version_entry && !o_package_version_entry->empty())
        return *o_package_version_entry;

      return "";
    }

    bool package_publish() 
    {
      auto o_package_publish_entry = config_file_->get_qualified_as<bool>(PACKAGE_PUBLISH);

      if (o_package_publish_entry)
        return *o_package_publish_entry;

      return false;
    }

  private:

    std::optional<fs::path> find_toml(const std::filesystem::path& path)
    {
      auto path_l = path;
      do
      {        
        auto filepath = path_l;
        filepath /= TOML_FILE_NAME;

        if (fs::exists(filepath))
        {
          return std::filesystem::absolute(filepath);
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