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
    static constexpr unsigned int const STAR = (unsigned int)-1;
    static constexpr unsigned int const SM_X = (unsigned int)-2;
    static constexpr unsigned int const BG_X = (unsigned int)-3;

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
        caret,
        single_star,
        single_sm_x,
        single_bg_x
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
          op = op::eq;
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
        else if (s.compare("*") == 0)
        {
          op = op::single_star;
          return;
        }
        else if (s.compare("x") == 0)
        {
          op = op::single_sm_x;
          return;
        }
        else if (s.compare("X") == 0)
        {
          op = op::single_bg_x;
          return;
        }
        else
          op = op::none;

        str::trim(s);
        parse_semver(s);
      }

      std::string to_string()
      {   
        std::string s;
        switch (op)
        {
        case op::none:
          return "";
        case op::single_star:
          return "*";
        case op::single_sm_x:
          return "x";
        case op::single_bg_x:
          return "X";
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
            s += "*";
          else
            s += minor;

          if (components > 2)
          {
            s += ".";
            if (patch == STAR)
              s += "*";
            else if (patch == SM_X)
              s += "x";
            else if (patch == BG_X)
              s += "X";
            else
              s += patch;
          }
        }
      }

      bool is_satisfied_by(const semver& ver) const
      {
        switch (op)
        {       
        case op::single_star:
        case op::single_sm_x:
        case op::single_bg_x:
          return true;
        case op::none:       
        {
          if (components == 3)
          {
            if(patch == STAR || patch == BG_X || patch == SM_X)
              return std::tie(major, minor) == std::tie(ver.major, ver.minor);
            else
              return std::tie(major, minor, patch) == std::tie(ver.major, ver.minor, ver.patch);
          }
          else
            if (components == 2)
            {
              if (minor == STAR || minor == BG_X || minor == SM_X)
                return std::tie(major) == std::tie(ver.major);
              else
                return std::tie(major, minor) == std::tie(ver.major, ver.minor);
            }
            else
              if (components == 1)
                return major == ver.major;              
        }
        case op::eq:
          return std::tie(ver.major, ver.minor, ver.patch) == std::tie(major, minor, patch);
        case op::gt:
          return std::tie(ver.major, ver.minor, ver.patch) > std::tie(major, minor, patch);
        case op::gte:
          return std::tie(ver.major, ver.minor, ver.patch) >= std::tie(major, minor, patch);
        case op::lt:
          return std::tie(ver.major, ver.minor, ver.patch) < std::tie(major, minor, patch);
        case op::lte:
          return std::tie(ver.major, ver.minor, ver.patch) <= std::tie(major, minor, patch);

        case op::tilde:
          if (components == 3)
          {
            auto v_minor = minor + 1u;
            auto v_patch = 0u;
            return std::tie(major, minor, patch) <= std::tie(ver.major, ver.minor, ver.patch) &&
              std::tie(ver.major, ver.minor, ver.patch) < std::tie(major, v_minor, v_patch);
          }
          else if (components == 2)
            return std::tie(major, minor) == std::tie(ver.major, ver.minor);
          else if (components == 1)
            return major == ver.major;

        case op::caret:
        {
          auto _major = major;
          auto _minor = minor == STAR || minor == SM_X || minor == BG_X ? 0 : minor;
          auto _patch = patch == STAR || patch == SM_X || patch == BG_X ? 0 : patch;

          if (components < 3)
            _patch = 0;

          if (components < 2)
            _minor = 0;

          auto result = std::tie(_major, _minor, _patch) <= std::tie(ver.major, ver.minor, ver.patch);
         

          int index = 0;

          if (major == 0)
          {
            index++;

            if (minor == 0 || minor == STAR || minor == SM_X || minor == BG_X)
              index++;
          }

          if (index == 0)
            return result && ver.major < (major + 1);

          if (index == 1)
            return result && ver.minor < (minor + 1);

          if (index == 2)
            return result && ver.patch < (patch + 1);

          return false;
        }
        default:
          throw std::runtime_error("unknown op");
        }
      }

      private:
        void parse_semver(const std::string& s)
        {
          auto major_minor_patch = str::split(s, '.');

          major = std::stoul(major_minor_patch[0]);

          components = major_minor_patch.size();

          if (components > 1)
          {
            if (major_minor_patch[1].compare("x") == 0 || major_minor_patch[1].compare("X") == 0 || major_minor_patch[1].compare("*") == 0)
              minor = STAR;
            else
              minor = std::stoul(major_minor_patch[1]);
          }

          if (components > 2)
          {
            if (major_minor_patch[2].compare("x") == 0 || major_minor_patch[2].compare("X") == 0 || major_minor_patch[2].compare("*") == 0)
              patch = STAR;
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
          a_minor = std::stoul(major_minor_patch[1]);

          if (a_components > 2)
            a_patch = std::stoul(major_minor_patch[2]);
          else
            a_patch = STAR;
        }
        else
          a_minor = STAR;

        major_minor_patch = str::split(range_parts[1], '.');
        b_major = std::stoul(major_minor_patch[0]);

        b_components = major_minor_patch.size();
        if (b_components > 1)
        {
          b_minor = std::stoul(major_minor_patch[1]);

          if (b_components > 2)
            b_patch = std::stoul(major_minor_patch[2]);
          else
            b_patch = STAR;
        }
        else
          b_minor = STAR;
      }

      std::string to_string() const
      {
        std::string s = std::to_string(a_major);
        
        if (a_components > 1)
        {
          s += ".";
          if (a_minor == STAR)         
            s += a_minor;

          if (a_components > 2)
          {
            s += ".";        
            s += a_patch;
          }
        }

        s += " - ";

        s += b_major;

        if (b_components > 1)
        {
          s += ".";
          s += b_minor;

          if (b_components > 2)
          {
            s += ".";
            s += b_patch;
          }
        }
      }

      bool is_satisfied_by(const semver& ver) const
      {
          auto a_minor_ = (a_minor == STAR) ? 0u : a_minor;
          auto a_patch_ = (a_patch == STAR) ? 0u : a_patch;

          auto b_major_ = b_major;
          auto b_minor_ = b_minor;
          auto b_patch_ = b_patch;

          if (b_components == 2)
            b_minor_++;
          else
            if (b_components == 1)
              b_major_++;

          auto ver_tie = std::tie(ver.major, ver.minor, ver.patch);

          if(b_components > 2)
            return
              std::tie(a_major, a_minor_, a_patch_) <= ver_tie &&
              ver_tie <= std::tie(b_major_, b_minor_, b_patch_);
          else
            return
              std::tie(a_major, a_minor_, a_patch_) >= ver_tie &&
              ver_tie < std::tie(b_major_, b_minor_, b_patch_);
      }
    };

    std::vector<std::variant<simple_range, hyphen_range>> rules_;

  public:
    semver_req()
    {
    }

    semver_req(const std::string& s)
    {     
      auto rulesx = str::split(s, '|');

      for(auto& rulex : rulesx)
      {  
        auto rules = str::split(rulex, ',');
        for (auto& rule_s : rules)
        {
          if (rule_s.empty())
            continue;

          str::trim(rule_s);

          auto hyphen_index = rule_s.find('-');

          if (hyphen_index == std::string::npos)
            rules_.push_back(simple_range(rule_s));
          else
            rules_.push_back(hyphen_range(rule_s));
        }
      }
    }

    bool is_satisfied_by(const semver& semver_) const
    {
      if (rules_.size() == 0)
        return true;
      
      for (auto& rule : rules_)
      {
        if (std::visit([&](auto& r) -> int { return r.is_satisfied_by(semver_); }, rule))
          return true;
      }

      return false;
    }
  };
}