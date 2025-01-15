/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 **************************************************************************************
 *                              
 *                    https://github.com/lst1975/TDMA-ng-Wifi
 *                              
 **************************************************************************************
 */
/*************************************************************************************
 *                               ngRTOS Kernel V2.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN ALL
 * COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. WHAT'S MORE, A DECLARATION OF 
 * NGRTOS MUST BE DISPLAYED IN THE FINAL SOFTWARE OR PRODUCT RELEASE. NGRTOS HAS 
 * NOT ANY LIMITATION OF CONTRIBUTIONS TO IT, WITHOUT ANY LIMITATION OF CODING STYLE, 
 * DRIVERS, CORE, APPLICATIONS, LIBRARIES, TOOLS, AND ETC. ANY LICENSE IS PERMITTED 
 * UNDER THE ABOVE LICENSE. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF 
 * ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO 
 * EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *
 *************************************************************************************
 *                              https://github.com/lst1975/ngRTOS
 *                              https://github.com/lst1975/neHttpd
 **************************************************************************************
 */

#include "nanohttp-defs.h"

/*/////////////////////////////////////////////////////////////////////////////////
                                      __arm__
/////////////////////////////////////////////////////////////////////////////////*/
#if defined(__aarch64__) || defined(__arm__) || defined(__arm) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__)
ng_uint64_t
get_tsc_freq_arch(void)
{
#if defined RTE_ARCH_ARM64 && !defined RTE_ARM_EAL_RDTSC_USE_PMU
	return __rte_arm64_cntfrq();
#elif defined RTE_ARCH_ARM64 && defined RTE_ARM_EAL_RDTSC_USE_PMU
#define CYC_PER_1MHZ 1E6
	/* Use the generic counter ticks to calculate the PMU
	 * cycle frequency.
	 */
	ng_uint64_t ticks;
	ng_uint64_t start_ticks, cur_ticks;
	ng_uint64_t start_pmu_cycles, end_pmu_cycles;

	/* Number of ticks for 1/10 second */
	ticks = __rte_arm64_cntfrq() / 10;

	start_ticks = __rte_arm64_cntvct_precise();
	start_pmu_cycles = rte_rdtsc_precise();
	do {
		cur_ticks = __rte_arm64_cntvct();
	} while ((cur_ticks - start_ticks) < ticks);
	end_pmu_cycles = rte_rdtsc_precise();

	/* Adjust the cycles to next 1Mhz */
	return RTE_ALIGN_MUL_CEIL(end_pmu_cycles - start_pmu_cycles,
			CYC_PER_1MHZ) * 10;
#else
	return 0;
#endif
}

/*/////////////////////////////////////////////////////////////////////////////////
                                      __x86_64__
/////////////////////////////////////////////////////////////////////////////////*/
#elif (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(__i386__) || defined(__i386))

/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2017 Intel Corporation
 */

#include <fcntl.h>
#include <unistd.h>
#ifdef RTE_TOOLCHAIN_MSVC
#define bit_AVX (1 << 28)
#else
#include <cpuid.h>
#endif

#define x86_vendor_amd(t1, t2, t3)        \
	((t1 == 0x68747541) && /* htuA */   \
	 (t2 == 0x444d4163) && /* DMAc */   \
	 (t3 == 0x69746e65))   /* itne */

static unsigned int
rte_cpu_get_model(ng_uint32_t fam_mod_step)
{
	ng_uint32_t family, model, ext_model;

	family = (fam_mod_step >> 8) & 0xf;
	model = (fam_mod_step >> 4) & 0xf;

	if (family == 6 || family == 15) {
		ext_model = (fam_mod_step >> 16) & 0xf;
		model += (ext_model << 4);
	}

	return model;
}

static ng_int32_t
rdmsr(int msr, ng_uint64_t *val)
{
#ifdef RTE_EXEC_ENV_LINUX
	int fd;
	int ret;

	fd = open("/dev/cpu/0/msr", O_RDONLY);
	if (fd < 0)
		return fd;

	ret = pread(fd, val, sizeof(ng_uint64_t), msr);

	close(fd);

	return ret;
#else
	RTE_SET_USED(msr);
	RTE_SET_USED(val);

	return -1;
#endif
}

