/**************************************************************************************
 *          Embedded HTTP Server with Web Configuraion Framework  V2.0.0-beta
 *               TDMA Time-Sensitive-Network Wifi V1.0.1
 * Copyright (C) 2022 Songtao Liu, 980680431@qq.com.  All Rights Reserved.
 **************************************************************************************
 *
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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
 * Permission is hereby granted, http_free of charge, to any person obtaining a copy of
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

#include <math.h>
#include "nanohttp-defs.h"
#include "nanohttp-time.h"
#include "nanohttp-system.h"
#include "nanohttp-error.h"

/* Pointer to user delay function */
void (*rte_delay_us)(unsigned int) = NULL;

/* The frequency of the RDTSC timer resolution */
static uint64_t eal_tsc_resolution_hz;

#define MS_PER_S 1000
#define US_PER_S 1000000
#define NS_PER_S 1000000000

#if defined(__aarch64__) || defined(__arm__) || defined(__arm) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__)
uint64_t
get_tsc_freq_arch(void)
{
#if defined RTE_ARCH_ARM64 && !defined RTE_ARM_EAL_RDTSC_USE_PMU
	return __rte_arm64_cntfrq();
#elif defined RTE_ARCH_ARM64 && defined RTE_ARM_EAL_RDTSC_USE_PMU
#define CYC_PER_1MHZ 1E6
	/* Use the generic counter ticks to calculate the PMU
	 * cycle frequency.
	 */
	uint64_t ticks;
	uint64_t start_ticks, cur_ticks;
	uint64_t start_pmu_cycles, end_pmu_cycles;

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
rte_cpu_get_model(uint32_t fam_mod_step)
{
	uint32_t family, model, ext_model;

	family = (fam_mod_step >> 8) & 0xf;
	model = (fam_mod_step >> 4) & 0xf;

	if (family == 6 || family == 15) {
		ext_model = (fam_mod_step >> 16) & 0xf;
		model += (ext_model << 4);
	}

	return model;
}

static int32_t
rdmsr(int msr, uint64_t *val)
{
#ifdef RTE_EXEC_ENV_LINUX
	int fd;
	int ret;

	fd = open("/dev/cpu/0/msr", O_RDONLY);
	if (fd < 0)
		return fd;

	ret = pread(fd, val, sizeof(uint64_t), msr);

	close(fd);

	return ret;
#else
	RTE_SET_USED(msr);
	RTE_SET_USED(val);

	return -1;
#endif
}

static uint32_t
check_model_wsm_nhm(uint8_t model)
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

static uint32_t
check_model_gdm_dnv(uint8_t model)
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

#ifdef RTE_TOOLCHAIN_MSVC
int
__get_cpuid_max(unsigned int e, unsigned int *s)
{
	uint32_t cpuinfo[4];

	__cpuid(cpuinfo, e);
	if (s)
		*s = cpuinfo[1];
	return cpuinfo[0];
}
#endif

uint64_t
get_tsc_freq_arch(void)
{
#ifdef RTE_TOOLCHAIN_MSVC
	int cpuinfo[4];
#endif
	uint64_t tsc_hz = 0;
	uint32_t a, b, c, d, maxleaf;
	uint8_t mult, model;
	int32_t ret;

#ifdef RTE_TOOLCHAIN_MSVC
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
#ifdef RTE_TOOLCHAIN_MSVC
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

#ifdef RTE_TOOLCHAIN_MSVC
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
#elif defined(__riscv)
/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015 Cavium, Inc
 * Copyright(c) 2022 StarFive
 * Copyright(c) 2022 SiFive
 * Copyright(c) 2022 Semihalf
 */

#include <stdio.h>

#include "eal_private.h"
#include "rte_byteorder.h"
#include "rte_cycles.h"
#include "rte_log.h"

/** Read generic counter frequency */
static uint64_t
__rte_riscv_timefrq(void)
{
#define TIMEBASE_FREQ_SIZE	8
	if (RTE_RISCV_TIME_FREQ > 0)
		return RTE_RISCV_TIME_FREQ;
	uint8_t buf[TIMEBASE_FREQ_SIZE];
	ssize_t cnt;
	FILE *file;

	file = fopen("/proc/device-tree/cpus/timebase-frequency", "rb");
	if (!file)
		goto fail;

	cnt = fread(buf, 1, TIMEBASE_FREQ_SIZE, file);
	fclose(file);
	switch (cnt) {
	case 8:
		return rte_be_to_cpu_64(*(uint64_t *)buf);
	case 4:
		return rte_be_to_cpu_32(*(uint32_t *)buf);
	default:
		break;
	}
fail:
	log_warn("Unable to read timebase-frequency from FDT.");
	return 0;
}

uint64_t
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
	uint64_t ticks, frq;
	uint64_t start_ticks, cur_ticks;
	uint64_t start_cycle, end_cycle;

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
#elif defined(__loongarch__)
#define LOONGARCH_CPUCFG4	0x4
#define CPUCFG4_CCFREQ_MASK	0xFFFFFFFF
#define CPUCFG4_CCFREQ_SHIFT	0

#define LOONGARCH_CPUCFG5	0x5
#define CPUCFG5_CCMUL_MASK	0xFFFF
#define CPUCFG5_CCMUL_SHIFT	0

#define CPUCFG5_CCDIV_MASK	0xFFFF0000
#define CPUCFG5_CCDIV_SHIFT	16

static __rte_noinline uint32_t
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

uint64_t
get_tsc_freq_arch(void)
{
	uint32_t base_freq, mul_factor, div_factor;

	base_freq = read_cpucfg(LOONGARCH_CPUCFG4);
	mul_factor = (read_cpucfg(LOONGARCH_CPUCFG5) & CPUCFG5_CCMUL_MASK) >>
		CPUCFG5_CCMUL_SHIFT;
	div_factor = (read_cpucfg(LOONGARCH_CPUCFG5) & CPUCFG5_CCDIV_MASK) >>
		CPUCFG5_CCDIV_SHIFT;

	return base_freq * mul_factor / div_factor;
}
#elif (defined(__powerpc__) || defined(__powerpc) || defined(__ppc__))
#include <features.h>
#ifdef __GLIBC__
#include <sys/platform/ppc.h>
#elif RTE_EXEC_ENV_LINUX
#include <string.h>
#include <stdio.h>
#endif

uint64_t
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
	return (uint64_t) base;
#else
	return 0;
#endif

}
#else
uint64_t get_tsc_freq_arch(void)
{
  return ng_get_freq();
}
#endif

static uint64_t
estimate_tsc_freq(void)
{
#define CYC_PER_10MHZ 1E7
	log_warn("WARNING: TSC frequency estimated roughly"
		" - clock timings may be less accurate.");
	/* assume that the rte_delay_us_sleep() will sleep for 1 second */
	uint64_t start = rte_rdtsc();
	rte_delay_us_sleep(US_PER_S);
	/* Round up to 10Mhz. 1E7 ~ 10Mhz */
	return RTE_ALIGN_MUL_NEAR(rte_rdtsc() - start, CYC_PER_10MHZ);
}

void
set_tsc_freq(void)
{
	uint64_t freq;

	freq = get_tsc_freq_arch();
	if (!freq)
		freq = get_tsc_freq();
	if (!freq)
		freq = estimate_tsc_freq();

	log_debug("TSC frequency is ~%" PRIu64 " KHz", freq / 1000);
	eal_tsc_resolution_hz = freq;
	ng_os_info.tsc_hz = freq;
}

#if defined(_MSC_VER) || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#define US_PER_SEC 1E6
#define CYC_PER_10MHZ 1E7

void
rte_delay_us_sleep(unsigned int us)
{
	HANDLE timer;
	LARGE_INTEGER due_time;

	/* create waitable timer */
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (!timer) {
		RTE_LOG_WIN32_ERR("CreateWaitableTimer()");
		rte_errno = ENOMEM;
		return;
	}

	/*
	 * due_time's uom is 100 ns, multiply by 10 to convert to microseconds
	 * set us microseconds time for timer
	 */
	due_time.QuadPart = -((int64_t)us * 10);
	if (!SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE)) {
		RTE_LOG_WIN32_ERR("SetWaitableTimer()");
		rte_errno = EINVAL;
		goto end;
	}
	/* start wait for timer for us microseconds */
	if (WaitForSingleObject(timer, INFINITE) == WAIT_FAILED) {
		RTE_LOG_WIN32_ERR("WaitForSingleObject()");
		rte_errno = EINVAL;
	}

end:
	CloseHandle(timer);
}

