import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import numpy as np
# import math


modes = ["no-ordering", "fast-fair", "full-ordering"]

nthreads = [i for i in range(1, 9)]
num_keys = 2000000


def plot_tpt(out_file):
    # Read latency numbers from file and convert to throughput.
    tpt_res = {}

    for t in nthreads:
        tpt_res[t] = {}
        
        for m in modes:
            max_lat_ns = 0.0

            with open(m+"-t"+str(t)+"-"+str(num_keys)+".txt", 'r') as fl:
                for line in fl.readlines():
                    line = line.strip()

                    if "BTREE elapsed" in line:
                        lat_ns = float(line[line.find("(ns) avg: ")+10:line.find(", stdev")])
                        if lat_ns > max_lat_ns:
                            max_lat_ns = lat_ns

            total_ns = max_lat_ns * (num_keys / t)
            tpt_res[t][m] = num_keys / (total_ns / 1000000)

    markers = ['o', '^', 's']

    ax1 = plt.subplot2grid((1, 1), (0, 0))

    # Plotting.
    ax1.grid(True,
             axis='y',
             alpha=0.5,
             zorder=0)

    for i in range(len(modes)):
        tpt_arr = [tpt_res[t][modes[i]] for t in nthreads]

        ax1.plot(np.arange(len(nthreads)), tpt_arr,
                 marker=markers[i],
                 label=modes[i],
                 zorder=3)

        # for j in range(len(nthreads)):
        #     ax1.annotate("{:.0f}".format(tpt_arr[j]),
        #                  xy=(j+i*width, tpt_arr[j]+200),
        #                  ha='center',
        #                  va='bottom',
        #                  rotation=30,
        #                  fontsize=8)

    ax1.set_xticks(np.arange(len(nthreads)))
    ax1.set_xticklabels([str(t) for t in nthreads])

    # ax1.set_ylim((0.0, 3200.0))

    ax1.set_xlabel("Number of concurrent threads")
    ax1.set_ylabel("Throughput of concurrent B-tree (insertions / ms)")

    ax1.legend()

    # plt.title("Avg. latency (ns)\n"
    #           "of the three modes of B-trees over different datasets")

    plt.tight_layout()

    plt.savefig(out_file, dpi=120)
    plt.close()


def main():
    plot_tpt("perf-tpt-nthreads.png")


if __name__ == "__main__":
    main()
