/*
  Author:  Jeff Kinne
  Purpose: demonstration that read/inc/write doesn't give the answer if multiple
           programs doing it at once, and the read/inc/write isn't atomic.
           Use a file for this.
 */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  // standard argc check and usage statement
  if (argc < 3) {
    printf(1, "Usage: ./add filename.txt n\n");
    exit();
  }
  // standard grabbing command-line arguments
  char *filename = argv[1];
  int inc = atoi(argv[2]);

  int count;
  int f;

  int i, status;
  // repeat a number of times - read, inc, write
  for(i=0; i < inc; i++) {
    f = open(filename, O_RDONLY);
    if (f < 0) { // first time, no file yet
      printf(1, "File %s does not exist, will initialize to 0.\n", filename);
      count = 0;
    }
    else { // file yet, read count
      status = read(f, &count, sizeof(count));
      if (status < 0) {
        printf(1, "Unable to read file.\n");
        exit();
      }
      close(f);
    }
    // increase count
    count++;

    // write it out
    f = open(filename, O_WRONLY | O_CREATE);
    if (f < 0) {
      printf(1, "Unable to open/create file.\n");
      exit();
    }
    write(f, &count, sizeof(count));
    close(f);
  }

  printf(1, "Final value of count: %d\n", count);
  
  exit();
}
