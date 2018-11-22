#pragma once



#include "../utils/span.hpp"
#include "../console.hpp"
#include "../config.hpp"

namespace pronto::commands
{ 
  struct build_command
  {
    static constexpr cstr readonly command_name = "build";

  private:
    static constexpr cstr readonly toml_not_found = "_pronto.toml have not been found.";
    
    console console_;
    config config_;

  public:
    int execute(readonly utils::cspan_vec_s commandArgs)
    {
      auto toml_path = config_.find_toml();

      try
      {
        auto config = cpptoml::parse_file(toml_path.value().string());
        auto gl_version = config->get_qualified_as<std::string>("dependencies.gl");
      }
      catch (cpptoml::parse_exception& ex)
      {
        std::string s = "toml config: ";
        s += ex.what();
        console_.err(s.c_str());
      }

      auto res = config_.open();

      auto x = res.ok();

      if (commandArgs.empty())
      {
        // TODO: look for config file in curr dir and go up, if not found.
        return 0;
      }

      console_.err(toml_not_found);

      auto iter = commandArgs.begin();
      auto end = commandArgs.end();

      auto& first = *iter;
      
      if (first.compare("-p") == 0 || first.compare("-profile") == 0)
      {
        iter++;
        
        if (iter != end)
        {
          auto& config_name = *iter;
          config_name.size();
        }
      }

      return 0;
    }

   
  }; 
}