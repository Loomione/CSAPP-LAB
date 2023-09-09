#include "cachelab.h"
#include "getopt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "unistd.h"

#define true 1
#define false 0

typedef int bool;

typedef struct CacheLine {
  bool vaildBit;
  int tag;
  size_t time; // 时间戳；
} CacheLine;

/*cache 初始化*/
CacheLine **Initiate(int set_index_bits, int associativity) {
  int sets = 1 << set_index_bits; // 组数量 2^s;
  unsigned int size;
  CacheLine **cache;
  cache = (CacheLine **)malloc(sizeof(CacheLine) * sets);
  for (int i = 0; i < sets; i++) {
    size = sizeof(CacheLine) * associativity;
    cache[i] = (CacheLine *)malloc(size);
    memset(cache[i], 0, size);
  }
  return cache;
}

/*释放Cache*/
void Clean(CacheLine **cache, int set_index_bits) {
  int sets = 1 << set_index_bits;
  for (int i = 0; i < sets; i++) {
    free(cache[i]);
  }
  free(cache);
}

/*获取当前时间戳*/
size_t GetCurTime() {
  static size_t time = 0;
  return time++;
}

/*判断是否hit*/
bool isHit(CacheLine *line, int line_length, unsigned long tag) {
  bool res = false;
  for (int i = 0; i < line_length; i++) {
    if (tag == line[i].tag && line[i].vaildBit == true) {
      res = true;
      line[i].time = GetCurTime();
      break;
    }
  }
  return res;
}
/*将一个缓存块放入缓存行中*/
bool PutInCache(CacheLine *line, int line_length, unsigned long tag) {
  int i, index = 0;
  bool result = false;
  size_t time = line[0].time;
  for (i = 0; i < line_length; i++) {
    if (line[i].vaildBit == false) {
      line[i].tag = tag;
      line[i].vaildBit = true;
      line[i].time = GetCurTime();
    }
  }
  /*没有空位了，使用LRU算法驱逐缓存块*/
  for (i = 0; i < line_length; i++) {
    if (time > line[i].time) {
      time = line[i].time;
      index = i;
    }
  }
  // 使用时间戳算法的缺点是 ： 每次都需要遍历整个缓存行；算法复杂度为O(n);
  // 可以替换为list + hash的实现方法，将时间复杂度降为O(1);
  result = true;
  line[index].tag = tag;
  line[index].vaildBit = true;
  line[index].time = GetCurTime();
  return result;
}

void PrintVerbosr(char *pre, char type, bool hit, bool eviction) {
  char *h = hit ? "hit" : "miss";
  char *e = eviction ? "eviction" : "";
  char *format;
  if (type == 'M') {
    format = "%s%s%s\n";
    strcat(pre, format);
    printf(pre, h, e, " hit");
  } else {
    format = "%s%s\n";
    strcat(pre, format);
    printf(pre, h, e);
  }
}
/* 缓存模拟核心逻辑 */
void CacheAccess(CacheLine **cache, int s, int E, int b, int v, int bytes,
                 int *hits, int *misses, int *evictions, unsigned long addr,
                 char type) {
  bool hit = false, eviction = false;
  char pre[20];
  unsigned long tag = addr >> (b + s),
                sets = ((addr << (64 - b - s)) >> (64 - s));
  CacheLine *set = cache[sets];
  hit = isHit(set, E, tag);
  if (!hit) {
    eviction = PutInCache(set, E, tag);
  }
  if (v) { // 打印信息
    sprintf(pre, "%c %lx,%d", type, addr, bytes);
    print_verbose(pre, type, hit, eviction);
  }
  // 统计数量
  *hits += hit;
  if (type == 'M') {
    *hits += 1;
  }
  *misses += !hit;
  *evictions += eviction;
}

void print_usage() {
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
