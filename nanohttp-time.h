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
#ifndef __nanohttp_time_h
#define __nanohttp_time_h

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "nanohttp-defs.h"
#include "nanohttp-buffer.h"
#include "nanohttp-ring.h"

#ifdef __cplusplus
extern "C" {
#endif

extern ng_uint64_t os__rte_rdtsc_syscall(void);

#define HTTPD_DATE_STRLEN_MIN 29

#if (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(__i386__) || defined(__i386))
#if RTE_TOOLCHAIN_MSVC || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
static inline ng_uint64_t rte_rdtsc(void){return __rdtsc();}
static inline ng_uint64_t rte_rdtsc_precise(void){ ng_smp_mb(); return rte_rdtsc(); }
static inline ng_uint64_t rte_get_tsc_cycles(void) { return rte_rdtsc(); }

#elif defined(__aarch64__)
/** Read generic counter frequency */
static __rte_always_inline ng_uint64_t __rte_arm64_cntfrq(void){
	ng_uint64_t freq;

	asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));
	return freq;
}

/** Read generic counter */
static __rte_always_inline ng_uint64_t
__rte_arm64_cntvct(void)
{
	ng_uint64_t tsc;

	asm volatile("mrs %0, cntvct_el0" : "=r" (tsc));
	return tsc;
}

static __rte_always_inline ng_uint64_t
__rte_arm64_cntvct_precise(void)
{
	asm volatile("isb" : : : "memory");
	return __rte_arm64_cntvct();
}

/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
#ifndef RTE_ARM_EAL_RDTSC_USE_PMU
/**
 * This call is portable to any ARMv8 architecture, however, typically
 * cntvct_el0 runs at <= 100MHz and it may be imprecise for some tasks.
 */
static __rte_always_inline ng_uint64_t rte_rdtsc(void){	return __rte_arm64_cntvct();}
#else
/**
 * This is an alternative method to enable rte_rdtsc() with high resolution
 * PMU cycles counter.The cycle counter runs at cpu frequency and this scheme
 * uses ARMv8 PMU subsystem to get the cycle counter at userspace, However,
 * access to PMU cycle counter from user space is not enabled by default in
 * arm64 linux kernel.
 * It is possible to enable cycle counter at user space access by configuring
 * the PMU from the privileged mode (kernel space).
 *
 * asm volatile("msr pmintenset_el1, %0" : : "r" ((u64)(0 << 31)));
 * asm volatile("msr pmcntenset_el0, %0" :: "r" BIT(31));
 * asm volatile("msr pmuserenr_el0, %0" : : "r"(BIT(0) | BIT(2)));
 * asm volatile("mrs %0, pmcr_el0" : "=r" (val));
 * val |= (BIT(0) | BIT(2));
 * isb();
 * asm volatile("msr pmcr_el0, %0" : : "r" (val));
 */

/** Read PMU cycle counter */
static __rte_always_inline ng_uint64_t
__rte_arm64_pmccntr(void)
{
	ng_uint64_t tsc;

	asm volatile("mrs %0, pmccntr_el0" : "=r"(tsc));
	return tsc;
}

static __rte_always_inline ng_uint64_t
rte_rdtsc(void)
{
	return __rte_arm64_pmccntr();
}
#endif
static __rte_always_inline ng_uint64_t
rte_rdtsc_precise(void)
{
	asm volatile("isb" : : : "memory");
	return rte_rdtsc();
}

static __rte_always_inline ng_uint64_t
rte_get_tsc_cycles(void)
{
	return rte_rdtsc();
}
#elif (defined(__arm__) || defined(__arm) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7__))
/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
#ifndef RTE_ARM_EAL_RDTSC_USE_PMU

/**
 * This call is easily portable to any architecture, however,
 * it may require a system call and imprecise for some tasks.
 */