static ng_uint32_t
check_model_wsm_nhm(ng_uint8_t model)
{
	switch (model) {
	/* Westmere */
	case 0x25:
	case 0x2C:
	case 0x2F:
	/* Nehalem */
	case 0x1E:
	case 0x1F:
	case 0x1A:
	case 0x2E:
		return 1;
	}

	return 0;
}

static ng_uint32_t
check_model_gdm_dnv(ng_uint8_t model)
{
	switch (model) {
	/* Goldmont */
	case 0x5C:
	/* Denverton */
	case 0x5F:
		return 1;
	}

	return 0;
}

#ifndef WIN32
#ifdef RTE_TOOLCHAIN_MSVC
int
__get_cpuid_max(unsigned int e, unsigned int *s)
{
	ng_uint32_t cpuinfo[4];

	__cpuid(cpuinfo, e);
	if (s)
		*s = cpuinfo[1];
	return cpuinfo[0];
}
#endif
#endif

ng_uint64_t
get_tsc_freq_arch(void)
{
#if defined(RTE_TOOLCHAIN_MSVC)
	int cpuinfo[4];
#endif
	ng_uint64_t tsc_hz = 0;
	ng_uint32_t a, b, c, d, maxleaf;
	ng_uint8_t mult, model;
	ng_int32_t ret;

#if defined(RTE_TOOLCHAIN_MSVC)
	__cpuid(cpuinfo, 0);
	a = cpuinfo[0];
	b = cpuinfo[1];
	c = cpuinfo[2];
	d = cpuinfo[3];
#else
	__cpuid(0, a, b, c, d);
#endif
	if (x86_vendor_amd(b, c, d))
		return 0;

	/*
	 * Time Stamp Counter and Nominal Core Crystal Clock
	 * Information Leaf
	 */
	maxleaf = __get_cpuid_max(0, NULL);

	if (maxleaf >= 0x15) {
#if defined(RTE_TOOLCHAIN_MSVC)
		__cpuid(cpuinfo, 0x15);
		a = cpuinfo[0];
		b = cpuinfo[1];
		c = cpuinfo[2];
		d = cpuinfo[3];
#else
		__cpuid(0x15, a, b, c, d);
#endif

		/* EBX : TSC/Crystal ratio, ECX : Crystal Hz */
		if (b && c)
			return c * (b / a);
	}

#if defined(RTE_TOOLCHAIN_MSVC)
	__cpuid(cpuinfo, 0x1);
	a = cpuinfo[0];
	b = cpuinfo[1];
	c = cpuinfo[2];
	d = cpuinfo[3];
#else
	__cpuid(0x1, a, b, c, d);
#endif
	model = rte_cpu_get_model(a);

	if (check_model_wsm_nhm(model))
		mult = 133;
	else if ((c & bit_AVX) || check_model_gdm_dnv(model))
		mult = 100;
	else
		return 0;

	ret = rdmsr(0xCE, &tsc_hz);
	if (ret < 0)
		return 0;

	return ((tsc_hz >> 8) & 0xff) * mult * 1E6;
}

/*/////////////////////////////////////////////////////////////////////////////////
                                      __riscv
/////////////////////////////////////////////////////////////////////////////////*/
#elif defined(__riscv)

/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Cavium, Inc
 * Copyright(c) 2022 StarFive
 * Copyright(c) 2022 SiFive
 * Copyright(c) 2022 Semihalf
 */

/** Read generic counter frequency */
static ng_uint64_t
__rte_riscv_timefrq(void)
{
#define TIMEBASE_FREQ_SIZE	8
	if (RTE_RISCV_TIME_FREQ > 0)
		return RTE_RISCV_TIME_FREQ;
	ng_uint8_t buf[TIMEBASE_FREQ_SIZE];
	ssize_t cnt;
	FILE *file;

	file = fopen("/proc/device-tree/cpus/timebase-frequency", "rb");
	if (!file)
		goto fail;

	cnt = fread(buf, 1, TIMEBASE_FREQ_SIZE, file);
	fclose(file);
	switch (cnt) {
	case 8:
		return rte_be_to_cpu_64(*(ng_uint64_t *)buf);
	case 4:
		return rte_be_to_cpu_32(*(ng_uint32_t *)buf);
	default:
		break;
	}
fail:
	log_warn("Unable to read timebase-frequency from FDT.");
	return 0;
}

