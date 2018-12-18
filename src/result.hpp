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

    result_no_t(std::initializer_list<result_no_t> r) : ok_(true)
    {
      (r);
    }
  };

  template<typename value_t>
  class result_val
  {
    value_t val_;
    bool ok_;

  public:
    value_t& value() const { return val_; }

    //value_t&& rvalue() const { return std::move(val_); }

    bool is_ok() const { return ok_; }

   /* result_val() : val_(value_t()), ok_(true)
    {
    }*/

    result_val(value_t val_) : val_(val_), ok_(true)
    {
    }


  /*  result_val(const value_t& val_) : val_(val_), ok_(true)
    {
    }

    result_val(value_t&& val_) : val_(val_), ok_(true)
    {
    }*/

    /*static result_val<value_t>&& move(value_t&& v)
    {
      return std::move(result_val<value_t>(v));
    }*/

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
    error_t&& rerror() const { return err_; }

    bool is_ok() const { return ok_; }

    result_err() : err_(error_t()), ok_(false)
    {
    }

    result_err(const error_t& err_) : err_(err_), ok_(false)
    {
    }

    result_err(error_t&& err_) : err_(err_), ok_(false)
    {
    }

    result_err(const result_no_t& r) : err_(error_t()), ok_(r.is_ok())
    {
    }

    result_err(std::initializer_list<result_no_t> r) : ok_(true)
    {
    }
  }; 

  template<typename value_t, typename error_t>
  struct result
  {  
    std::variant<value_t, error_t> var_;
  public:
    result(value_t val) : var_(val)
    {
    }

    /*result(const value_t& val) : var_(val)
    {
    }*/

  /*  result(value_t&& val) : var_(val)
    {
    }*/

    result(const error_t& err) : var_(err)
    {
    }

    result(result_val<value_t> r) : var_(r.is_ok() ? r.value() : error_t())
    {
    }

    result(const result_val<value_t>& r) : var_(r.is_ok() ? r.value() : error_t())
    {
    }
   
  /*  result(result_val<value_t>&& r) : var_(r.is_ok() ? r.rvalue() : error_t())
    {
    }*/

    result(const result_err<error_t>& r) : var_(r.is_ok() ? value_t() : r.error())
    {
    }

  /*  result(result_err<error_t>&& r) : var_(r.is_ok() ? value_t() : r.error())
    {
    }*/

    result(const result_no_t& r) : var_(r.is_ok() ? value_t() : error_t())
    {
    }

    value_t& value() { return std::get<value_t>(var_); }
    //const value_t& ref() const { return std::get<value_t>(var_); }
    value_t&& rvalue() const { return std::move(var_); }

    const error_t& error() const { return std::get<error_t>(var_); }
    error_t&& rerror() const { return std::move(var_); }

    bool is_ok() const { return var_.index() == 0; }

    struct factory
    {
      static result<value_t, error_t> ok(value_t&& val)
      {
        auto res = result<value_t, error_t>();
        res.var_.emplace(val);
        return res;
        // return res
      }

      static result<value_t, error_t> fail(error_t&& err)
      {
        auto res = result<value_t, error_t>();
        res.var_.emplace(err);
        return res;
        // return res
      }
    };

    result(const result<value_t, error_t>& copy) = default;

  private:
    result()
    {
    }
  };

  template<typename error_t>
  result_err<error_t> fail(const error_t& err)
  {
    return result_err<error_t>(err);
  }

  template<typename error_t>
  result_err<error_t> fail(error_t&& err)
  {
    return result_err<error_t>(err);
  }

  template<typename value_t>
  result_val<value_t> ok(value_t val)
  {
    return result_val<value_t>(val);
  }

 /* template<typename value_t>
  result_val<value_t> ok(const value_t& val)
  {
    return result_val<value_t>(val);
  }*/

 /* template<typename value_t>
  result_val<value_t>&& ok(value_t&& val)
  {
    return result_val<value_t>(val);
  }*/

  template<typename value_t>
  result_val<value_t>&& ok_move(value_t&& val)
  {
    return result_val<value_t>::move(std::move(val));
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