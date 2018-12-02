#pragma once

#include <windows.h>
#include <string>

#include "str.hpp"

namespace pronto::exec
{
  int run(const std::string& cmd)
  {
#ifdef _WIN32 

    auto wcmd = str::str2wstr(cmd);

    std::string str_result = "";
    HANDLE h_pipe_read, h_pipe_write;

    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
    saAttr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe to get results from child's stdout.
    if (!CreatePipe(&h_pipe_read, &h_pipe_write, &saAttr, 0))
      return str_result;

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.hStdOutput = h_pipe_write;
    si.hStdError = h_pipe_write;
    si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

    PROCESS_INFORMATION pi = { 0 };

    BOOL fSuccess = CreateProcessW(NULL, (LPWSTR)wcmd.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (!fSuccess)
    {
      CloseHandle(h_pipe_write);
      CloseHandle(h_pipe_read);
      return str_result;
    }

    bool process_ended = false;
    for (; !process_ended;)
    {
      // Give some timeslice (50ms), so we won't waste 100% cpu.
      process_ended = WaitForSingleObject(pi.hProcess, 20) == WAIT_OBJECT_0;

      // Even if process exited - we continue reading, if there is some data available over pipe.
      for (;;)
      {
        char buf[1024];
        DWORD dwRead = 0;
        DWORD dwAvail = 0;

        if (!::PeekNamedPipe(h_pipe_read, NULL, 0, NULL, &dwAvail, NULL))
          break;

        if (!dwAvail) // no data available, return
          break;

        if (!::ReadFile(h_pipe_read, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
          // error, the child process might ended
          break;

        buf[dwRead] = 0;
        str_result += buf;
      }
    } //for

    CloseHandle(h_pipe_write);
    CloseHandle(h_pipe_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return str_result;
#else
    system(s.c_str());
#endif // _WIN

  }
}
