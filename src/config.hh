#pragma once

#include <filesystem>
#include <optional>
#include <stdlib.h>
#include <optional>
#include <map>
#include <vector>
#include <string>

#include "deps/cpptoml.hh"

#include "toolchains/toolchain_names.hh"

#include "result.hh"
#include "env.hh"
#include "console.hh"
#include "utils/semver.hh"
#include "utils/semver_req.hh"
#include "utils/iter.hh"

#include "dependency.hh"


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
    static constexpr const char* const PACKAGE_INCLUDES = "package.includes";

    static constexpr const char* const DEPENDENCIES = "dependencies";
    static constexpr const char* const BUILD_DEPENDENCIES = "build-dependencies";
    static constexpr const char* const DEV_DEPENDENCIES = "dev-dependencies";

    static constexpr const char* const DEPENDENCY_VERSION = "version";

    static constexpr const char* const DEPENDENCY_GIT = "git";
    static constexpr const char* const DEPENDENCY_GIT_BRANCH = "branch";

    static constexpr const char* const DEPENDENCY_GIT_BRANCH_DEFAULT = "master";

    static constexpr const char* const DEPENDENCY_PATH = "path";

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

      auto toml_path_l = find_inner_toml(path);

      if (not toml_path_l.has_value())
      {
        toml_path_l = find_outer_toml(path);

        if (not toml_path_l.has_value())
        {
          return fail<std::string>(TOML_NOT_FOUND);
        }
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

    const std::vector<std::string> sources() const
    {
      auto o_sources_entry = config_file_->get_qualified_array_of<std::string>(COMPILATION_SOURCES);
      
      if (o_sources_entry && !o_sources_entry->empty())
        return *o_sources_entry;
       
      return { std::string("*.cpp"), std::string("*.cc") };
    }

    const std::vector<std::string> excludes() const
    {
      auto o_excludes_entry = config_file_->get_qualified_array_of<std::string>(COMPILATION_EXCLUDES);      

      if (o_excludes_entry && !o_excludes_entry->empty())
        return *o_excludes_entry;

      return {};
    }

    const std::filesystem::path target_dir() const
    {
      auto o_target_dir_entry = config_file_->get_qualified_as<std::string>(COMPILATION_TARGET_DIR);

      if (o_target_dir_entry && !o_target_dir_entry->empty())
        return std::filesystem::path(*o_target_dir_entry);

      return std::filesystem::path(COMPILATION_TARGET_DIR_DEFAULT);
    }

    const std::string package_name() const
    {
      auto o_package_name_entry = config_file_->get_qualified_as<std::string>(PACKAGE_NAME);
    
      if (o_package_name_entry && !o_package_name_entry->empty())
        return *o_package_name_entry;

      return "";
    }

    const utils::semver package_version() const
    {
      auto o_package_version_entry = config_file_->get_qualified_as<std::string>(PACKAGE_VERSION);

      if (o_package_version_entry && !o_package_version_entry->empty())
        return utils::semver(*o_package_version_entry);

      return utils::semver();
    }

    const std::vector<std::string> package_authors() const
    {
      auto o_package_authors_entry = config_file_->get_qualified_array_of<std::string>(PACKAGE_AUTHORS);

      if (o_package_authors_entry && !o_package_authors_entry->empty())
        return *o_package_authors_entry;

      return {};
    }

    const std::vector<std::string> package_includes() const
    {
      auto o_package_includes_entry = config_file_->get_qualified_array_of<std::string>(PACKAGE_INCLUDES);

      if (o_package_includes_entry && !o_package_includes_entry->empty())
        return *o_package_includes_entry;

      return {};
    }

    bool package_publish() const
    {
      auto o_package_publish_entry = config_file_->get_qualified_as<bool>(PACKAGE_PUBLISH);

      if (o_package_publish_entry)
        return *o_package_publish_entry;

      return false;
    }

    std::vector<dependency> dependencies() const
    {
      return get_dependencies(DEPENDENCIES);
    }

    std::vector<dependency> build_dependencies() const
    {
      return get_dependencies(BUILD_DEPENDENCIES);
    }

    std::vector<dependency> dev_dependencies() const
    {
      return get_dependencies(DEV_DEPENDENCIES);
    }

  private:

    std::vector<dependency> get_dependencies(const std::string& key) const
    {
      auto o_deps = config_file_->get_table(key);

      std::vector<dependency> vec;

      for (auto& dep : *o_deps)
      {
        auto& key = dep.first;
        if (dep.second->is_value())
        {
          auto ver_req = dep.second->as<std::string>()->get();
          vec.emplace_back(key, utils::semver_req(ver_req));
        }
        else
        {
          auto dep_items = dep.second->as_table();

          auto o_ver_req = dep_items->get_as<std::string>(DEPENDENCY_VERSION);
          if (o_ver_req)
            vec.emplace_back(key, utils::semver_req(*o_ver_req));
          else
            vec.emplace_back(key, utils::semver_req("*"));

          {
            auto o_git = dep_items->get_as<std::string>(DEPENDENCY_GIT);
            auto o_branch = dep_items->get_as<std::string>(DEPENDENCY_GIT_BRANCH);

            if (o_git)
            {
              vec.back().git = *o_git;
              vec.back().git_branch = o_branch ? *o_branch : DEPENDENCY_GIT_BRANCH_DEFAULT;
            }
          }

          {
            auto o_path = dep_items->get_as<std::string>(DEPENDENCY_PATH);
            if (o_path)
              vec.back().path = *o_path;
          }
        }
      }
      return vec;
    }

    std::optional<fs::path> find_outer_toml(const std::filesystem::path& path)
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

        if (not path_l.has_parent_path())
        {
          return {};
        }

        path_l = path_l.parent_path();

      } while(true);
    }

    std::optional<fs::path> find_inner_toml(const std::filesystem::path& path)
    {
      auto path_l = path;
      do
      {
        auto filepath = path_l;
        filepath /= TOML_FILE_NAME;

        if (fs::exists(filepath))
        {
          return std::filesystem::absolute(filepath);        }
        
        auto dir_iter = std::filesystem::directory_iterator(path_l); 

        int dir_count = 0;
        for (auto& dir : dir_iter)
        {
          dir_count++;
          path_l = dir;
          if(dir_count > 1)
            return {};
        }

        if (dir_count == 0)
          return {};

      } while (true);
    }
  };
}