import re

# Python Program illustrating 
# numpy.percentile() method 
   
import numpy as np


class REMatcher(object):
    def __init__(self, matchstring):
        self.matchstring = matchstring

    def match(self, regexp):
        self.rematch = re.match(regexp, self.matchstring)
        return bool(self.rematch)

    def group(self, i):
        return self.rematch.group(i)

#filepath = 'mem_logs/mem_controller_writes.txt'
filepath = 'memlogs_no_ordering_actual/memory_controller_output.txt'

mem_ctrl_dict = dict()
mem_control_file = open("memctrl-final.txt", "w")
with open(filepath) as fp:
   line = fp.readline()
   cnt = 1
   while line:
       #print(line)
       line = fp.readline()
       m = REMatcher(line)

       time = None
       cache_block = None
       if (m.match(r".*: system.mem_ctrls: recvAtomic: WritebackDirty (.*)")):
            #print(m.group(1))
            #print(hex(int(str(m.group(1)), 16) & 0xFFFC0))
            cache_block = hex(int(str(m.group(1)), 16))

       if (m.match(r"(.*): system.mem_ctrls: recvAtomic: WritebackDirty .*")):
            #print(m.group(1))
            time = m.group(1)
       cnt += 1
       #print("{} {}".format(str(time), str(cache_block)))
       if time is not None and cache_block is not None:
          if str(cache_block) in mem_ctrl_dict:
               # append the new number to the existing array at this slot
               mem_ctrl_dict[str(cache_block)].append(int(time))
          else:
               # create a new array in this slot
               mem_ctrl_dict[str(cache_block)] = [int(time)]
          mem_control_file.write("{} {}\n".format(str(time), str(cache_block)))
mem_control_file.close()

#filepath = 'mem_logs/dcache_writes.txt'
filepath = 'memlogs_no_ordering_actual/cache_writes.txt'
dcache_writes_file = open("dcache_writes.txt", "w")
avg_volatile_time = 0
count = 0
time_diff = []
with open(filepath) as fp:
   line = fp.readline()
   cnt = 1
   while line:
       #print(line)
       line = fp.readline()
       m = REMatcher(line)

       time = None
       cache_block = None
       if (m.match(r".* WriteReq \[(.*)\:.*\].*")):
            #print(m.group(1))
            #print(hex(int(str(m.group(1)), 16) & 0xFFFC0))
            cache_block = hex(int(str(m.group(1)), 16) & 0xFFFC0)

       if (m.match(r"(.*): WriteReq .*")):
            #print(m.group(1))
            time = m.group(1)
       cnt += 1
       if time is not None and cache_block is not None:
            dcache_writes_file.write("{} {}\n".format(str(time), str(cache_block)))
            if str(cache_block) in mem_ctrl_dict:
               time_list = mem_ctrl_dict[str(cache_block)]
               # print(int(time))
               # print(time_list)
               res = next((x for x, val in enumerate(time_list) 
                                   if val > int(time)), None)
               if res is not None:
                    time_diff.append(int(time_list[int(res)]) - int(time))
                    #print("{} {} {} {}\n".format(int(time_list[int(res)]),  int(time), int(time_list[int(res)]) - int(time), str(cache_block)))
                    #print("{} {}\n".format(str(time), time_list[int(res)]))
                    #print("{}\n".format(int(time_list[int(res)]) - int(time)))
print("1st percentile of arr : ", 
       np.percentile(time_diff, 1))
print("5th percentile of arr : ", 
       np.percentile(time_diff, 5))
print("50th percentile of arr : ", 
       np.percentile(time_diff, 50))
print("99th percentile of arr : ",
       np.percentile(time_diff, 99))
print("Average of arr : ",
       np.mean(time_diff))
dcache_writes_file.close()

