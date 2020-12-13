import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import numpy as np
# import math


modes = ["no-ordering", "fast-fair", "full-ordering"]

wratios = [0, 25, 50, 75, 100]


def plot_lat(out_file):
    # Read latency numbers from file.
    lat_res = {}

    for w in wratios:
        lat_res[w] = {}
        
        for m in modes:
            lat_ns = 0.0

            with open(m+"-w"+str(w)+"-100000-lat.txt", 'r') as fl:
                for line in fl.readlines():
                    line = line.strip()

                    if "BTREE elapsed" in line:
                        lat_ns = float(line[line.find("(ns) avg: ")+10:line.find(", stdev")])
                        lat_stdev = float(line[line.find(", stdev: ")+9:])
                        break

            lat_res[w][m] = (lat_ns, lat_stdev)

    width = 0.24

    ax1 = plt.subplot2grid((1, 1), (0, 0))

    # Plotting.
    ax1.grid(True,
             axis='y',
             alpha=0.5,
             zorder=0)

    for i in range(len(modes)):
        lat_arr = [lat_res[w][modes[i]][0] for w in wratios]
        lat_stdevs = [lat_res[w][modes[i]][1] for w in wratios]

        ax1.bar(np.arange(len(wratios))+i*width, lat_arr,
                width=width,
                yerr=lat_stdevs,
                ecolor='black',
                capsize=3,
                label=modes[i],
                zorder=3)

        for j in range(len(wratios)):
            ax1.annotate("{:.0f}".format(lat_arr[j]),
                         xy=(j+i*width, lat_arr[j]+200),
                         ha='center',
                         va='bottom',
                         rotation=30,
                         fontsize=8)

    ax1.set_xticks(np.arange(len(wratios))+width)
    ax1.set_xticklabels([str(w)+"%" for w in wratios])

    ax1.set_ylim((0.0, 3200.0))

    # ax1.set_xlabel("Dataset (synthesized)")
    ax1.set_ylabel("Avg. latency of a user request (ns)")

    ax1.legend()

    # plt.title("Avg. latency (ns)\n"
    #           "of the three modes of B-trees over different datasets")

    plt.tight_layout()

    plt.savefig(out_file, dpi=120)
    plt.close()


def plot_pmc(out_file):
    # Read L3 misses from file.
    pmc_res = {}

    for w in wratios:
        pmc_res[w] = {}
        
        for m in modes:
            l1_cnt, l3_cnt = 0, 0

            with open(m+"-w"+str(w)+"-100000-pmc.txt", 'r') as fl:
                for line in fl.readlines():
                    line = line.strip()

                    if"num of L1 misses" in line:
                        l1_cnt = int(line[line.find("misses: ")+8:line.find(", avg:")])
                    elif "num of L3 misses" in line:
                        l3_cnt = int(line[line.find("misses: ")+8:line.find(", avg:")])
                        break

            # pmc_res[d][m] = float(l3_cnt) / float(l1_cnt)
            pmc_res[w][m] = float(l3_cnt) / (100000 / 100)

    width = 0.24

    ax1 = plt.subplot2grid((1, 1), (0, 0))

    # Plotting synth.
    ax1.grid(True,
             axis='y',
             alpha=0.5,
             zorder=0)

    for i in range(len(modes)):
        pmc_arr = [pmc_res[w][modes[i]] for w in wratios]

        ax1.bar(np.arange(len(wratios))+i*width, pmc_arr,
                width=width,
                # alpha=0.8,
                # hatch='\\',
                label=modes[i],
                zorder=3)

        for j in range(len(wratios)):
            ax1.annotate("{:.0f}".format(pmc_arr[j]),
                         xy=(j+i*width, pmc_arr[j]+30),
                         ha='center',
                         va='bottom',
                         rotation=30,
                         fontsize=8)

    ax1.set_xticks(np.arange(len(wratios))+width)
    ax1.set_xticklabels([str(w)+"%" for w in wratios])

    ax1.set_ylim((0.0, 1400.0))

    # ax1.set_xlabel("Dataset (synthesized)")
    ax1.set_ylabel("# L3 misses (x1000 misses per 100 insertions)")

    ax1.legend()

    # plt.title("# L3 misses (x1000 misses per 100 insertions)\n"
    #           "of the three modes of B-trees over different datasets")

    plt.tight_layout()

    plt.savefig(out_file, dpi=120)
    plt.close()


def main():
    plot_lat("perf-lat-wratio.png")
    plot_pmc("perf-pmc-wratio.png")


if __name__ == "__main__":
    main()