uint64_t
get_tsc_freq(void)
{
	LARGE_INTEGER t_start, t_end, elapsed_us;
	LARGE_INTEGER frequency;
	uint64_t tsc_hz;
	uint64_t end, start;

	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&t_start);
	start = rte_get_tsc_cycles();

	rte_delay_us_sleep(US_PER_SEC / 10); /* 1/10 second */

	if (rte_errno != 0)
		return 0;

	QueryPerformanceCounter(&t_end);
	end = rte_get_tsc_cycles();

	elapsed_us.QuadPart = t_end.QuadPart - t_start.QuadPart;

	/*
	 * To guard against loss-of-precision, convert to microseconds
	 * *before* dividing by ticks-per-second.
	 */
	elapsed_us.QuadPart *= US_PER_SEC;
	elapsed_us.QuadPart /= frequency.QuadPart;

	double secs = ((double)elapsed_us.QuadPart)/US_PER_SEC;
	tsc_hz = (uint64_t)((end - start)/secs);

	/* Round up to 10Mhz. 1E7 ~ 10Mhz */
	return RTE_ALIGN_MUL_NEAR(tsc_hz, CYC_PER_10MHZ);
}

int
rte_eal_timer_init(void)
{
	set_tsc_freq();
	return 0;
}
#else
void
rte_delay_us_sleep(unsigned int us)
{
	struct timespec wait[2];
	int ind = 0;

	wait[0].tv_sec = 0;
	if (us >= US_PER_S) {
		wait[0].tv_sec = us / US_PER_S;
		us -= wait[0].tv_sec * US_PER_S;
	}
	wait[0].tv_nsec = 1000 * us;

	while (nanosleep(&wait[ind], &wait[1 - ind]) && errno == EINTR) {
		/*
		 * Sleep was interrupted. Flip the index, so the 'remainder'
		 * will become the 'request' for a next call.
		 */
		ind = 1 - ind;
	}
}
#if defined(__FreeBSD__)
uint64_t
get_tsc_freq(void)
{
	size_t sz;
	int tmp;
	uint64_t tsc_hz;

	sz = sizeof(tmp);
	tmp = 0;

	if (sysctlbyname("kern.timecounter.smp_tsc", &tmp, &sz, NULL, 0))
		EAL_LOG(WARNING, "%s", strerror(errno));
	else if (tmp != 1)
		EAL_LOG(WARNING, "TSC is not safe to use in SMP mode");

	tmp = 0;

	if (sysctlbyname("kern.timecounter.invariant_tsc", &tmp, &sz, NULL, 0))
		EAL_LOG(WARNING, "%s", strerror(errno));
	else if (tmp != 1)
		EAL_LOG(WARNING, "TSC is not invariant");

	sz = sizeof(tsc_hz);
	if (sysctlbyname("machdep.tsc_freq", &tsc_hz, &sz, NULL, 0)) {
		EAL_LOG(WARNING, "%s", strerror(errno));
		return 0;
	}

	return tsc_hz;
}

