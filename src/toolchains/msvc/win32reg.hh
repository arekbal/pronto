#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <optional>

#include "../../utils/str.hh"

#include "../../result.hh"

namespace pronto::toolchains::msvc::win32reg
{
  enum class reg_key_access
  {
    // Combines the STANDARD_RIGHTS_REQUIRED, KEY_QUERY_VALUE, KEY_SET_VALUE, KEY_CREATE_SUB_KEY, KEY_ENUMERATE_SUB_KEYS, KEY_NOTIFY, and KEY_CREATE_LINK access rights.
    all_access = KEY_ALL_ACCESS,

    // Reserved for system use.
    create_link = KEY_CREATE_LINK,

    // Required to create a subkey of a registry key.
    create_sub_key = KEY_CREATE_SUB_KEY,

    // Required to enumerate the subkeys of a registry key.
    enumerate_sub_keys = KEY_ENUMERATE_SUB_KEYS,

    // Equivalent to KEY_READ.
    execute = KEY_EXECUTE,

    // Required to request change notifications for a registry key or for subkeys of a registry key.
    notify = KEY_NOTIFY,

    // Required to query the values of a registry key.
    query_value = KEY_QUERY_VALUE,

    // Combines the STANDARD_RIGHTS_READ, KEY_QUERY_VALUE, KEY_ENUMERATE_SUB_KEYS, and KEY_NOTIFY values.
    read = KEY_READ,

    // Required to create, delete, or set a registry value.
    set_value = KEY_SET_VALUE,

    // Indicates that an application on 64-bit Windows should operate on the 32-bit registry view.
    // This flag is ignored by 32-bit Windows. For more information, see Accessing an Alternate Registry View.
    // This flag must be combined using the OR operator with the other flags in this table that either query or access registry values.
    // Windows 2000: This flag is not supported.
    wow64_32key = KEY_WOW64_32KEY,

    // Indicates that an application on 64-bit Windows should operate on the 64-bit registry view.
    //  This flag is ignored by 32-bit Windows. For more information, see Accessing an Alternate Registry View.
    //  This flag must be combined using the OR operator with the other flags in this table that either query or access registry values.
    //  Windows 2000: This flag is not supported.
    wow64_64key = KEY_WOW64_64KEY,

    // Combines the STANDARD_RIGHTS_WRITE, KEY_SET_VALUE, and KEY_CREATE_SUB_KEY access rights.
    write = KEY_WRITE,
  };

  class reg_key
  {
    HKEY h_key_;  

    reg_key(HKEY h_key_) 
      :
      h_key_(h_key_)
    { }

  public:
    reg_key(reg_key& reg_key_) // move on copy
      :
      h_key_(reg_key_.h_key_)
    {
      reg_key_.h_key_ = nullptr;
    }

    reg_key(reg_key&& reg_key_)
      :
      h_key_(reg_key_.h_key_)
    {
      reg_key_.h_key_ = nullptr;
    }

    // Writes all the attributes of the specified open registry key into the registry.
    result_err<LSTATUS> flush()
    {
      auto status = ::RegFlushKey(h_key_);

      if (ERROR_SUCCESS != status)
      {
        return fail(std::move(status));
      }      

      return {};
    }

    ~reg_key()
    {
      if(
        h_key_ != nullptr and
        h_key_ != HKEY_LOCAL_MACHINE and
        h_key_ != HKEY_CLASSES_ROOT and
        h_key_ != HKEY_CURRENT_USER and
        h_key_ != HKEY_CURRENT_CONFIG and
        h_key_ != HKEY_PERFORMANCE_DATA and
        h_key_ != HKEY_USERS)
      {
        auto status = ::RegCloseKey(h_key_);
        if (ERROR_SUCCESS != status)
        {
          // TODO: log error
        }
      }
    }

    struct factory
    {
      static reg_key local_machine()
      {
        return reg_key(HKEY_LOCAL_MACHINE);
      }

