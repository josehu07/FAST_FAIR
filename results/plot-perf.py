import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import numpy as np


modes = ["no-ordering", "fast-fair", "full-ordering"]
datasets = ["monotonic", "uniform"]


def plot_lat(out_file):
    # Read latency numbers from file.
    lat_res = {}

    for d in datasets:
        lat_res[d] = {}
        
        for m in modes:
            lat_us = 0.0

            with open(m+"-"+d+"-100000-lat.txt", 'r') as fl:
                for line in fl.readlines():
                    line = line.strip()

                    if "INSERT elapsed" in line:
                        lat_us = float(line[line.find(", avg: ")+7:])
                        break

            lat_res[d][m] = lat_us

    # Plotting.
    width = 0.24

    for i in range(len(modes)):
        lat_arr = [lat_res[d][modes[i]] for d in datasets]

        plt.bar(np.arange(len(datasets))+i*width, lat_arr,
                width=width, label=modes[i])

        for j in range(len(datasets)):
            plt.annotate("{:.2f}".format(lat_arr[j]),
                         xy=(j+i*width, lat_arr[j]),
                         ha='center', va='bottom')

    plt.xticks(np.arange(len(datasets))+width, datasets)

    plt.xlabel("Dataset (synthesized)")
    plt.ylabel("Avg. latency of an insertion (ns)")

    plt.legend()

    plt.title("Avg. latency (ns)\n"
              "of the three modes of B-trees over two datasets")

    plt.savefig(out_file, dpi=120)
    plt.close()


def plot_pmc(out_file):
    # Read L3 misses from file.
    pmc_res = {}

    for d in datasets:
        pmc_res[d] = {}
        
        for m in modes:
            pmc_cnt = 0

            with open(m+"-"+d+"-100000-pmc.txt", 'r') as fl:
                in_insert = False

                for line in fl.readlines():
                    line = line.strip()

                    if in_insert and "num of L3 misses" in line:
                        pmc_cnt = int(line[line.find("misses: ")+8:line.find(", avg:")])
                        break

                    if "INSERT elapsed" in line:
                        in_insert = True
                    else:
                        in_insert = False

            pmc_res[d][m] = pmc_cnt

    # Plotting.
    width = 0.24

    for i in range(len(modes)):
        pmc_arr = [pmc_res[d][modes[i]] for d in datasets]

        plt.bar(np.arange(len(datasets))+i*width, pmc_arr,
                width=width, label=modes[i])

        for j in range(len(datasets)):
            plt.annotate("{:d}".format(pmc_arr[j]),
                         xy=(j+i*width, pmc_arr[j]),
                         ha='center', va='bottom')

    plt.xticks(np.arange(len(datasets))+width, datasets)

    plt.xlabel("Dataset (synthesized)")
    plt.ylabel("Total # L3 misses of 100k insertions")

    plt.legend()

    plt.title("Total # L3 misses of 100k insertions\n"
              "of the three modes of B-trees over two datasets")

    plt.savefig(out_file, dpi=120)
    plt.close()


def main():
    plot_lat("perf-lat.png")
    plot_pmc("perf-pmc.png")


if __name__ == "__main__":
    main()
