#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

void usage(const char* bin) {

  printf("Usage: %s SEC\n"
         "\n"
         "Sleep for SEC seconds and exit.\n", bin);

}

int main(int argc, char *argv[])
{

  if (argc < 2) {
    usage(argv[0]);
    return ERROR_BAD_ARGUMENTS;
  }
 
  int s = atoi(argv[1]);
  if(s == 0) {
    usage(argv[0]);
    return ERROR_BAD_ARGUMENTS;
  }

  Sleep(s*1000);

  return 0;
}
