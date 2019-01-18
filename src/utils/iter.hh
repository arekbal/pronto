#pragma once

#include <vector>
#include <string>
#include "str.hh"
#include "span.hh"

namespace pronto::utils::iter
{
  template<typename iter_t>
  bool empty(iter_t& iter_)
  {
    return iter_.begin() == iter_.end();
  }
}