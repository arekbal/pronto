#pragma once

#include "../utils/span.hpp"
#include "../console.hpp"
#include "../config.hpp"
#include "../utils/str.hpp"

namespace pronto::commands
{ 
  struct build_command
  {
    static constexpr cstr const command_name = "build";

  private:   

    console console_;
    config config_;

  public:  

    int execute(const utils::cspan_vec_s command_args)
    {
      if (command_args.empty())
      {
        // TODO: just build from config

        auto res = config_.load();

        return 0;
      }

      // TODO: check for extra path argument before building
      auto iter = command_args.begin();
      auto end = command_args.end();

      auto& first = *iter;

      if (!str::starts_with(first, "-"))
      {
        auto res = config_.load(first);



        return 0;
      }
      
     /* if (first.compare("-p") == 0 || first.compare("-profile") == 0)
      {
        iter++;
        
        if (iter != end)
        {
          auto& config_name = *iter;
          config_name.size();
        }
      }*/

      return 0;
    }   
  }; 
}