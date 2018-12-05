#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include "commands/build_command.hpp"
#include "commands/run_command.hpp"
#include "commands/help_command.hpp"
#include "commands/get_command.hpp"
#include "commands/set_command.hpp"
#include "utils/span.hpp"

namespace pronto 
{  
  class command_selector
  {    
    std::vector<std::string> tokens_;

    std::variant<
      commands::build_command<>,
      commands::run_command,
      commands::get_command<>,
      commands::set_command<>,
      commands::help_command>
      selected_;

  public: 
    command_selector(int &argc, char **argv)
    {
      for (int i = 1; i < argc; ++i)
        tokens_.emplace_back(argv[i]);
      
      if (tokens_.size() > 0)
      {        
        auto token = tokens_[0];
        if (0 == token.compare(commands::build_command<>::command_name)) selected_.emplace<commands::build_command<>>();
        else if (0 == token.compare(commands::run_command::command_name)) selected_.emplace<commands::run_command>();
        else if (0 == token.compare(commands::get_command<>::command_name)) selected_.emplace<commands::get_command<>>();
        else if (0 == token.compare(commands::set_command<>::command_name)) selected_.emplace<commands::set_command<>>();
        else
          selected_.emplace<commands::help_command>();
      }
      else 
        selected_.emplace<commands::help_command>();
    }

    int execute()
    {      
      auto cmdArgs = utils::make_cspan(tokens_, 1);
     
      return std::visit([=] (auto& cmd) -> int { return cmd.execute(cmdArgs); }, selected_);
    }

  private:
    bool exists(const std::string &opt) const {
      return std::find(this->tokens_.begin(), this->tokens_.end(), opt)
        != this->tokens_.end();
    }

    const std::string& get(const std::string &opt) const {
      std::vector<std::string>::const_iterator itr;
      itr = std::find(this->tokens_.begin(), this->tokens_.end(), opt);
      if (itr != this->tokens_.end() && ++itr != this->tokens_.end())
        return *itr;

      static const std::string empty_string("");
      return empty_string;
    }
  };
}
