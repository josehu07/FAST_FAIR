#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <numeric>

#include <pthread.h>
#include <sched.h>

#include <papi.h>

#include "btree.h"

static const int CHUNK_SIZE = 100;

void clear_cache() {
  // Remove cache
  int size = 256 * 1024 * 1024;
  char *garbage = new char[size];
  for (int i = 0; i < size; ++i)
    garbage[i] = i;
  for (int i = 100; i < size; ++i)
    garbage[i] += garbage[i - 100];
  delete[] garbage;
}

// BTREE
int thread_func(btree *bt, int *ops, entry_key_t *keys, int num_data) {
  struct timespec start, end;
  std::vector<double> elapsed_times;

  // Wait for cpuset core setting.
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Initialize PAPI.
  int papi_event_set = PAPI_NULL;
  long long papi_values[3];

  if (!use_quartz) {
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
      std::cerr << "Error: Failed to initialize PAPI library" << std::endl;
      return 1;
    }

    if (PAPI_create_eventset(&papi_event_set) != PAPI_OK) {
      std::cerr << "Error: Failed to create PAPI event set" << std::endl;
      return 1;
    }

    if (PAPI_add_event(papi_event_set, PAPI_TOT_INS) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_TOT_INS event" << std::endl;
      return 1;
    }

    if (PAPI_add_event(papi_event_set, PAPI_L1_DCM) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_L1_DCM event" << std::endl;
      return 1;
    }

    if (PAPI_add_event(papi_event_set, PAPI_L3_TCM) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_L3_TCM event" << std::endl;
      return 1;
    }

    // if (PAPI_add_event(papi_event_set, PAPI_L3_LDM) != PAPI_OK) {
    //   std::cerr << "Error: Failed to add PAPI_L3_LDM event" << std::endl;
    //   return 1;
    // }

    // if (PAPI_add_event(papi_event_set, PAPI_L3_STM) != PAPI_OK) {
    //   std::cerr << "Error: Failed to add PAPI_L3_STM event" << std::endl;
    //   return 1;
    // }

    if (PAPI_start(papi_event_set) != PAPI_OK) {
      std::cerr << "Error: Failed to start PAPI event set" << std::endl;
      return 1;
    }
  }

  clear_cache();

  {
    // Zero the counters.
    if (!use_quartz) {
      if (PAPI_reset(papi_event_set) != PAPI_OK) {
        std::cerr << "Error: Failed to reset PAPI counters (1)" << std::endl;
        return 1;
      }
    }

    if (use_quartz) {
      // Make requests in chunks of 100.
      for (int i = 0; i < num_data / CHUNK_SIZE; ++i) {
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int j = i * CHUNK_SIZE; j < (i+1) * CHUNK_SIZE; ++j) {

          // Decide op accoridng to opcode
          if (ops[j] == 0)
            bt->btree_search(keys[j]);
          else if (ops[j] == 2)
            bt->btree_insert(keys[j], (char *) keys[j]);
          else if (ops[j] == 3)
            bt->btree_delete(keys[j]);
          else {
            std::cerr << "Error: unknown opcode " << ops[j] << std::endl;
            exit(1);
          }
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        long long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 +
                                 (end.tv_nsec - start.tv_nsec);

        elapsed_times.push_back((double) elapsed_time / CHUNK_SIZE);  // push in avg for this chunk
      }
    } else {
      for (int i = 0; i < num_data; ++i) {
        bt->btree_insert(keys[i], (char *) keys[i]);
      }
    }

    // Read PAPI counters.
    if (!use_quartz) {
      if (PAPI_read(papi_event_set, papi_values) != PAPI_OK) {
        std::cerr << "Error: Failed to read PAPI counters (1)" << std::endl;
        return 1;
      }
    } 

    if (use_quartz) {
      double elapsed_time_sum = std::accumulate(elapsed_times.begin(), elapsed_times.end(), 0ll);
      double elapsed_time_avg = (double) elapsed_time_sum / elapsed_times.size();

      double elapsed_time_var = 0.0;
      std::for_each(elapsed_times.begin(), elapsed_times.end(),
                    [&](const double d) {
                      elapsed_time_var += (d - elapsed_time_avg) * (d - elapsed_time_avg);
                    });
      double elapsed_time_stdev = std::sqrt(elapsed_time_var / elapsed_times.size());

      printf(" BTREE elapsed time (ns) avg: %lf, stdev: %lf\n",
             elapsed_time_avg, elapsed_time_stdev);
    } else {
      // long long store_misses = papi_values[2] - papi_values[3];

      printf(" BTREE elapsed insts:  %lld, avg: %lf\n"
             "    num of L1 misses:  %lld, avg: %lf\n"
             "    num of L3 misses:  %lld, avg: %lf\n",
             papi_values[0], (double) papi_values[0] / num_data,
             papi_values[1], (double) papi_values[1] / num_data,
             papi_values[2], (double) papi_values[2] / num_data);
    }
  }

  // Stop PAPI counters.
  if (!use_quartz) {
    if (PAPI_stop(papi_event_set, papi_values) != PAPI_OK) {
      std::cerr << "Error: Failed to stop PAPI event set" << std::endl;
      return 1;
    }
  }
}