int
rte_eal_timer_init(void)
{
	set_tsc_freq();
	return 0;
}

#else

uint64_t
get_tsc_freq(void)
{
#ifdef CLOCK_MONOTONIC_RAW
#define NS_PER_SEC 1E9
#define CYC_PER_10MHZ 1E7

	struct timespec sleeptime = {.tv_nsec = NS_PER_SEC / 10 }; /* 1/10 second */

	struct timespec t_start, t_end;
	uint64_t tsc_hz;

	if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_start) == 0) {
		uint64_t ns, end, start = rte_rdtsc();
		nanosleep(&sleeptime,NULL);
		clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
		end = rte_rdtsc();
		ns = ((t_end.tv_sec - t_start.tv_sec) * NS_PER_SEC);
		ns += (t_end.tv_nsec - t_start.tv_nsec);

		double secs = (double)ns/NS_PER_SEC;
		tsc_hz = (uint64_t)((end - start)/secs);
		/* Round up to 10Mhz. 1E7 ~ 10Mhz */
		return RTE_ALIGN_MUL_NEAR(tsc_hz, CYC_PER_10MHZ);
	}
#endif
	return 0;
}
int
rte_eal_timer_init(void)
{
	set_tsc_freq();
	return 0;
}
#endif

#endif

/**
 * This call is easily portable to any architecture, however,
 * it may require a system call and imprecise for some tasks.
 */
