#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HOST_NAME_SIZE 1023

int main () {
  char* hostname;
  hostname = malloc((MAX_HOST_NAME_SIZE +1) * sizeof(char));

  gethostname(hostname, MAX_HOST_NAME_SIZE);
  printf("Hostname: %s\n", hostname);
  return 0;
}