static inline ng_uint64_t
__rte_rdtsc_syscall(void)
{
	return os__rte_rdtsc_syscall();
}
#define rte_rdtsc __rte_rdtsc_syscall
#else
/**
 * This function requires to configure the PMCCNTR and enable
 * userspace access to it:
 *
 *      asm volatile("mcr p15, 0, %0, c9, c14, 0" : : "r"(1));
 *      asm volatile("mcr p15, 0, %0, c9, c12, 0" : : "r"(29));
 *      asm volatile("mcr p15, 0, %0, c9, c12, 1" : : "r"(0x8000000f));
 *
 * which is possible only from the privileged mode (kernel space).
 */
static inline ng_uint64_t
__rte_rdtsc_pmccntr(void)
{
	unsigned tsc;
	ng_uint64_t final_tsc;

	/* Read PMCCNTR */
	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(tsc));
	/* 1 tick = 64 clocks */
	final_tsc = ((ng_uint64_t)tsc) << 6;

	return (ng_uint64_t)final_tsc;
}
#define rte_rdtsc __rte_rdtsc_pmccntr

#endif /* RTE_ARM_EAL_RDTSC_USE_PMU */
static inline ng_uint64_t
rte_rdtsc_precise(void)
{
	rte_mb();
	return rte_rdtsc();
}

static inline ng_uint64_t
rte_get_tsc_cycles(void) { return rte_rdtsc(); }
#elif defined(__riscv)
#ifndef RTE_RISCV_RDTSC_USE_HPM
#define RTE_RISCV_RDTSC_USE_HPM 0
#endif
/** Read wall time counter */
static __rte_always_inline ng_uint64_t
__rte_riscv_rdtime(void)
{
	ng_uint64_t tsc;
	asm volatile("csrr %0, time" : "=r" (tsc) : : "memory");
	return tsc;
}

/** Read wall time counter ensuring no re-ordering */
static __rte_always_inline ng_uint64_t
__rte_riscv_rdtime_precise(void)
{
	asm volatile("fence" : : : "memory");
	return __rte_riscv_rdtime();
}

/** Read hart cycle counter */
static __rte_always_inline ng_uint64_t
__rte_riscv_rdcycle(void)
{
	ng_uint64_t tsc;
	asm volatile("csrr %0, cycle" : "=r" (tsc) : : "memory");
	return tsc;
}

/** Read hart cycle counter ensuring no re-ordering */
static __rte_always_inline ng_uint64_t
__rte_riscv_rdcycle_precise(void)
{
	asm volatile("fence" : : : "memory");
	return __rte_riscv_rdcycle();
}

/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
static __rte_always_inline ng_uint64_t
rte_rdtsc(void)
{
	/**
	 * By default TIME userspace counter is used. It is stable and shared
	 * across cores. Although it's frequency may not be enough for all
	 * applications.
	 */
	if (!RTE_RISCV_RDTSC_USE_HPM)
		return __rte_riscv_rdtime();
	/**
	 * Alternatively HPM's CYCLE counter may be used. However this counter
	 * is not guaranteed by ISA to either be stable frequency or always
	 * enabled for userspace access (it may trap to kernel or firmware,
	 * though as of Linux kernel 5.13 it doesn't).
	 * It is also highly probable that values of this counter are not
	 * synchronized across cores. Therefore if it is to be used as a timer,
	 * it can only be used in the scope of a single core.
	 */
	return __rte_riscv_rdcycle();
}

static inline ng_uint64_t
rte_rdtsc_precise(void)
{
	if (!RTE_RISCV_RDTSC_USE_HPM)
		return __rte_riscv_rdtime_precise();
	return __rte_riscv_rdcycle_precise();
}

static __rte_always_inline ng_uint64_t
rte_get_tsc_cycles(void)
{
	return rte_rdtsc();
}
#elif (defined(__powerpc__) || defined(__powerpc) || defined(__ppc__))
/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
static inline ng_uint64_t
rte_rdtsc(void)
{
#ifdef __GLIBC__
	return __ppc_get_timebase();
#else
	return __builtin_ppc_get_timebase();
#endif
}

static inline ng_uint64_t rte_rdtsc_precise(void){	rte_mb();	return rte_rdtsc();}
static inline ng_uint64_t rte_get_tsc_cycles(void) { return rte_rdtsc(); }
#elif defined(__loongarch__)
/**
 * Read the time base register.
 *
 * @return
 *   The time base for this lcore.
 */