uint64_t
os__rte_rdtsc_syscall(void)
{
	struct timespec val;
	uint64_t v;

	while (clock_gettime(CLOCK_MONOTONIC_RAW, &val) != 0)
		/* no body */;

	v  = (uint64_t) val.tv_sec * 1000000000LL;
	v += (uint64_t) val.tv_nsec;
	return v;
}

uint64_t
rte_get_tsc_hz(void)
{
	return eal_tsc_resolution_hz;
}

/**
 * Get the number of cycles in one second for the default timer.
 *
 * @return
 *   The number of cycles in one second.
 */
uint64_t
rte_get_timer_hz(void)
{
	uint64_t freq = rte_get_tsc_hz();
  if (!freq) freq = ng_os_info.freq;
  return freq;
}

void
rte_delay_us_block(unsigned int us)
{
	const uint64_t start = rte_get_timer_cycles();
	const uint64_t ticks = (uint64_t)us * rte_get_timer_hz() / 1E6;
	while ((rte_get_timer_cycles() - start) < ticks)
		rte_pause();
}

static double startime;  
static uint64_t starcycles;  

uint64_t ng_get_time(void)
{
  uint64_t endCycles;
  endCycles = rte_get_timer_cycles();
  return startime+round(
    ng_cycles2sec(ng_difftime(endCycles,starcycles), 
      ng_os_info.freq));
}

void ng_update_time(void)
{
  ng_tmv_s tv;
  ng_gettimeofday(&tv);
  starcycles = rte_get_timer_cycles();
  startime   = tv.tv_sec+tv.tv_usec/1000000.0;
}

#define __ng_YEARS_N  251
#define __ng_MONTHS_N 11
#define __ng_DAYS_N   31
#define __ng_HOURS_N  23

