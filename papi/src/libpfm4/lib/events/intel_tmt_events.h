/*
 * Contributed by Stephane Eranian <eranian@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of libpfm, a performance monitoring support library for
 * applications on Linux.
 *

 * PMU: intel_tmt (Intel Tremont)
 */

static const intel_x86_umask_t intel_tmt_ocr[]={
  { .uname   = "DEMAND_RFO_L3_MISS",
    .udesc   = "Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that was not supplied by the L3 cache.",
    .ucode   = 0x3f0400000200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "DEMAND_RFO_ANY_RESPONSE",
    .udesc   = "Counts all demand reads for ownership (RFO) requests and software based prefetches for exclusive ownership (PREFETCHW) that have any response type.",
    .ucode   = 0x1000200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "DEMAND_DATA_RD_L3_MISS",
    .udesc   = "Counts demand data reads that was not supplied by the L3 cache.",
    .ucode   = 0x3f0400000100ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "DEMAND_DATA_RD_ANY_RESPONSE",
    .udesc   = "Counts demand data reads that have any response type.",
    .ucode   = 0x1000100ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_mem_load_uops_retired[]={
  { .uname   = "L2_MISS",
    .udesc   = "Counts the number of load uops retired that miss in the level 2 cache",
    .ucode   = 0x1000ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "L1_MISS",
    .udesc   = "Counts the number of load uops retired that miss in the level 1 data cache",
    .ucode   = 0x0800ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "L3_HIT",
    .udesc   = "Counts the number of load uops retired that miss in the level 3 cache",
    .ucode   = 0x0400ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "L2_HIT",
    .udesc   = "Counts the number of load uops retired that hit in the level 2 cache",
    .ucode   = 0x0200ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "L1_HIT",
    .udesc   = "Counts the number of load uops retired that hit the level 1 data cache",
    .ucode   = 0x0100ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
};

static const intel_x86_umask_t intel_tmt_mem_uops_retired[]={
  { .uname   = "ALL_STORES",
    .udesc   = "Counts the number of store uops retired.",
    .ucode   = 0x8200ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "ALL_LOADS",
    .udesc   = "Counts the number of load uops retired.",
    .ucode   = 0x8100ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
};

static const intel_x86_umask_t intel_tmt_cycles_div_busy[]={
  { .uname   = "ANY",
    .udesc   = "Counts cycles the floating point divider or integer divider or both are busy.  Does not imply a stall waiting for either divider.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_DFL,
  },
};

static const intel_x86_umask_t intel_tmt_br_misp_retired[]={
  { .uname   = "ALL_BRANCHES",
    .udesc   = "Counts the number of mispredicted branch instructions retired.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_PEBS | INTEL_X86_DFL,
  },
};

static const intel_x86_umask_t intel_tmt_br_inst_retired[]={
  { .uname   = "ALL_BRANCHES",
    .udesc   = "Counts the number of branch instructions retired for all branch types.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_PEBS | INTEL_X86_DFL,
  },
};

static const intel_x86_umask_t intel_tmt_machine_clears[]={
  { .uname   = "ANY",
    .udesc   = "Counts all machine clears due to, but not limited to memory ordering, memory disambiguation, SMC, page faults and FP assist.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_DFL,
  },
};

static const intel_x86_umask_t intel_tmt_itlb_misses[]={
  { .uname   = "WALK_COMPLETED_2M_4M",
    .udesc   = "Page walk completed due to an instruction fetch in a 2M or 4M page.",
    .ucode   = 0x0400ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "WALK_COMPLETED_4K",
    .udesc   = "Page walk completed due to an instruction fetch in a 4K page.",
    .ucode   = 0x0200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_itlb[]={
  { .uname   = "FILLS",
    .udesc   = "Counts the number of times there was an ITLB miss and a new translation was filled into the ITLB.",
    .ucode   = 0x0400ull,
    .uflags  = INTEL_X86_DFL,
  },
};

static const intel_x86_umask_t intel_tmt_icache[]={
  { .uname   = "ACCESSES",
    .udesc   = "Counts requests to the Instruction Cache (ICache) for one or more bytes cache Line.",
    .ucode   = 0x0300ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "MISSES",
    .udesc   = "Counts requests to the Instruction Cache (ICache) for one or more bytes in a cache line and they do not hit in the ICache (miss).",
    .ucode   = 0x0200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_dtlb_store_misses[]={
  { .uname   = "WALK_COMPLETED_2M_4M",
    .udesc   = "Page walk completed due to a demand data store to a 2M or 4M page.",
    .ucode   = 0x0400ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "WALK_COMPLETED_4K",
    .udesc   = "Page walk completed due to a demand data store to a 4K page.",
    .ucode   = 0x0200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_longest_lat_cache[]={
  { .uname   = "REFERENCE",
    .udesc   = "Counts memory requests originating from the core that reference a cache line in the last level cache. If the platform has an L3 cache, last level cache is the L3, otherwise it is the L2.",
    .ucode   = 0x4f00ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "MISS",
    .udesc   = "Counts memory requests originating from the core that miss in the last level cache. If the platform has an L3 cache, last level cache is the L3, otherwise it is the L2.",
    .ucode   = 0x4100ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_dtlb_load_misses[]={
  { .uname   = "WALK_COMPLETED_2M_4M",
    .udesc   = "Page walk completed due to a demand load to a 2M or 4M page.",
    .ucode   = 0x0400ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "WALK_COMPLETED_4K",
    .udesc   = "Page walk completed due to a demand load to a 4K page.",
    .ucode   = 0x0200ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
};

static const intel_x86_umask_t intel_tmt_cpu_clk_unhalted[]={
  { .uname   = "REF",
    .udesc   = "Counts the number of unhalted reference clock cycles at TSC frequency.",
    .ucode   = 0x0100ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "CORE_P",
    .udesc   = "Counts the number of unhalted core clock cycles.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "CORE",
    .udesc   = "Counts the number of unhalted core clock cycles.",
    .uequiv  = "CORE_P",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_NCOMBO,
  },
  { .uname   = "REF_TSC",
    .udesc   = "Counts the number of unhalted reference clock cycles at TSC frequency. (Fixed event)",
    .ucode   = 0x0300ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_CODE_OVERRIDE,
  },
};

static const intel_x86_umask_t intel_tmt_inst_retired[]={
  { .uname   = "ANY_P",
    .udesc   = "Counts the number of instructions retired.",
    .ucode   = 0x0000ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
  { .uname   = "ANY",
    .udesc   = "Counts the number of instructions retired. (Fixed event)",
    .ucode   = 0x0100ull,
    .uflags  = INTEL_X86_NCOMBO | INTEL_X86_PEBS,
  },
};

static const intel_x86_entry_t intel_tmt_pe[]={
  { .name   = "UNHALTED_CORE_CYCLES",
    .desc   = "Count core clock cycles whenever the clock signal on the specific core is running (not halted)",
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0x20000000full,
    .code = 0x3c,
  },
  { .name   = "UNHALTED_REFERENCE_CYCLES",
    .desc   = "Unhalted reference cycles",
    .modmsk = INTEL_FIXED3_ATTRS,
    .cntmsk = 0x400000000ull,
    .code = 0x0300, /* pseudo encoding */
    .flags = INTEL_X86_FIXED,
  },
  { .name   = "INSTRUCTION_RETIRED",
    .desc   = "Number of instructions at retirement",
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0x10000000full,
    .code = 0xc0,
  },
  { .name   = "INSTRUCTIONS_RETIRED",
    .desc   = "This is an alias for INSTRUCTION_RETIRED",
    .modmsk = INTEL_V2_ATTRS,
    .equiv = "INSTRUCTION_RETIRED",
    .cntmsk = 0x10000000full,
    .code = 0xc0,
  },
  { .name   = "OCR",
    .desc   = "Counts demand data reads that have any response type.",
    .equiv  = "OFFCORE_RESPONSE_0",
    .code   = 0x01b7,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_NHM_OFFCORE,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_ocr),
    .umasks = intel_tmt_ocr,
  },
  { .name   = "OFFCORE_RESPONSE_0",
    .desc   = "Counts demand data reads that have any response type.",
    .code   = 0x01b7,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_NHM_OFFCORE,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_ocr),
    .umasks = intel_tmt_ocr,
  },
  { .name   = "OFFCORE_RESPONSE_1",
    .desc   = "Counts demand data reads that have any response type.",
    .code   = 0x02b7,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_NHM_OFFCORE,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_ocr),
    .umasks = intel_tmt_ocr,
  },
  { .name   = "MEM_LOAD_UOPS_RETIRED",
    .desc   = "Counts the number of load uops retired that hit the level 1 data cache",
    .code   = 0x00d1,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_PEBS,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_mem_load_uops_retired),
    .umasks = intel_tmt_mem_load_uops_retired,
  },
  { .name   = "MEM_UOPS_RETIRED",
    .desc   = "Counts the number of load uops retired.",
    .code   = 0x00d0,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_PEBS,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_mem_uops_retired),
    .umasks = intel_tmt_mem_uops_retired,
  },
  { .name   = "CYCLES_DIV_BUSY",
    .desc   = "Counts cycles the floating point divider or integer divider or both are busy.  Does not imply a stall waiting for either divider.",
    .code   = 0x00cd,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_cycles_div_busy),
    .umasks = intel_tmt_cycles_div_busy,
  },
  { .name   = "BR_MISP_RETIRED",
    .desc   = "Counts the number of mispredicted branch instructions retired.",
    .code   = 0x00c5,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_PEBS,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_br_misp_retired),
    .umasks = intel_tmt_br_misp_retired,
  },
  { .name   = "BR_INST_RETIRED",
    .desc   = "Counts the number of branch instructions retired for all branch types.",
    .code   = 0x00c4,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = INTEL_X86_PEBS,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_br_inst_retired),
    .umasks = intel_tmt_br_inst_retired,
  },
  { .name   = "MACHINE_CLEARS",
    .desc   = "Counts all machine clears due to, but not limited to memory ordering, memory disambiguation, SMC, page faults and FP assist.",
    .code   = 0x00c3,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_machine_clears),
    .umasks = intel_tmt_machine_clears,
  },
  { .name   = "ITLB_MISSES",
    .desc   = "Page walk completed due to an instruction fetch in a 4K page.",
    .code   = 0x0085,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_itlb_misses),
    .umasks = intel_tmt_itlb_misses,
  },
  { .name   = "ITLB",
    .desc   = "Counts the number of times there was an ITLB miss and a new translation was filled into the ITLB.",
    .code   = 0x0081,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_itlb),
    .umasks = intel_tmt_itlb,
  },
  { .name   = "ICACHE",
    .desc   = "Counts requests to the Instruction Cache (ICache) for one or more bytes in a cache line and they do not hit in the ICache (miss).",
    .code   = 0x0080,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_icache),
    .umasks = intel_tmt_icache,
  },
  { .name   = "DTLB_STORE_MISSES",
    .desc   = "Page walk completed due to a demand data store to a 4K page.",
    .code   = 0x0049,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_dtlb_store_misses),
    .umasks = intel_tmt_dtlb_store_misses,
  },
  { .name   = "LONGEST_LAT_CACHE",
    .desc   = "Counts memory requests originating from the core that miss in the last level cache. If the platform has an L3 cache, last level cache is the L3, otherwise it is the L2.",
    .code   = 0x002e,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_longest_lat_cache),
    .umasks = intel_tmt_longest_lat_cache,
  },
  { .name   = "DTLB_LOAD_MISSES",
    .desc   = "Page walk completed due to a demand load to a 4K page.",
    .code   = 0x0008,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_dtlb_load_misses),
    .umasks = intel_tmt_dtlb_load_misses,
  },
  { .name   = "CPU_CLK_UNHALTED",
    .desc   = "Counts the number of unhalted core clock cycles. (Fixed event)",
    .code   = 0x003c,
    .modmsk = INTEL_V2_ATTRS,
    .cntmsk = 0xfull,
    .ngrp   = 1,
    .flags  = 0,
    .numasks= LIBPFM_ARRAY_SIZE(intel_tmt_cpu_clk_unhalted),
    .umasks = intel_tmt_cpu_clk_unhalted,
  },
};
/* 15 events available */
