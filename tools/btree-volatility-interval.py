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


filepath = 'btree_inconsistency_window/list_of_address_translations.txt'

pmap_dict = dict()
with open(filepath) as fp:
   line = fp.readline()
   cnt = 1
   while line:
       line = fp.readline()
       m = REMatcher(line)

       p_addr = None
       v_addr = None
       if (m.match(r"Translation Virtual page number: (.*) Physical page number:.*")):
            v_addr = m.group(1)

       if (m.match(r"Translation Virtual page number: .* Physical page number: (.*)")):
            p_addr = m.group(1)
       cnt += 1
       if v_addr is not None and p_addr is not None:
            # print("{} {}\n".format(str(v_addr), str(p_addr)))
            pmap_dict[str(hex(int(v_addr,16)))] = p_addr

filepath = 'btree_inconsistency_window/memory_controller_writes.txt'
mem_ctrl_dict = dict()
with open(filepath) as fp:
   line = fp.readline()
   while line:
       # print(line)
       line = fp.readline()
       m = REMatcher(line)

       time = None
       cache_block = None
       if (m.match(r".*: system.mem_ctrls: recvAtomic: Write.* (.*)")):
            cache_block = hex(int(str(m.group(1)), 16) & 0xFFFFC0)

       if (m.match(r"(.*): system.mem_ctrls: recvAtomic: Write.* .*")):
            time = m.group(1)
       cnt += 1
       # print("{} {}".format(str(time), str(cache_block)))
       if time is not None and cache_block is not None:
          if str(cache_block) in mem_ctrl_dict:
               # append the new number to the existing array at this slot
               mem_ctrl_dict[str(cache_block)].append(int(time))
          else:
               # create a new array in this slot
               mem_ctrl_dict[str(cache_block)] = [int(time)]
#print(mem_ctrl_dict)
filepath = 'btree_inconsistency_window/list_of_sibling_header_pairs.txt'
violation_count = 0
total_count = 0
total_inconsistent_time = 0

time_diff = []
with open(filepath) as fp:
   line = fp.readline()
   while line:
       # print(line)
       line = fp.readline()
       m = REMatcher(line)

       time = None
       sibling = None
       hdr = None

       if (m.match(r"(.*): WriteReq .*")):
            time = int(str(m.group(1)), 16)

       if (m.match(r".*: WriteReq .* Sibling and header are: (.*),.*")):
            sibling = str(m.group(1))
            sibling_virtual_pg = hex(int(str(m.group(1)), 16) & 0xFFF000)
            sibling_virtual_pg = hex(int(str(sibling_virtual_pg), 16) >> 12)
            sibling_physical_pg =  pmap_dict[sibling_virtual_pg]
            #print(sibling_physical_pg)
            sibling_physical_addr = hex((int(str(sibling_physical_pg), 16) << 12) | (int(str(m.group(1)), 16) & 0x000FFF))
            sibling = hex(int(str(sibling_physical_addr), 16) & 0xFFFFC0)
            #print(sibling_physical_addr)


       if (m.match(r".*: WriteReq .* Sibling and header are: .*,(.*)")):
            hdr = str(m.group(1))
            hdr_virtual_pg = hex(int(str(m.group(1)), 16) & 0xFFF000)
            hdr_virtual_pg = hex(int(str(hdr_virtual_pg), 16) >> 12)
            hdr_physical_pg =  pmap_dict[hdr_virtual_pg]
            #print(hdr_physical_pg)
            hdr_physical_addr = hex((int(str(hdr_physical_pg), 16) << 12) | (int(str(m.group(1)), 16) & 0x000FFF))
            hdr =  hex(int(str(hdr_physical_addr), 16) & 0xFFFFC0)
            #print(hdr_physical_addr)

       sibling_time = None
       hdr_time = None
       if str(sibling) in mem_ctrl_dict and str(hdr) in mem_ctrl_dict:
            total_count = total_count + 1

       if str(sibling) in mem_ctrl_dict:
            time_list = mem_ctrl_dict[str(sibling)]
            #print(int(time))
            #print(time_list)
            res = next((val for x, val in enumerate(time_list) 
                                   if int(val) > int(time)), None)
            #print(res)
            if res is not None:
                #print(res)
                sibling_time = int(res)
       if str(hdr) in mem_ctrl_dict:
            time_list = mem_ctrl_dict[str(hdr)]
            #print(int(time))
            #print(time_list)
            res = next((val for x, val in enumerate(time_list) 
                                   if int(val) > int(time)), None)
            #print(res)
            if res is not None:
                #print(res)
                hdr_time = int(res)
       if (sibling_time is not None and hdr_time is not None):
           time_diff.append(int(sibling_time) - int(time))
           time_diff.append(int(hdr_time) - int(time))
print("1st percentile of arr : ", 
       np.percentile(time_diff, 1))
print("5th percentile of arr : ", 
       np.percentile(time_diff, 5))
print("50th percentile of arr : ", 
       np.percentile(time_diff, 50))
print("95th percentile of arr : ",
       np.percentile(time_diff, 95))
print("99th percentile of arr : ",
       np.percentile(time_diff, 99))
print("Average of arr : ",
       np.mean(time_diff))