static inline ng_uint64_t
rte_rdtsc(void)
{
	ng_uint64_t count;

	__asm__ __volatile__ (
		"rdtime.d %[cycles], $zero\n"
		: [cycles] "=r" (count)
		::
		);
	return count;
}

static inline ng_uint64_t rte_rdtsc_precise(void) { rte_mb(); return rte_rdtsc(); }
static inline ng_uint64_t rte_get_tsc_cycles(void) { return rte_rdtsc(); }
#else
static inline ng_uint64_t rte_rdtsc(void) { return os__rte_rdtsc_syscall(); }
static inline ng_uint64_t rte_rdtsc_precise(void) { rte_mb(); return rte_rdtsc(); }
static inline ng_uint64_t rte_get_tsc_cycles(void) { return rte_rdtsc(); }
#endif

/**
 * Get the number of cycles since boot from the default timer.
 *
 * @return
 *   The number of cycles
 */
static inline ng_uint64_t
rte_get_timer_cycles(void)
{
	return rte_get_tsc_cycles();
}

void ng_update_time(void);
ng_uint64_t ng_get_time(void);
ng_uint64_t rte_get_tsc_hz(void);
ng_uint64_t rte_get_timer_hz(void);
void rte_delay_us_sleep(unsigned int us);
ng_uint64_t get_tsc_freq(void);
    
/*
Cycles To Ms Formula
The following formula is used to calculate the Cycles to Milliseconds (ms).

      MS=(C/F)*1000
Variables:
  MS     is the time in milliseconds (ms)
  C      is the number of cycles
  F      is the frequency of the CPU in Hertz (Hz)
  
  To calculate the Cycles to Milliseconds, divide the number of cycles by 
  the frequency of the CPU. Then, multiply the result by 1000 to convert 
  the time from seconds to milliseconds. The result is the time taken for 
  a specific number of cycles in a processor, measured in milliseconds.
*/
static inline ng_uint64_t ng_cycles2sec(ng_uint64_t cycles, ng_uint64_t freq)
{
  if (!freq) freq = rte_get_timer_hz();
  return cycles / freq;
}

typedef struct __ng_tv ng_tmv_s;
struct __ng_tv {
  ng_uint64_t tv_sec;
  ng_uint64_t tv_usec;
  int_t    gmtoff;
};

#define NG_TMV_INIT { .tv_sec = 0, .tv_usec = 0, .gmtoff = 0}

/*
 * Similar to the struct tm in userspace <time.h>, but it needs to be here so
 * that the kernel source is self contained.
 */
struct ng_rtc_time {
  /*
   * the number of seconds after the minute, normally in the range
   * 
   */
  ng_uint32_t tm_ms:10;        /* Millisecond in second (0-999) */
  ng_uint32_t tm_sec:6;        /* Seconds          [0, 60]                0 to 59, but can be up to 60 to allow for leap seconds */
  ng_uint32_t tm_min:6;        /* Minutes          [0, 59]                the number of minutes after the hour, in the range [0, 60] */
  ng_uint32_t tm_hour:5;       /* Hour             [0, 23]                the number of hours past midnight, in the range 0 to 23 */
  ng_uint32_t tm_mday:5;       /* Day of the month [1, 31]                the day of the month, in the range 1 to 31 */
  ng_uint32_t tm_mon:4;        /* Month            [0, 11]  (January = 0) the number of months since January, in the range 0 to 11 */
  ng_uint32_t tm_year:15;      /* the number of years since 1900 */
  ng_uint32_t tm_wday:3;       /* Day of the week  [0, 6]   (Sunday = 0)  the number of days since Sunday, in the range 0 to 6 */
  ng_uint32_t tm_yday:9;       /* Day of the year  [0, 365] (Jan/01 = 0)  the number of days since January 1, in the range 0 to 365 */
  ng_uint32_t tm_isdst:1;      /* Daylight savings flag */

  ng_int32_t tm_gmtoff;        /* Seconds East of UTC */
  const char *tm_zone;      /* Timezone abbreviation */
};
typedef struct ng_rtc_time ng_rtc_time_s;