ng_uint64_t
get_tsc_freq_arch(void)
{
	log_info("TSC using RISC-V %s.",
		RTE_RISCV_RDTSC_USE_HPM ? "rdcycle" : "rdtime");
	if (!RTE_RISCV_RDTSC_USE_HPM)
		return __rte_riscv_timefrq();
#define CYC_PER_1MHZ 1E6
	/*
	 * Use real time clock to estimate current cycle frequency
	 */
	ng_uint64_t ticks, frq;
	ng_uint64_t start_ticks, cur_ticks;
	ng_uint64_t start_cycle, end_cycle;

	/* Do not proceed unless clock frequency can be obtained. */
	frq = __rte_riscv_timefrq();
	if (!frq)
		return 0;

	/* Number of ticks for 1/10 second */
	ticks = frq / 10;

	start_ticks = __rte_riscv_rdtime_precise();
	start_cycle = rte_rdtsc_precise();
	do {
		cur_ticks = __rte_riscv_rdtime();
	} while ((cur_ticks - start_ticks) < ticks);
	end_cycle = rte_rdtsc_precise();

	/* Adjust the cycles to next 1Mhz */
	return RTE_ALIGN_MUL_CEIL((end_cycle - start_cycle) * 10, CYC_PER_1MHZ);
}

/*/////////////////////////////////////////////////////////////////////////////////
                                      __loongarch__
/////////////////////////////////////////////////////////////////////////////////*/
#elif defined(__loongarch__)

#define LOONGARCH_CPUCFG4	0x4
#define CPUCFG4_CCFREQ_MASK	0xFFFFFFFF
#define CPUCFG4_CCFREQ_SHIFT	0

#define LOONGARCH_CPUCFG5	0x5
#define CPUCFG5_CCMUL_MASK	0xFFFF
#define CPUCFG5_CCMUL_SHIFT	0

#define CPUCFG5_CCDIV_MASK	0xFFFF0000
#define CPUCFG5_CCDIV_SHIFT	16

static __rte_noinline ng_uint32_t
read_cpucfg(int arg)
{
	int ret = 0;

	__asm__ __volatile__ (
		"cpucfg %[var], %[index]\n"
		: [var]"=r"(ret)
		: [index]"r"(arg)
		:
		);

	return ret;
}

ng_uint64_t
get_tsc_freq_arch(void)
{
	ng_uint32_t base_freq, mul_factor, div_factor;

	base_freq = read_cpucfg(LOONGARCH_CPUCFG4);
	mul_factor = (read_cpucfg(LOONGARCH_CPUCFG5) & CPUCFG5_CCMUL_MASK) >>
		CPUCFG5_CCMUL_SHIFT;
	div_factor = (read_cpucfg(LOONGARCH_CPUCFG5) & CPUCFG5_CCDIV_MASK) >>
		CPUCFG5_CCDIV_SHIFT;

	return base_freq * mul_factor / div_factor;
}

/*/////////////////////////////////////////////////////////////////////////////////
                                      __powerpc__
/////////////////////////////////////////////////////////////////////////////////*/
#elif (defined(__powerpc__) || defined(__powerpc) || defined(__ppc__))

#include <features.h>
#ifdef __GLIBC__
#include <sys/platform/ppc.h>
#elif RTE_EXEC_ENV_LINUX
#include <string.h>
#include <stdio.h>
#endif

ng_uint64_t
get_tsc_freq_arch(void)
{
#ifdef __GLIBC__
	return __ppc_get_timebase_freq();
#elif RTE_EXEC_ENV_LINUX
	static unsigned long base;
	char buf[512];
	ssize_t nr;
	FILE *f;

	if (base != 0)
		goto out;

	f = fopen("/proc/cpuinfo", "rb");
	if (f == NULL)
		goto out;

	while (fgets(buf, sizeof(buf), f) != NULL) {
		char *ret = strstr(buf, "timebase");

		if (ret == NULL)
			continue;
		ret += sizeof("timebase") - 1;
		ret = strchr(ret, ':');
		if (ret == NULL)
			continue;
		base = strtoul(ret + 1, NULL, 10);
		break;
	}
	fclose(f);
out:
	return (ng_uint64_t) base;
#else
	return 0;
#endif

}

/*/////////////////////////////////////////////////////////////////////////////////
 ?
/////////////////////////////////////////////////////////////////////////////////*/
#else

extern ng_uint64_t ng_get_freq(void);
ng_uint64_t get_tsc_freq_arch(void)
{
  return ng_get_freq();
}

#endif

