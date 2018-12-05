#pragma once
#include <windows.h>
#include <string>

namespace pronto::toolchains
{
  class win32reg
  {
    int run()
    {
      HKEY h_key;

      //Komputer\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\VC
      //Komputer\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\VC\19.0

      LONG l_res = ::RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\VisualStudio\\14.0\\Setup\\VC", 0, KEY_READ, &h_key);      

      if (l_res == ERROR_FILE_NOT_FOUND)
      {
        // no key
      }
      else if (l_res == ERROR_SUCCESS)
      {
        std::wstring val;
        std::wstring default_val = L"";

        auto resppp = get_string_reg_key(h_key, L"ProductDir", val, default_val);


        // found
      }

      /*HKEY hKey;

  bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
  bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);
  std::wstring strValueOfBinDir;
  std::wstring strKeyDefaultValue;
  GetStringRegKey(hKey, L"BinDir", strValueOfBinDir, L"bad");
  GetStringRegKey(hKey, L"", strKeyDefaultValue, L"bad");*/

  //Komputer\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\Setup\VC

      //return proc_.run("cargo");
    }

    LONG get_string_reg_key(HKEY hKey, const std::wstring& strValueName, std::wstring& strValue, const std::wstring& strDefaultValue)
    {
      strValue = strDefaultValue;
      WCHAR szBuffer[512];
      DWORD dwBufferSize = sizeof(szBuffer);
      ULONG nError;
      nError = ::RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
      if (ERROR_SUCCESS == nError)
      {
        strValue = szBuffer;
      }
      return nError;
    }

    LONG get_dword_reg_key(HKEY hKey, const std::wstring& strValueName, DWORD& nValue, DWORD nDefaultValue)
    {
      nValue = nDefaultValue;
      DWORD dwBufferSize(sizeof(DWORD));
      DWORD nResult(0);
      LONG nError = ::RegQueryValueExW(hKey,
        strValueName.c_str(),
        0,
        NULL,
        reinterpret_cast<LPBYTE>(&nResult),
        &dwBufferSize);

      if (ERROR_SUCCESS == nError)
      {
        nValue = nResult;
      }
      return nError;
    }

    LONG get_bool_reg_key(HKEY hKey, const std::wstring& strValueName, bool& b_value, bool bDefaultValue)
    {
      DWORD nDefValue((bDefaultValue) ? 1 : 0);
      DWORD nResult(nDefValue);
      LONG nError = get_dword_reg_key(hKey, strValueName.c_str(), nResult, nDefValue);
      if (ERROR_SUCCESS == nError)
      {
        b_value = (nResult != 0) ? true : false;
      }
      return nError;
    }
  };
}