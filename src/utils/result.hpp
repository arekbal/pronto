#pragma once

#include <variant>

namespace pronto
{
  class result_no_t
  {
    bool ok_;
  public:

    bool is_ok() const { return ok_; }

    result_no_t(bool ok_) : ok_(ok_)
    {
    }
  };

  template<typename value_t>
  class result_val
  {
    value_t val_;
    bool ok_;

  public:
    const value_t& value() const { return val_; }

    bool is_ok() const { return ok_; }

    result_val(value_t val_) : val_(val_), ok_(true)
    {
    }

    result_val(result_no_t r) : val_(val_), ok_(r.is_ok())
    {
    }
  };

  template<typename error_t>
  class result_err
  {
    error_t err_;
    bool ok_;
  public:
    const error_t& error() const { return err_; }

    bool is_ok() const { return ok_; }

    result_err(error_t err_) : err_(err_), ok_(false)
    {
    }

    result_err(result_no_t r) : err_(error_t()), ok_(r.is_ok())
    {
    }
  }; 

  template<typename value_t, typename error_t>
  struct result
  {
    std::variant<value_t, error_t> var_;   

    result(result_val<value_t> r) : var_(r.is_ok() ? r.value() : error_t())
    {
    }

    result(result_err<error_t> r) : var_(r.is_ok() ? value_t() : r.error())
    {
    }

    result(result_no_t r) : var_(r.is_ok() ? value_t() : error_t())
    {
    }

    bool ok() const { return var_.index() == 0; }

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

    result(const result<value_t, error_t>& copy) = default;

  private:
    result()
    {
    }
  };

  template<typename error_t>
  result_err<error_t> fail(error_t err)
  {
    return result_err<error_t>(err);
  }

  template<typename value_t>
  result_val<value_t> ok(value_t val)
  {
    return result_val<value_t>(val);
  }
 
  result_no_t ok()
  {    
    return result_no_t(true);
  }

  result_no_t fail()
  {   
    return result_no_t(false);
  }
}