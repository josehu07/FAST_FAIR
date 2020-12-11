#include <papi.h>

#include "btree.h"

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

// MAIN
int main(int argc, char **argv) {
  // Parsing arguments
  int num_data = 0;
  int n_threads = 1;
  float selection_ratio = 0.0f;
  char *input_path = (char *) std::string("../sample_input.txt").data();

  printf("\n===== B-Tree App Output =====\n\n");

  int c;
  while ((c = getopt(argc, argv, "n:w:t:s:i:q")) != -1) {
    switch (c) {
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

  btree *bt;
  bt = new btree();

  struct timespec start, end;

  // Reading data
  entry_key_t *keys = new entry_key_t[num_data];

  ifstream ifs;
  ifs.open(input_path);
  if (!ifs) {
    cout << "input loading error!" << endl;

    delete[] keys;
    exit(-1);
  }

  for (int i = 0; i < num_data; ++i)
    ifs >> keys[i];

  ifs.close();

  // Initialize PAPI.
  int papi_event_set = PAPI_NULL;
  long long papi_values[4];

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

    if (PAPI_add_event(papi_event_set, PAPI_L3_TCM) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_L3_TCM event" << std::endl;
      return 1;
    }

    if (PAPI_add_event(papi_event_set, PAPI_L3_LDM) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_L3_LDM event" << std::endl;
      return 1;
    }

    if (PAPI_add_event(papi_event_set, PAPI_L3_STM) != PAPI_OK) {
      std::cerr << "Error: Failed to add PAPI_L3_STM event" << std::endl;
      return 1;
    }

    if (PAPI_start(papi_event_set) != PAPI_OK) {
      std::cerr << "Error: Failed to start PAPI event set" << std::endl;
      return 1;
    }
  }

  {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Zero the counters.
    if (!use_quartz) {
      if (PAPI_reset(papi_event_set) != PAPI_OK) {
        std::cerr << "Error: Failed to reset PAPI counters (1)" << std::endl;
        return 1;
      }
    }

    for (int i = 0; i < num_data; ++i) {
      bt->btree_insert(keys[i], (char *)keys[i]);
    }

    // Read PAPI counters.
    if (!use_quartz) {
      if (PAPI_read(papi_event_set, papi_values) != PAPI_OK) {
        std::cerr << "Error: Failed to read PAPI counters (1)" << std::endl;
        return 1;
      }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    long long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 +
                             (end.tv_nsec - start.tv_nsec);
    elapsed_time /= 1000;

    if (use_quartz) {
      printf("INSERT elapsed time (ns): %lld, avg: %lf\n",
             elapsed_time,   (double) elapsed_time   / num_data);
    } else {
      printf("INSERT elapsed cycles:    %lld, avg: %lf\n"
             "       num of L3 misses:  %lld, avg: %lf\n"
             "        L3 load  misses:  %lld, avg: %lf\n"
             "        L3 store misses:  %lld, avg: %lf\n",
             papi_values[0], (double) papi_values[0] / num_data,
             papi_values[1], (double) papi_values[1] / num_data
             papi_values[2], (double) papi_values[2] / num_data
             papi_values[3], (double) papi_values[3] / num_data);
    }
  }

  clear_cache();

  {
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Zero the counters.
    if (!use_quartz) {
      if (PAPI_reset(papi_event_set) != PAPI_OK) {
        std::cerr << "Error: Failed to reset PAPI counters (2)" << std::endl;
        return 1;
      }
    }

    for (int i = 0; i < num_data; ++i) {
      bt->btree_search(keys[i]);
    }

    // Read PAPI counters.
    if (!use_quartz) {
      if (PAPI_read(papi_event_set, papi_values) != PAPI_OK) {
        std::cerr << "Error: Failed to read PAPI counters (2)" << std::endl;
        return 1;
      }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    long long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 +
                             (end.tv_nsec - start.tv_nsec);
    elapsed_time /= 1000;

    if (use_quartz) {
      printf("SEARCH elapsed time (ns): %lld, avg: %lf\n",
             elapsed_time,   (double) elapsed_time   / num_data);
    } else {
      printf("SEARCH elapsed cycles:    %lld, avg: %lf\n"
             "       num of L3 misses:  %lld, avg: %lf\n"
             "        L3 load  misses:  %lld, avg: %lf\n"
             "        L3 store misses:  %lld, avg: %lf\n",
             papi_values[0], (double) papi_values[0] / num_data,
             papi_values[1], (double) papi_values[1] / num_data
             papi_values[2], (double) papi_values[2] / num_data
             papi_values[3], (double) papi_values[3] / num_data);
    }
  }

  // Stop PAPI counters.
  if (!use_quartz) {
    if (PAPI_stop(papi_event_set, papi_values) != PAPI_OK) {
      std::cerr << "Error: Failed to stop PAPI event set" << std::endl;
      return 1;
    }
  }

  delete bt;
  delete[] keys;

  return 0;
}