static const uint64_t __ng_YEARS[256] = {
         0UL,
  31536000UL,  63072000UL,  94694400UL, 126230400UL, 157766400UL, 189302400UL,
 220924800UL, 252460800UL, 283996800UL, 315532800UL, 347155200UL, 378691200UL,
 410227200UL, 441763200UL, 473385600UL, 504921600UL, 536457600UL, 567993600UL,
 599616000UL, 631152000UL, 662688000UL, 694224000UL, 725846400UL, 757382400UL,
 788918400UL, 820454400UL, 852076800UL, 883612800UL, 915148800UL, 946684800UL,
 978307200UL,1009843200UL,1041379200UL,1072915200UL,1104537600UL,1136073600UL,
1167609600UL,1199145600UL,1230768000UL,1262304000UL,1293840000UL,1325376000UL,
1356998400UL,1388534400UL,1420070400UL,1451606400UL,1483228800UL,1514764800UL,
1546300800UL,1577836800UL,1609459200UL,1640995200UL,1672531200UL,1704067200UL,
1735689600UL,1767225600UL,1798761600UL,1830297600UL,1861920000UL,1893456000UL,
1924992000UL,1956528000UL,1988150400UL,2019686400UL,2051222400UL,2082758400UL,
2114380800UL,2145916800UL,2177452800UL,2208988800UL,2240611200UL,2272147200UL,
2303683200UL,2335219200UL,2366841600UL,2398377600UL,2429913600UL,2461449600UL,
2493072000UL,2524608000UL,2556144000UL,2587680000UL,2619302400UL,2650838400UL,
2682374400UL,2713910400UL,2745532800UL,2777068800UL,2808604800UL,2840140800UL,
2871763200UL,2903299200UL,2934835200UL,2966371200UL,2997993600UL,3029529600UL,
3061065600UL,3092601600UL,3124224000UL,3155760000UL,3187296000UL,3218832000UL,
3250454400UL,3281990400UL,3313526400UL,3345062400UL,3376684800UL,3408220800UL,
3439756800UL,3471292800UL,3502915200UL,3534451200UL,3565987200UL,3597523200UL,
3629145600UL,3660681600UL,3692217600UL,3723753600UL,3755376000UL,3786912000UL,
3818448000UL,3849984000UL,3881606400UL,3913142400UL,3944678400UL,3976214400UL,
4007836800UL,4039372800UL,4070908800UL,4102444800UL,4133980800UL,4165516800UL,
4197052800UL,4228588800UL,4260211200UL,4291747200UL,4323283200UL,4354819200UL,
4386441600UL,4417977600UL,4449513600UL,4481049600UL,4512672000UL,4544208000UL,
4575744000UL,4607280000UL,4638902400UL,4670438400UL,4701974400UL,4733510400UL,
4765132800UL,4796668800UL,4828204800UL,4859740800UL,4891363200UL,4922899200UL,
4954435200UL,4985971200UL,5017593600UL,5049129600UL,5080665600UL,5112201600UL,
5143824000UL,5175360000UL,5206896000UL,5238432000UL,5270054400UL,5301590400UL,
5333126400UL,5364662400UL,5396284800UL,5427820800UL,5459356800UL,5490892800UL,
5522515200UL,5554051200UL,5585587200UL,5617123200UL,5648745600UL,5680281600UL,
5711817600UL,5743353600UL,5774976000UL,5806512000UL,5838048000UL,5869584000UL,
5901206400UL,5932742400UL,5964278400UL,5995814400UL,6027436800UL,6058972800UL,
6090508800UL,6122044800UL,6153667200UL,6185203200UL,6216739200UL,6248275200UL,
6279897600UL,6311433600UL,6342969600UL,6374505600UL,6406128000UL,6437664000UL,
6469200000UL,6500736000UL,6532358400UL,6563894400UL,6595430400UL,6626966400UL,
6658588800UL,6690124800UL,6721660800UL,6753196800UL,6784819200UL,6816355200UL,
6847891200UL,6879427200UL,6911049600UL,6942585600UL,6974121600UL,7005657600UL,
7037280000UL,7068816000UL,7100352000UL,7131888000UL,7163510400UL,7195046400UL,
7226582400UL,7258118400UL,7289654400UL,7321190400UL,7352726400UL,7384262400UL,
7415884800UL,7447420800UL,7478956800UL,7510492800UL,7542115200UL,7573651200UL,
7605187200UL,7636723200UL,7668345600UL,7699881600UL,7731417600UL,7762953600UL,
7794576000UL,7826112000UL,7857648000UL,7889184000UL,7920806400UL,7952342400UL};

static const uint32_t __ng_MONTHS[16] = {
  0U, 2678400U,  5097600U,  7776000U, 10368000U, 13046400U, 15638400U, 
  18316800U, 20995200U, 23587200U, 26265600U, 28857600U, 31536000U};
static const uint32_t __ng_MONTHS_leap[16] = {
  0U, 2678400U, 5184000U, 7862400U, 10454400U, 13132800U, 15724800U, 
  18403200U, 21081600U, 23673600U, 26352000U, 28944000U, 31622400U};
static const uint32_t __ng_DAYS[32] = {
  0U,    86400U,  172800U,  259200U,  345600U,  432000U,  518400U,
  604800U,  691200U,  777600U,  864000U,  950400U, 1036800U, 1123200U, 
 1209600U, 1296000U, 1382400U, 1468800U, 1555200U, 1641600U, 1728000U, 
 1814400U, 1900800U, 1987200U, 2073600U, 2160000U, 2246400U, 2332800U, 
 2419200U, 2505600U, 2592000U, 2678400U};
static const uint32_t __ng_HOURS[32] = {
  0U, 
  3600U,  7200U, 10800U, 14400U, 18000U, 21600U, 25200U, 28800U, 32400U, 
  36000U, 39600U, 43200U, 46800U, 50400U, 54000U, 57600U, 61200U, 64800U, 
  68400U, 72000U, 75600U, 79200U, 82800U, 86400U};
static const uint32_t __ng_MINUTES[64] = {
  0U, 60U, 120U, 180U, 240U, 300U, 360U, 420U, 480U, 540U, 600U, 660U, 720U, 
  780U, 840U, 900U, 960U, 1020U, 1080U, 1140U, 1200U, 1260U, 1320U, 1380U, 
  1440U, 1500U, 1560U, 1620U, 1680U, 1740U, 1800U, 1860U, 1920U, 1980U, 
  2040U, 2100U, 2160U, 2220U, 2280U, 2340U, 2400U, 2460U, 2520U, 2580U, 
  2640U, 2700U, 2760U, 2820U, 2880U, 2940U, 3000U, 3060U, 3120U, 3180U, 
  3240U, 3300U, 3360U, 3420U, 3480U, 3540U, 3600U};
  