      static reg_key classes_root()
      {
        return reg_key(HKEY_CLASSES_ROOT);
      }

      static reg_key curr_user()
      {
        return reg_key(HKEY_CURRENT_USER);
      }

      static reg_key curr_config()
      {
        return reg_key(HKEY_CURRENT_CONFIG);
      }

      static reg_key perf_data()
      {
        return reg_key(HKEY_PERFORMANCE_DATA);
      }

      static reg_key users()
      {
        return reg_key(HKEY_USERS);
      }

      static result<reg_key, LSTATUS> impersonated_curr_user(reg_key_access access = reg_key_access::read)
      {
        HKEY h_key;
        auto status = ::RegOpenCurrentUser((REGSAM)access, &h_key);
        if (ERROR_SUCCESS == status)
          return reg_key(h_key);

        return status;
      }
    };
  
    result<reg_key, LSTATUS> get_subkey(const std::string& s, reg_key_access access=reg_key_access::read)
    {
      std::wstring ws = str::utf8_to_utf16(s);

      HKEY h_key;
      auto status = ::RegOpenKeyExW(h_key_, ws.c_str(), 0, (REGSAM)access, &h_key);
      if (ERROR_SUCCESS == status)
        return reg_key(h_key);

      return status;
    }

    // incorrect... returns empty entries
    std::vector<std::string> list_subkeys() const
    {
      std::vector<WCHAR> buff(512);

      auto subkeys = std::vector<std::string>();

      LSTATUS enumerate_keys_result;
      DWORD index = 0;
      while (true)
      {
        enumerate_keys_result = ::RegEnumKeyW(h_key_, index, buff.data(), (DWORD)buff.capacity());

        index++;

        if (ERROR_SUCCESS == enumerate_keys_result)
        {
          auto ws = std::wstring(buff.begin(), buff.end());
          auto s = str::utf16_to_utf8(ws);
          subkeys.push_back(s);
          continue;
        }

        if (ERROR_MORE_DATA == enumerate_keys_result)
        {
          buff.clear();
          buff.reserve(buff.capacity() * 2);
          continue;
        }

        break; // ignore errors...

        // if (enumerate_keys_result == ERROR_NO_MORE_ITEMS)
        // {
        //   break;
        // }
      }

      return subkeys;
    }

    result<std::string, LSTATUS> get_string(const std::string& val_name)
    {
      std::wstring ws_val_name = str::utf8_to_utf16(val_name);

      std::vector<BYTE> buff(512, 0);

      DWORD buff_size = 512;

      LSTATUS status;

      while (true)
      {
        status = ::RegQueryValueExW(h_key_, ws_val_name.c_str(), 0, NULL, buff.data(), &buff_size);
        
        if (ERROR_MORE_DATA == status)
        {
          buff.clear();
          buff.reserve(buff_size);
          continue;
        }

        if (ERROR_SUCCESS == status)
        {
          buff.resize(buff_size);
          std::wstring result_ws(reinterpret_cast<std::wstring::traits_type::char_type*>(buff.data()), buff.size() / sizeof(std::wstring::traits_type::char_type));

          return str::utf16_to_utf8(result_ws);
        }        

        return status;
      }
    }

    result<bool, LSTATUS> get_bool(const std::string& val_name)
    {
      auto res = get_dword(val_name);
      if (res.is_ok())
        return res.value() != 0 ? true : false;

      return res.error();
    }
    
    result<DWORD, LSTATUS> get_dword(const std::string& val_name)
    {
      std::wstring ws_val_name = str::utf8_to_utf16(val_name);

      DWORD buff_size(sizeof(DWORD));

      DWORD res(0);
      auto status = ::RegQueryValueExW(h_key_,
        ws_val_name.c_str(),
        0,
        NULL,
        reinterpret_cast<LPBYTE>(&res),
        &buff_size);

      if (ERROR_SUCCESS == status)
        return res;

      return status;
    }
  };
}