#pragma once

#include <variant>

namespace pronto
{
  struct no_t
  { 
    no_t()
    {
    }

    no_t(std::initializer_list<no_t> empty_list)
    {

    }
  };

  template<typename value_t, typename error_t>
  struct result
  {
    std::variant<value_t, error_t> var_;

    bool ok()
    {
      return var_.index() == 0;
    }

    static result<value_t, error_t> ok(value_t&& val)
    {
      auto res = result<value_t, error_t>();
      res.var_.emplace(val);
      return res;
     // return res
    }

    static result<value_t, error_t> fail(error_t err)
    {
      auto res = result<value_t, error_t>();
      res.var_.emplace<error_t>(err);
      return res;
      // return res
    }

    result(readonly result<value_t, error_t>& copy) = default;

  private:
    result()
    {
    }
  };

  template<typename error_t>
  using result_fail = result<no_t, error_t>;

  template<typename value_t>
  using result_ok = result<value_t, no_t>;
}