#pragma once

#include <windows.h>
#include <shlwapi.h>
#include <string>

#include "utils/str.hpp"

namespace pronto
{
  template<typename console_t=console>
  struct process
  {
    console_t console_;

    int run(const std::string& cmd, bool expand_env_vars=true)
    {
#ifdef _WIN32
     /* BOOL PathUnExpandEnvStringsA(
        LPCSTR pszPath,
        LPSTR  pszBuf,
        UINT   cchBuf
      );*/

      auto wcmd = str::utf8_to_utf16(cmd);
      std::wstring wstr(5000, 0);

      std::string str_result = "";
      HANDLE h_out_pipe_read, h_out_pipe_write;
      HANDLE h_err_pipe_read, h_err_pipe_write;

      SECURITY_ATTRIBUTES sa_attr = { sizeof(SECURITY_ATTRIBUTES) };
      sa_attr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
      sa_attr.lpSecurityDescriptor = NULL;

      // Create a pipe to get results from child's stdout.

      auto create_pipe_result = CreatePipe(&h_out_pipe_read, &h_out_pipe_write, &sa_attr, 0);

      if (!create_pipe_result)
        return create_pipe_result;

      create_pipe_result = CreatePipe(&h_err_pipe_read, &h_err_pipe_write, &sa_attr, 0);

      if (!create_pipe_result)
        return create_pipe_result;

      STARTUPINFO si = { sizeof(STARTUPINFO) };
      si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;      
      si.hStdOutput = h_out_pipe_write;
      si.hStdError = h_err_pipe_write;
      si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.
      
      PROCESS_INFORMATION pi = { 0 };

      //::ExpandEnvironmentStrings()

      BOOL create_proc_result = ::CreateProcessW(NULL, (LPWSTR)wcmd.c_str(), NULL, NULL, TRUE, 
        // NULL
         CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT
        , NULL, NULL, &si, &pi);
      if (!create_proc_result)
      {
        ::CloseHandle(h_out_pipe_write);
        ::CloseHandle(h_out_pipe_read);
        ::CloseHandle(h_err_pipe_write);
        ::CloseHandle(h_err_pipe_read);
        std::string s = "'" + cmd + "' not recognized cmd";
        console_.err(s.c_str());
        return create_proc_result;
      }

      bool process_ended = false;

      BOOL read_file_result = FALSE;
      for (; !process_ended;)
      {
        // Give some timeslice (20/50ms), so we won't waste 100% cpu.
        process_ended = ::WaitForSingleObject(pi.hProcess, 20) == WAIT_OBJECT_0;

        // Even if process exited - we continue reading, if there is some data available over pipe.
        for (;;)
        {
          char buf[1024];
          DWORD dw_read = 0;
          DWORD dw_avail = 0;

          if (!::PeekNamedPipe(h_out_pipe_read, NULL, 0, NULL, &dw_avail, NULL))
            break;

          if (!dw_avail) // no data available, return
            break;

          read_file_result = ::ReadFile(h_out_pipe_read, buf, min(sizeof(buf) - 1, dw_avail), &dw_read, NULL);

          if (!read_file_result || !dw_read)
            // error, the child process might ended
            break;

          buf[dw_read] = 0;
          str_result += buf;
        }

        if(!str_result.empty())
          console_.inf(str_result.c_str());

        //// Even if process exited - we continue reading, if there is some data available over pipe.
        //for (;;)
        //{
        //  char buf[1024];
        //  DWORD dw_read = 0;
        //  DWORD dw_avail = 0;

        //  if (!::PeekNamedPipe(h_out_pipe_read, NULL, 0, NULL, &dw_avail, NULL))
        //    break;

        //  if (!dw_avail) // no data available, return
        //    break;

        //  read_file_result = ::ReadFile(h_out_pipe_read, buf, min(sizeof(buf) - 1, dw_avail), &dw_read, NULL);

        //  if (!read_file_result || !dw_read)
        //    // error, the child process might ended
        //    break;

        //  buf[dw_read] = 0;
        //  str_result += buf;
        //}

        //if (!str_result.empty())
        //  console_.inf(str_result.c_str());

        str_result.clear();

        // Even if process exited - we continue reading, if there is some data available over pipe.
        for (;;)
        {
          char buf[1024];
          DWORD dw_read = 0;
          DWORD dw_avail = 0;

          if (!::PeekNamedPipe(h_err_pipe_read, NULL, 0, NULL, &dw_avail, NULL))
            break;

          if (!dw_avail) // no data available, return
            break;

          read_file_result = ::ReadFile(h_err_pipe_read, buf, min(sizeof(buf) - 1, dw_avail), &dw_read, NULL);

          if (!read_file_result || !dw_read)
            // error, the child process might ended
            break;

          buf[dw_read] = 0;
          str_result += buf;
        }

        if (!str_result.empty())
          console_.err(str_result.c_str());
      } //for

      ::CloseHandle(h_out_pipe_write);
      ::CloseHandle(h_out_pipe_read);
      ::CloseHandle(h_err_pipe_write);
      ::CloseHandle(h_err_pipe_read);
      ::CloseHandle(pi.hProcess);
      ::CloseHandle(pi.hThread);

      return read_file_result;
#else
      return system(s.c_str());
#endif // _WIN

    }
  };
}