// MAIN
int main(int argc, char **argv) {
  // Parsing arguments
  int num_data = 0;
  int n_threads = 1;
  float selection_ratio = 0.0f;
  char *input_path = (char *) std::string("../sample_input.txt").data();
  int max_cpu_core_id = 7;

  printf("\n===== B-Tree App Output =====\n\n");

  int c;
  while ((c = getopt(argc, argv, "c:n:w:t:s:i:q")) != -1) {
    switch (c) {
    case 'c':
      max_cpu_core_id = atoi(optarg);
      break;
    case 'n':
      num_data = atoi(optarg);
      break;
    case 'w':
      write_latency_in_ns = atol(optarg);
      break;
    case 't':
      n_threads = atoi(optarg);
      break;
    case 's':
      selection_ratio = atof(optarg);
      break;
    case 'i':
      input_path = optarg;
      break;
    // [Proj 3] Using Quartz emulator.
    case 'q':
      use_quartz = true;
      break;
    default:
      break;
    }
  }

  if ((num_data < 100) || (num_data > 2000000)) {
    std::cerr << "Error: number of keys must be >= 100 and <= 2000000" << std::endl;
    return 1;
  }

  if (num_data % 100 != 0) {
    std::cerr << "Error: number of keys must be a multiple of 100" << std::endl;
    return 1;
  }

  if (max_cpu_core_id < n_threads - 1) {
    std::cerr << "Error: max cpu_core_id must be >= n_threads - 1" << std::endl;
    return 1;
  }

  btree *bt;
  bt = new btree();

  // Reading data
  entry_key_t *keys = new entry_key_t[num_data];
  int *ops = new int[num_data];

  ifstream ifs;
  ifs.open(input_path);
  if (!ifs) {
    std::cerr << "Error: input loading failed!" << endl;

    delete[] keys;
    exit(-1);
  }

  for (int i = 0; i < num_data; ++i)
    ifs >> ops[i] >> keys[i];

  ifs.close();

  // For each worker:
  std::vector<std::thread> workers;
  int cpu_core_id = max_cpu_core_id;

  int num_data_per_thread = num_data / n_threads;

  for (int i = 0; i < n_threads; ++i) {
    // Pin to one core and execute.
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core_id, &cpuset);

    int from = i * num_data_per_thread;
    int to = (i == n_threads - 1) ? num_data : (i+1) * num_data_per_thread;

    std::thread mb(thread_func, bt, ops,
                   keys + from, to - from);

    int rc = pthread_setaffinity_np(mb.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        std::cerr << "Error: Setting thread CPU core affinity: " << rc << std::endl;
        return 1;
    }

    workers.push_back(std::move(mb));
    cpu_core_id--;
  }

  for (int i = 0; i < n_threads; ++i)
    workers[i].join();  // Remember to join the threads.

  delete bt;
  delete[] keys;

  return 0;
}
