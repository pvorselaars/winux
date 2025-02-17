#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

  if (argc < 2) {
    printf("Usage: %s PROG [ARGS]\n"
           "\n"
           "Run PROG and report times elapsed.\n"
           "\n"
           "\treal, the time between process creation and termination.\n"
           "\tuser, the time spent by all threads in user mode\n"
           "\tsys, the time spent by all threads in kernel mode.\n", argv[0]);
    return ERROR_BAD_ARGUMENTS;
  }
  
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  FILETIME created;
  FILETIME exit;
  FILETIME sys;
  FILETIME user;
  SYSTEMTIME wall_t;
  SYSTEMTIME created_t;
  SYSTEMTIME exit_t;
  SYSTEMTIME sys_t;
  SYSTEMTIME user_t;

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  memset(&pi, 0, sizeof(pi));

  char cmdline[MAX_PATH];
  for(int i = 1; i < argc; i++) {
    strcat_s(cmdline, MAX_PATH - strlen(cmdline), argv[i]);
    strcat_s(cmdline, MAX_PATH - strlen(cmdline), " ");
  }

  if(!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    fprintf(stderr, "%s: unable to create process\n", argv[0]);
    return GetLastError();
  }

  WaitForSingleObject(pi.hProcess, INFINITE);

  if(!GetProcessTimes(pi.hProcess, &created, &exit, &sys, &user))
    return GetLastError();

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  FileTimeToSystemTime(&created, &created_t);
  FileTimeToSystemTime(&exit, &exit_t);
  FileTimeToSystemTime(&sys, &sys_t);
  FileTimeToSystemTime(&user, &user_t);

  wall_t.wHour          = exit_t.wHour - created_t.wHour;
  wall_t.wMinute        = exit_t.wMinute - created_t.wMinute;
  wall_t.wSecond        = exit_t.wSecond - created_t.wSecond;
  wall_t.wMilliseconds  = exit_t.wMilliseconds - created_t.wMilliseconds;

  printf("real: %dh %dm %02ds %03dms\n", wall_t.wHour, wall_t.wMinute, wall_t.wSecond, wall_t.wMilliseconds);
  printf("user: %dh %dm %02ds %03dms\n", sys_t.wHour, sys_t.wMinute, sys_t.wSecond, sys_t.wMilliseconds);
  printf("sys:  %dh %dm %02ds %03dms\n", user_t.wHour, user_t.wMinute, user_t.wSecond, user_t.wMilliseconds);

  return 0;
}