/* Operations on timevals. */
#define	ng_timerclear(tvp)		(tvp)->tv_sec = (tvp)->tv_usec = 0
#define	ng_timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	ng_timerisvalid(tvp)						\
	((tvp)->tv_usec >= 0 && (tvp)->tv_usec < 1000000)
#define	ng_timercmp(tvp, uvp, cmp)						\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	    ((tvp)->tv_usec cmp (uvp)->tv_usec) :			\
	    ((tvp)->tv_sec cmp (uvp)->tv_sec))
#define	ng_timeradd(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;	\
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	ng_timersub(tvp, uvp, vvp)						\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)

/* Operations on timespecs. */
#define	ng_timespecclear(tsp)		(tsp)->tv_sec = (tsp)->tv_nsec = 0
#define	ng_timespecisset(tsp)		((tsp)->tv_sec || (tsp)->tv_nsec)
#define	ng_timespecisvalid(tsp)						\
	((tsp)->tv_nsec >= 0 && (tsp)->tv_nsec < 1000000000L)
#define	ng_timespeccmp(tsp, usp, cmp)					\
	(((tsp)->tv_sec == (usp)->tv_sec) ?				\
	    ((tsp)->tv_nsec cmp (usp)->tv_nsec) :			\
	    ((tsp)->tv_sec cmp (usp)->tv_sec))
#define	ng_timespecadd(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec >= 1000000000L) {			\
			(vsp)->tv_sec++;				\
			(vsp)->tv_nsec -= 1000000000L;			\
		}							\
	} while (0)
#define	ng_timespecsub(tsp, usp, vsp)					\
	do {								\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {				\
			(vsp)->tv_sec--;				\
			(vsp)->tv_nsec += 1000000000L;			\
		}							\
	} while (0)

#define TYPE_SIGNED(type) ((type) -1 < 0)

/* Return -X as a double.  Using this avoids casting to 'double'.  */
static __ng_inline__ double
dminus(double x)
{
  return -x;
}

static __ng_inline__ double
__ng_difftime(ng_time_t time1, ng_time_t time0)
{
  /*
  ** If double is large enough, simply convert and subtract
  ** (assuming that the larger type has more precision).
  */
  if (sizeof(ng_time_t) < sizeof(double)) {
    double t1 = time1, t0 = time0;
    return t1 - t0;
  }

  /*
  ** The difference of two unsigned values can't overflow
  ** if the minuend is greater than or equal to the subtrahend.
  */
  if (!TYPE_SIGNED(ng_time_t))
    return time0 <= time1 ? time1 - time0 : dminus(time0 - time1);

  /* Use uintmax_t if wide enough.  */
  if (sizeof(ng_time_t) <= sizeof(ng_uintmax_t)) {
    ng_uintmax_t t1 = time1, t0 = time0;
    return time0 <= time1 ? t1 - t0 : dminus(t0 - t1);
  }

  /*
  ** Handle cases where both time1 and time0 have the same sign
  ** (meaning that their difference cannot overflow).
  */
  if ((time1 < 0) == (time0 < 0))
    return time1 - time0;

  /*
  ** The values have opposite signs and uintmax_t is too narrow.
  ** This suffers from double rounding; attempt to lessen that
  ** by using long double temporaries.
  */
  {
    long double t1 = time1, t0 = time0;
    return t1 - t0;
  }
}

static __ng_inline__ ng_time_t ng_difftime (ng_time_t time1, ng_time_t time0)
{
  return (ng_time_t)__ng_difftime(time1, time0);
}

int ng_unix2tm_time(ng_rtc_time_s *tm, ng_tmv_s *tv, int tz_offset);
int ng_http_date(ng_buffer_s *b);
int rte_eal_timer_init(void);
void ng_date_print(void);
int __ng_http_date(char *buf, int len, int isHttp, const char *tz);
int raw_ng_http_date(ng_time_t t, char *buf, int len, int isHttp, const char *tz);
int ng_http_date_s(char *buf, int len);
int raw_ng_http_tm(ng_rtc_time_s *tm, char *buf, int len, int isHttp);
int raw_ng_tm(ng_rtc_time_s *tm, char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif
