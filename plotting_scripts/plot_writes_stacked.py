import os
import sys
import matplotlib.pyplot as plt
import numpy as np

BENCHMARKS = ["uniform","monotonic","reverse"]

Result = open("result.txt","wb")

per_app_stats = {}

#Number of applications
N = len(BENCHMARKS) 
ind = np.arange(N)
number_of_loads = []
writes_with_no_ordering = [50579,63250,60869]

writecleans_with_no_ordering = [0,0,0]
writebacks_with_no_ordering = [50579,63250,60869]


writes_with_fast_fair = [549305,395642,593316]

writecleans_with_fast_fair = [419657,266842,465014]
writebacks_with_fast_fair = [129648,128800,128302]

writes_with_full_order = [1333648,635469,1443052]
writecleans_with_full_order = [1207802,506484,1314615]
writebacks_with_full_order = [125846,128895,128437]

#for plotting the average in addition

width = 0.1
h = plt.figure(3)
ax1 = plt.subplot2grid((1, 1), (0, 0))
ax1.bar(ind-0.1, writebacks_with_no_ordering, width, color='r', align='edge',label='No Order-WB',zorder=3,hatch='o',edgecolor='black')
ax1.bar(ind-0.1, writecleans_with_no_ordering, width, color='w',bottom=writebacks_with_no_ordering, align='edge',label='No Order-WC',zorder=3,hatch="-",edgecolor='black')
ax1.bar(ind, writebacks_with_fast_fair, width, color='g', align='edge',label='Fast Fair-WB',zorder=3,hatch='o',edgecolor='black') 
ax1.bar(ind, writecleans_with_fast_fair, width, color='y', align='edge',label='Fast Fair-WC',zorder=3,bottom=writebacks_with_fast_fair,hatch="-",edgecolor='black') 
ax1.bar(ind+0.1, writebacks_with_full_order, width, color='c', align='edge',label='Full Order-WB',zorder=3,hatch='o',edgecolor='black') 
ax1.bar(ind+0.1, writecleans_with_full_order, width, color='m', align='edge',label='Full Order-WC',zorder=3,bottom=writebacks_with_full_order,hatch="-",edgecolor='black') 
#plt.bar(ind+0.1, slowdown_with_pointer_chased_loads_stalled_random_eighth_prob, width, color='y', align='edge',label='Stall with prob 0.125')
ax1.set_ylabel('Writes Distribution')
ax1.set_title('Writes')
ax1.set_xlabel('Application names')
ax1.set_xticks(np.arange(len(ind)))
ax1.set_xticklabels([w for w in BENCHMARKS])
#ax1.ticklabel_format(style='plain')
ax1.grid(True,axis='y',alpha=0.5,zorder=0)
#ax1.yticks(np.arange(0.0,1500000,100000))
ax1.set_ylim((0.0, 1500000.0))
ax1.legend()
plt.savefig("Writebacks.png")
