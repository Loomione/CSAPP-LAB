#include "cachelab.h"
#include "getopt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "unistd.h"

void usage() {
  printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n\n");
  printf("Examples:\n");
  printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
  printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char **argv) {
  // 命令行输入解析：   利用getopt()        man 3 getopt
  // ./csim -s 4 -E 1 -b 4 -t traces/yi.trace -v
  unsigned long address;
  int s, E, b, bytes, has_opt = 0, hits = 0, misses = 0, evictions = 0, v = 0;
  char *t;
  char input, type;
  FILE *fp;
  // cache_line **cache;
  /* 解析命令行参数 */
  while ((input = getopt(argc, argv, "s:E🅱️t:vh")) != -1) {
    has_opt = 1;
    switch (input) {
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      t = optarg;
      break;
    case 'v':
      v = 1;
      break;
    case 'h':
      print_usage();
      exit(0);
    default:
      print_usage();
      exit(-1);
    }
  }
  if (!has_opt) {
    printf("./csim: Missing required command line argument\n");
    print_usage();
    return 0;
  }
  printSummary(0, 0, 0);
  return 0;
}
