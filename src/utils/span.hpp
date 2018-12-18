#pragma once

#include "../core.hpp"

namespace pronto::utils
{  
  template <class iter_t>
  class span {
  public:  
    iter_t begin() const { return begin_; }
    iter_t end() const { return end_; }

    span(iter_t begin, iter_t end) : begin_(begin), end_(end) {}

    auto slice(u32 start) const -> span<iter_t>
    {
      auto begin = std::copy(begin_);
      auto end = std::copy(end_);
      auto size = end - begin;
      start = int(size - start) > -1 ? start : size;
      begin += start;
      return span<iter_t>(begin, end);
    }

    auto size() const
    {
      return end_ - begin_;
    }

    bool empty() const
    {
      return size() == 0;
    }

    auto slice(u32 start, u32 length) const -> span<iter_t>
    {
      auto begin = begin_ + 0;
      auto end = end_ + 0;
      auto size = end - begin;

      start = int(size - start) > -1 ? start : size;
      length = int(size - start) < length ? (size - start) : length;

      begin += start;
      end -= (size - start - length);
      return span<iter_t>(begin, end);
    }

  private:
    iter_t begin_;
    iter_t end_;
  };

  template <class iter_t>
  static auto make_span(iter_t begin, iter_t end)
  {
    return span<iter_t>(begin, end);
  }

  template <class iter_t>
  static auto make_cspan(const iter_t begin, const iter_t end)
  {
    return span<iter_t>(begin, end);
  }

  template <class item_t>
  static auto make_span(const std::vector<item_t>& items)
  {
    return span<std::vector<item_t>::iterator>(items.begin(), items.end());
  }

  template <class item_t>
  static auto make_cspan(std::vector<item_t>& items)
  {
    return span<std::vector<item_t>::const_iterator>(items.cbegin(), items.cend());
  }

  template <class item_t>
  static auto make_span(std::vector<item_t>& items, u32 start)
  {
    start = int(items.size() - start) > -1 ? start : items.size();
    auto begin = items.begin();
    begin += start;
    return span<std::vector<item_t>::iterator>(begin, items.end());
  } 

  template <class item_t>
  static auto make_cspan(std::vector<item_t>& items, u32 start)
  {
    start = i32(items.size() - start) > -1 ? start : items.size();
    auto begin = items.cbegin();
    begin += start;
    return span<std::vector<item_t>::const_iterator>(begin, items.cend());
  }

  template <class item_t>
  static auto make_span(std::vector<item_t>& items, u32 start, u32 length)
  {
    start = int(items.size() - start) > -1 ? start : items.size();
    length = int(items.size() - start) < length ? (items.size() - start) : length;
    auto begin = items.begin();
    begin += start;
    auto end = items.end();
    end -= (items.size() - start - length);
    return span<std::vector<item_t>::iterator>(begin, end);
  }

  template <class item_t>
  static auto make_cspan(std::vector<item_t>& items, u32 start, u32 length)
  {
    start = int(items.size() - start) > -1 ? start : items.size();
    length = int(items.size() - start) < length ? (items.size() - start) : length;
    auto begin = items.cbegin();
    begin += start;
    auto end = items.cend();
    end -= (items.size() - start - length);
    return span<std::vector<item_t>::const_iterator>(begin, end);
  }
  
  using cspan_vec_s = span<std::vector<std::string>::const_iterator>;
}