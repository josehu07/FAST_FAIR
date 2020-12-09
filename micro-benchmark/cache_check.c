#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define CPU_FREQ_MHZ (1994)
#define DELAY_IN_NS (1000)
#define CACHE_LINE_SIZE 64

#define NUM_UPDATES 100
#define SKIP_SLOTS 64

static inline void mfence() { asm volatile("mfence" ::: "memory"); }

unsigned long write_latency_in_ns = 0;
unsigned long long search_time_in_insert = 0;
unsigned int gettime_cnt = 0;
unsigned long long clflush_time_in_insert = 0;
unsigned long long update_time_in_insert = 0;
int clflush_cnt = 0;
int node_cnt = 0;

#define IS_FORWARD(c) (c % 2 == 0)

static inline void cpu_pause() { __asm__ volatile("pause" ::: "memory"); }

static inline unsigned long read_tsc(void) {
  unsigned long var;
  unsigned int hi, lo;

  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  var = ((unsigned long long int)hi << 32) | lo;

  return var;
}

static inline void clflush(char *data, int len) {
  volatile char *ptr = (char *)((unsigned long)data & ~(CACHE_LINE_SIZE - 1));
  mfence();
  for (; ptr < data + len; ptr += CACHE_LINE_SIZE) {
    unsigned long etsc =
        read_tsc() + (unsigned long)(write_latency_in_ns * CPU_FREQ_MHZ / 1000);
    asm volatile("clflush %0" : "+m"(*(volatile char *)ptr));
    /*while (read_tsc() < etsc)
      cpu_pause();*/
    //++clflush_cnt;
  }
  mfence();
}


int main() {
    char arr[NUM_UPDATES * (SKIP_SLOTS + 1)];
    unsigned long start_cycle = read_tsc();
    // Update different slots
    for (int i = 0; i < NUM_UPDATES; i++) {
        arr[i * SKIP_SLOTS] = 10;
        clflush((char*)(arr + (i * SKIP_SLOTS)), SKIP_SLOTS);
    }
    unsigned long end_cycle = read_tsc();
    printf("Num cycles: %ld\n", (end_cycle - start_cycle));
    return 0;
}
