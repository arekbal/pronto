#pragma once

#include <vector>
#include <string>
#include <variant>

#include "str.hh"
#include "span.hh"
#include "semver.hh"

namespace pronto::utils
{
  struct semver_req
  {
    struct simple_range
    {
      enum class op
      {
        none,
        gt,
        lt,
        eq,
        gte,
        lte,
        tilde,
        caret
      };      

      unsigned int major;
      unsigned int minor;
      unsigned int patch;
      unsigned char components;

      op op;

      simple_range(std::string s)
      {
        str::trim(s);

        if (str::starts_with(s, ">="))
        {
          op = op::gte;
          s = s.substr(2);
        }
        else if (str::starts_with(s, ">"))
        {
          op = op::gt;
          s = s.substr(1);
        }
        else if (str::starts_with(s, "<="))
        {
          op = op::lte;
          s = s.substr(2);
        }
        else if (str::starts_with(s, "<"))
        {
          op = op::lt;
          s = s.substr(1);
        }
        else if (str::starts_with(s, "="))
        {
          op = op::lt;
          s = s.substr(1);
        }
        else if (str::starts_with(s, "^"))
        {
          op = op::caret;
          s = s.substr(1);         
        }
        else if (str::starts_with(s, "~"))
        {
          op = op::tilde;
          s = s.substr(1);
        }
        else
          op = op::none;

        str::trim(s);
        parse_semver(s);
        return;

        parse_semver(s);
      }

      std::string to_string()
      {
        std::string s;
        switch (op)
        {
        case op::caret:
          s = "^";
          break;
        case op::tilde:
          s = "~";
          break;
        case op::eq:
          s = "=";
          break;
        case op::gt:
          s = ">";
          break;
        case op::gte:
          s = ">=";
          break;
        case op::lt:
          s = "<";
          break;
        case op::lte:
          s = "<=";
          break;            
        default:
          s = "";
          break;
        }

        s += major;
        if (components > 1)
        {
          s += ".";
          if (minor == -1)
            s += "X";
          else
            s += minor;

          if (components > 2)
          {
            s += ".";
            if (patch == -1)
              s += "X";
            else
              s += patch;
          }
        }
      }

      bool satisfies(const semver& ver)
      {
        return false;
      }

      private:
        void parse_semver(const std::string& s)
        {
          auto major_minor_patch = str::split(s, '.');

          major = std::stoul(major_minor_patch[0]);

          components = major_minor_patch.size();

          if (components > 1)
          {
            if (major_minor_patch[1].compare("x") == 0 || major_minor_patch[1].compare("X") == 0)
              minor = -1;
            else
              minor = std::stoul(major_minor_patch[1]);
          }

          if (components > 2)
          {
            if (major_minor_patch[2].compare("x") == 0 || major_minor_patch[2].compare("X") == 0)
              patch = -1;
            else
              patch = std::stoul(major_minor_patch[2]);
          }
        }
    };

    struct hyphen_range
    {
      unsigned int a_major;
      unsigned int a_minor;
      unsigned int a_patch;
      unsigned char a_components;

      unsigned int b_major;
      unsigned int b_minor;
      unsigned int b_patch;
      unsigned char b_components;

      hyphen_range(const std::string& s)
      {
        auto range_parts = str::split(s, '-');

        str::trim(range_parts[0]);
        str::trim(range_parts[1]);

        auto major_minor_patch = str::split(range_parts[0], '.');
        a_major = std::stoul(major_minor_patch[0]);

        a_components = major_minor_patch.size();
        if (a_components > 1)
        {
          if (major_minor_patch[1].compare("x") == 0 || major_minor_patch[1].compare("X") == 0)
            a_minor = -1;
          else
            a_minor = std::stoul(major_minor_patch[1]);
        }
        
        if (a_components > 2)
        {
          if (major_minor_patch[2].compare("x") == 0 || major_minor_patch[2].compare("X") == 0)
            a_patch = -1;
          else
            a_patch = std::stoul(major_minor_patch[2]);
        }

        major_minor_patch = str::split(range_parts[1], '.');
        b_major = std::stoul(major_minor_patch[0]);

        b_components = major_minor_patch.size();

        if (b_components > 1)
        {
          if (major_minor_patch[1].compare("x") == 0 || major_minor_patch[1].compare("X") == 0)
            b_minor = -1;
          else
            b_minor = std::stoul(major_minor_patch[1]);
        }

        if (b_components > 2)
        {
          if (major_minor_patch[2].compare("x") == 0 || major_minor_patch[2].compare("X") == 0)
            b_patch = -1;
          else
            b_patch = std::stoul(major_minor_patch[2]);
        }
      }

      std::string to_string()
      {
        std::string s = std::to_string(a_major);
        
        if (a_components > 1)
        {
          s += ".";
          if (a_minor == -1)
            s += "X";
          else
            s += a_minor;

          if (a_components > 2)
          {
            s += ".";
            if (a_patch == -1)
              s += "X";
            else
              s += a_patch;
          }
        }

        s += " - ";

        s += b_major;

        if (b_components > 1)
        {
          s += ".";
          if (b_minor == -1)
            s += "X";
          else
            s += b_minor;

          if (b_components > 2)
          {
            s += ".";
            if (b_patch == -1)
              s += "X";
            else
              s += b_patch;
          }
        }
      }

      bool satisfies(const semver& ver)
      {
        return false;
      }
    };

    std::vector<std::variant<simple_range, hyphen_range>> rules_;

  public:
    semver_req()
    {
    }

    semver_req(const std::string& s)
    {     
      auto rules = str::split(s, '|');

      for(auto& rule : rules)
      {        
        if (rule.empty())
          continue;

        str::trim(rule);

        auto hyphen_index = rule.find('-');
        
        if (hyphen_index > -1)
          rules_.push_back(hyphen_range(rule));
        else
          rules_.push_back(simple_range(rule));
      }
    }

    bool satisfies(const semver& semver_)
    {
      bool result = false;
      for (auto& rule : rules_)
      {
        result |= std::visit([&](auto& r) -> int { return r.satisfies(semver_); }, rule);
      }

      return result;
    }
  };
}