static const uint32_t __ng_mon_DAYS[32] = {
  31, 28, 31,  30, 31, 30, 31, 31, 30, 31, 30, 31,
  31, 29, 31,  30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint32_t __ng_year_DAYS[32] = {
  0,  31, 59, 90,  120, 151, 181, 212, 243, 273, 304, 334, 365,
  0,  31, 60, 91,  121, 152, 182, 213, 244, 274, 305, 335, 366};

static __ng_inline__ int ___IS_LEAP(int y) 
{
  return ((((y) & 0x3) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0);
}

static __ng_inline__ int __is_invalid_tm(ng_rtc_time_s *tm)
{
  return (tm->tm_year < 70 
      || tm->tm_year > 325 
      || tm->tm_mon  > 11 
      || tm->tm_mday < 1 
      || tm->tm_mday > 31 
      || tm->tm_hour > 23 
      || tm->tm_min  > 59 
      || tm->tm_sec  > 60);
}

/* Unix time is total number of seconds measured since  01-Jan-1970 00:00:00.  
  struct tm {
      int         tm_sec;    // Seconds          [0, 60]
      int         tm_min;    // Minutes          [0, 59]
      int         tm_hour;   // Hour             [0, 23]
      int         tm_mday;   // Day of the month [1, 31]
      int         tm_mon;    // Month            [0, 11]  (January = 0) 
      int         tm_year;   // Year minus 1900 
      int         tm_wday;   // Day of the week  [0, 6]   (Sunday = 0)
      int         tm_yday;   // Day of the year  [0, 365] (Jan/01 = 0)
      int         tm_isdst;  // Daylight savings flag 

      long        tm_gmtoff; // Seconds East of UTC 
      const char *tm_zone;   // Timezone abbreviation
  };
 */
ng_result_t
ng_tm2unix_time(ng_rtc_time_s *tm, ng_tmv_s *tv) 
{
  if (__is_invalid_tm(tm))
    return ng_ERR_EINVAL;
  
  tv->tv_sec =  __ng_YEARS[tm->tm_year + 1900 - 1970] 
        + (___IS_LEAP(tm->tm_year + 1900) ? 
              __ng_MONTHS_leap[tm->tm_mon] : 
              __ng_MONTHS[tm->tm_mon]) 
        + __ng_DAYS[tm->tm_mday-1]
        + __ng_HOURS[tm->tm_hour] 
        + __ng_MINUTES[tm->tm_min] 
        + tm->tm_sec;
  tv->tv_usec = tm->tm_ms * MS_PER_S;
  return ng_ERR_NONE;              
}

#define SECSPERYEAR  31536000  /* 3600*24*365 */
#define SECSPERMONTH  2419200  /* 3600*24*28 */
#define SECSPERDAY   86400     /* 3600*24 */
#define JAN11900DOW  (4U)      /* Jan 1, 1970 was a Wensday */
#define ZONE_OFFSET  28800

#define __ng_div_find(t, m, i) do { \
  while (m[i+1] < t) \
    i++;\
}while(0)

ng_result_t
ng_unix2tm_time(ng_rtc_time_s *tm, ng_tmv_s *tv, int tz_offset) 
{
  int i, leap = 0;
  const uint32_t *m;
  time_t day_in_epoch;
  time_t t = tv->tv_sec;
  
  t += tz_offset;

  tm->tm_ms   = tv->tv_usec/MS_PER_S;
  tm->tm_sec  = t%60;
  t -= tm->tm_sec;
  tm->tm_gmtoff = tz_offset;
  
  /* Now what day since the beginning of NTP time, and 
   * the day of week. 
   */
  day_in_epoch = t / SECSPERDAY;
  tm->tm_wday = (day_in_epoch + JAN11900DOW) % 7;

  i = t / SECSPERYEAR;
  if (i > __ng_YEARS_N)
    return ng_ERR_EINVAL;
  
  __ng_div_find(t, __ng_YEARS, i);
  if (t == __ng_YEARS[i])
  {
    tm->tm_year = i + 1970 - 1900;
    tm->tm_mon  = 0;
    tm->tm_mday = 1;
    tm->tm_hour = 0;
    tm->tm_min  = 0;
    tm->tm_sec  = 0;
    return ng_ERR_NONE;
  }
  tm->tm_year = i + 1970 - 1900;
  t -= __ng_YEARS[i];
  leap = ___IS_LEAP(tm->tm_year + 1900);

  i = t / SECSPERMONTH-1;
  if (i > __ng_MONTHS_N)
    return ng_ERR_EINVAL;
  m = leap ? __ng_MONTHS_leap : __ng_MONTHS;
  __ng_div_find(t, m, i);
  t -= m[i];
  tm->tm_mon = i;

  m = __ng_DAYS;
  i = t / SECSPERDAY;
  if (i > __ng_mon_DAYS[tm->tm_mon+(leap?12:0)] - 1)
    return ng_ERR_EINVAL;
  __ng_div_find(t, m, i);
  t -= m[i];
  tm->tm_mday = i + 1;
  
  m = leap ? __ng_year_DAYS + 13 : __ng_year_DAYS;
  tm->tm_yday = m[tm->tm_mon] + tm->tm_mday - 1;

  tm->tm_hour = t/3600;
  t -= __ng_HOURS[tm->tm_hour];

  tm->tm_min  = t/60;
  NG_ASSERT(t == __ng_MINUTES[tm->tm_min]);

  return ng_ERR_NONE;
}
  
/*
Syntax
  Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT

Directives
  <day-name>
    One of Mon, Tue, Wed, Thu, Fri, Sat, or Sun (case-sensitive).

  <day>
    2 digit day number, e.g., "04" or "23".

  <month>
    One of Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec (case sensitive).

  <year>
    4 digit year number, e.g., "1990" or "2016".

  <hour>
    2 digit hour number, e.g., "09" or "23".

  <minute>
    2 digit minute number, e.g., "04" or "59".

  <second>
    2 digit second number, e.g., "04" or "59".

GMT
  Greenwich Mean Time. HTTP dates are always expressed in GMT, never in local time.

Examples
  Response with a Date header
  The following HTTP message is a successful 200 status, with a Date header showing 
  the time the message originated. Other headers are omitted for brevity:

http
Copy to Clipboard
HTTP/1.1 200
Content-Type: text/html
Date: Tue, 29 Oct 2024 16:56:32 GMT

<html lang="en-US" ��

*/

static const char *year_name[] = {
	"1970","1971","1972","1973","1974","1975","1976","1977","1978","1979",
	"1980","1981","1982","1983","1984","1985","1986","1987","1988","1989",
	"1990","1991","1992","1993","1994","1995","1996","1997","1998","1999",
	"2000","2001","2002","2003","2004","2005","2006","2007","2008","2009",
	"2010","2011","2012","2013","2014","2015","2016","2017","2018","2019",
	"2020","2021","2022","2023","2024","2025","2026","2027","2028","2029",
	"2030","2031","2032","2033","2034","2035","2036","2037","2038","2039",
	"2040","2041","2042","2043","2044","2045","2046","2047","2048","2049",
	"2050","2051","2052","2053","2054","2055","2056","2057","2058","2059",
	"2060","2061","2062","2063","2064","2065","2066","2067","2068","2069",
	"2070","2071","2072","2073","2074","2075","2076","2077","2078","2079",
	"2080","2081","2082","2083","2084","2085","2086","2087","2088","2089",
	"2090","2091","2092","2093","2094","2095","2096","2097","2098","2099",
	"2100","2101","2102","2103","2104","2105","2106","2107","2108","2109",
	"2110","2111","2112","2113","2114","2115","2116","2117","2118","2119",
	"2120","2121","2122","2123","2124","2125","2126","2127","2128","2129",
	"2130","2131","2132","2133","2134","2135","2136","2137","2138","2139",
	"2140","2141","2142","2143","2144","2145","2146","2147","2148","2149",
	"2150","2151","2152","2153","2154","2155","2156","2157","2158","2159",
	"2160","2161","2162","2163","2164","2165","2166","2167","2168","2169",
	"2170","2171","2172","2173","2174","2175","2176","2177","2178","2179",
	"2180","2181","2182","2183","2184","2185","2186","2187","2188","2189",
	"2190","2191","2192","2193","2194","2195","2196","2197","2198","2199",
	"2200","2201","2202","2203","2204","2205","2206","2207","2208","2209",
	"2210","2211","2212","2213","2214","2215","2216","2217","2218","2219",
	"2220","2221","2222","2223","2224","2225"
};

static const char *month_name[] = 
  {
   "Jan ", "Feb ", "Mar ", "Apr ", "May ", "Jun ", "Jul ", 
   "Aug ", "Sep ", "Oct ", "Nov ", "Dec "
  };
static const char *week_name[] = 
  {
    "Sun,", "Mon,", "Tue,", "Wed,", "Thu,", "Fri,", "Sat,"
  };
static const char *min_sec_hour_day_name[] = 
  {
    " 00 "," 01 "," 02 "," 03 "," 04 "," 05 "," 06 "," 07 "," 08 ",
    " 09 "," 10 "," 11 "," 12 "," 13 "," 14 "," 15 "," 16 "," 17 ",
    " 18 "," 19 "," 20 "," 21 "," 22 "," 23 "," 24 "," 25 "," 26 ",
    " 27 "," 28 "," 29 "," 30 "," 31 "," 32 "," 33 "," 34 "," 35 ",
    " 36 "," 37 "," 38 "," 39 "," 40 "," 41 "," 42 "," 43 "," 44 ",
    " 45 "," 46 "," 47 "," 48 "," 49 "," 50 "," 51 "," 52 "," 53 ",
    " 54 "," 55 "," 56 "," 57 "," 58 "," 59 "," 60 "
  };

int __ng_http_date(char *buf, int len, int isHttp)
{
  char *p;
  ng_tmv_s tv;
  ng_rtc_time_s tm;
  long tz_offset;
  
  if (isHttp)
  {
    if (len < HTTPD_DATE_STRLEN_MIN+sizeof("Date: ")-1+2)
      return -1;
    tz_offset = 0;
  }
  else
  {
    if (len < HTTPD_DATE_STRLEN_MIN)
      return -1;
    tz_offset = ng_os_info.tz_offset;
  }

  p = buf;
  tv.tv_sec = ng_get_time();
  tv.tv_usec = 0;
  ng_unix2tm_time(&tm, &tv, tz_offset);
  
  if (isHttp)
  {
    *(uint64_t*)p = *(const uint64_t*)"Date:   ";
    p += 6;
  }
  *(uint32_t*)p = *(const uint32_t*)week_name[tm.tm_wday];
  p += 4;
  *(uint32_t*)p = *(const uint32_t*)min_sec_hour_day_name[tm.tm_mday];
  p += 4;
  *(uint32_t*)p = *(const uint32_t*)month_name[tm.tm_mon];
  p += 4;
  *(uint32_t*)p = *(const uint32_t*)year_name[tm.tm_year - 70];
  p[4] = ' ';
  p += 5;
  *(uint16_t*)p = *(const uint32_t*)(min_sec_hour_day_name[tm.tm_hour]+1);
  p[2] = ':';
  p += 3;
  *(uint16_t*)p = *(const uint32_t*)(min_sec_hour_day_name[tm.tm_min]+1);
  p[2] = ':';
  p += 3;
  *(uint16_t*)p = *(const uint32_t*)(min_sec_hour_day_name[tm.tm_sec]+1);
  p += 2;
  *(uint32_t*)p = *(const uint32_t*)" GMT";
  p += 4;
  if (isHttp)
  {
    *(uint16_t*)p = *(const uint16_t*)"\r\n";
    p += 2;
  }
  return p - buf;
}

int ng_http_date(httpd_buf_t *b)
{
  return __ng_http_date(b->buf, b->len, 1);
}

void ng_date_print(void)
{
  char date[HTTPD_DATE_STRLEN_MIN];
  httpd_buf_t b;
  BUF_SET(&b, date, sizeof(date));
  __ng_http_date(b.buf, b.len, 0);
  fprintf(stdout, "\nSystem is already running %"PRIu64" seconds\n", 
    ng_get_time() - (uint64_t)startime);
  fprintf(stdout, "Date   : %.*s\n\n", (int)b.len, b.cptr);
  
}
