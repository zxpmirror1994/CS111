#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include "ospfs.h"

int main(int argc, char **argv)
{
  int filetest;
  filetest = open("./test/hello.txt", O_RDONLY | O_CREAT);
  
  ioctl(filetest, IOC_CRASH, atoi(argv[1]));
  close(filetest);
}
