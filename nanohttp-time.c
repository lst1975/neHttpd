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
#include "nanohttp-time.h"
#include "nanohttp-system.h"
#include "nanohttp-error.h"
#include "nanohttp-utils.h"
#include "nanohttp-ctype.h"
#include "nanohttp-freq.h"

/* Pointer to user delay function */
void (*rte_delay_us)(unsigned int) = NULL;

/* The frequency of the RDTSC timer resolution */
static ng_uint64_t eal_tsc_resolution_hz;

#define MS_PER_S 1000
#define US_PER_S 1000000
#define NS_PER_S 1000000000

static ng_uint64_t
estimate_tsc_freq(void)
{
#define CYC_PER_10MHZ 1E7
	log_warn("WARNING: TSC frequency estimated roughly"
		" - clock timings may be less accurate.");
	/* assume that the rte_delay_us_sleep() will sleep for 1 second */
	ng_uint64_t start = rte_rdtsc();
	rte_delay_us_sleep(US_PER_S);
	/* Round up to 10Mhz. 1E7 ~ 10Mhz */
	return RTE_ALIGN_MUL_NEAR(rte_rdtsc() - start, CYC_PER_10MHZ);
}

void
set_tsc_freq(void)
{
	ng_uint64_t freq;

	freq = get_tsc_freq_arch();
	if (!freq)
		freq = get_tsc_freq();
	if (!freq)
		freq = estimate_tsc_freq();

  ng_print_cpufreq("TSC frequency", freq / 1000000.0);
	eal_tsc_resolution_hz = freq;
	ng_os_info.tsc_hz = freq;
}

#if RTE_TOOLCHAIN_MSVC || defined(__MINGW64__) || defined(__MINGW32__) || defined(__CYGWIN__) 
#include <windows.h>
#define US_PER_SEC 1E6
#define CYC_PER_10MHZ 1E7

void
rte_delay_us_sleep(unsigned int us)
{
  HANDLE timer;
  LARGE_INTEGER due_time;

  /* create waitable timer */
  timer = CreateWaitableTimer(NULL, TRUE, NULL);
  if (!timer) 
  {
  	log_error("CreateWaitableTimer() failed. %m.", ng_errno);
  	ng_set_errno(ERROR_NOT_ENOUGH_MEMORY);
  	goto clean0;
  }

  /*
   * due_time's uom is 100 ns, multiply by 10 to convert to microseconds
   * set us microseconds time for timer
   */
  due_time.QuadPart = -((ng_int64_t)us * 10);
  if (!SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE)) 
  {
    log_error("SetWaitableTimer() failed. %m.", ng_errno);
    ng_set_errno(WSAEINVAL);
    goto clean1;
  }
  /* start wait for timer for us microseconds */
  if (WaitForSingleObject(timer, INFINITE) == WAIT_FAILED) 
  {
  	log_error("WaitForSingleObject() failed. %m.", ng_errno);
  	ng_set_errno(WSAEINVAL);
  	goto clean1;
  }

clean1:
	CloseHandle(timer);
clean0:
  return;
}

ng_uint64_t
get_tsc_freq(void)
{
	LARGE_INTEGER t_start, t_end, elapsed_us;
	LARGE_INTEGER frequency;
	ng_uint64_t tsc_hz;
	ng_uint64_t end, start;

	QueryPerformanceFrequency(&frequency);

	QueryPerformanceCounter(&t_start);
	start = rte_get_tsc_cycles();

	rte_delay_us_sleep(US_PER_SEC / 10); /* 1/10 second */

	if (ng_errno != 0)
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
	tsc_hz = (ng_uint64_t)((end - start)/secs);

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
ng_uint64_t
get_tsc_freq(void)
{
	ng_size_t sz;
	int tmp;
	ng_uint64_t tsc_hz;

	sz = sizeof(tmp);
	tmp = 0;

	if (sysctlbyname("kern.timecounter.smp_tsc", &tmp, &sz, NULL, 0))
		log_warn("get_tsc_freq: %m.", ng_errno);
	else if (tmp != 1)
		log_warn("TSC is not safe to use in SMP mode");

	tmp = 0;

	if (sysctlbyname("kern.timecounter.invariant_tsc", &tmp, &sz, NULL, 0))
		log_warn("get_tsc_freq: %m.", ng_errno);
	else if (tmp != 1)
		log_warn("TSC is not invariant.");

	sz = sizeof(tsc_hz);
	if (sysctlbyname("machdep.tsc_freq", &tsc_hz, &sz, NULL, 0)) {
		log_warn("get_tsc_freq: %m.", ng_errno);
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

ng_uint64_t
get_tsc_freq(void)
{
#ifdef CLOCK_MONOTONIC_RAW
#define NS_PER_SEC 1E9
#define CYC_PER_10MHZ 1E7

	struct timespec sleeptime = {.tv_nsec = NS_PER_SEC / 10 }; /* 1/10 second */

	struct timespec t_start, t_end;
	ng_uint64_t tsc_hz;

	if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_start) == 0) {
		ng_uint64_t ns, end, start = rte_rdtsc();
		nanosleep(&sleeptime,NULL);
		clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
		end = rte_rdtsc();
		ns = ((t_end.tv_sec - t_start.tv_sec) * NS_PER_SEC);
		ns += (t_end.tv_nsec - t_start.tv_nsec);

		double secs = (double)ns/NS_PER_SEC;
		tsc_hz = (ng_uint64_t)((end - start)/secs);
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

ng_uint64_t
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
ng_uint64_t
rte_get_timer_hz(void)
{
	ng_uint64_t freq = rte_get_tsc_hz();
  if (!freq) freq = ng_os_info.freq;
  return freq;
}

void
rte_delay_us_block(unsigned int us)
{
	const ng_uint64_t start = rte_get_timer_cycles();
	const ng_uint64_t ticks = (ng_uint64_t)us * rte_get_timer_hz() / 1E6;
	while ((rte_get_timer_cycles() - start) < ticks)
		rte_pause();
}

static ng_uint64_t startime;  
static ng_uint64_t starcycles;  

// Function to round a double value to the nearest ng_int64_t value
static inline ng_int64_t round_to_int64(double num) {
  // Check if the number is within the range of ng_int64_t
  double max_int64 = (double)NG_INT64_MAX;
  double min_int64 = (double)NG_INT64_MIN;

  if (num > max_int64) {
    log_warn("Number exceeds ng_int64_t maximum range.");
    return NG_INT64_MAX;
  } else if (num < min_int64) {
    log_warn("Number exceeds ng_int64_t minimum range.");
    return NG_INT64_MIN;
  }

  // Round the number to the nearest integer
  ng_int64_t rounded = (ng_int64_t)(num + (num >= 0 ? 0.5 : -0.5));

  return rounded;
}

// Function to round a double value to the nearest ng_uint64_t value
static inline ng_uint64_t round_to_uint64(double num) {
  // Check if the number is negative
  if (num < 0) {
    log_warn("Cannot round a negative number to ng_uint64_t.");
    return 0;
  }

  // Check if the number is greater than the maximum value of ng_uint64_t
  double max_uint64 = (double)NG_UINT64_MAX;
  if (num > max_uint64) {
    log_warn("Number exceeds ng_uint64_t range.");
    return NG_UINT64_MAX;
  }

  // Round the number to the nearest integer
  ng_uint64_t rounded = (ng_uint64_t)(num + 0.5);

  return rounded;
}

ng_uint64_t ng_get_time(void)
{
  ng_uint64_t endcycles, elapsed, result;
  endcycles = rte_get_timer_cycles();
  elapsed = round_to_uint64(ng_cycles2sec(ng_difftime(endcycles, starcycles), 0));
  result = startime+elapsed;
  if (endcycles <= starcycles)
  {
    startime   = result;
    starcycles = endcycles;
  }
  return result;
}

void ng_update_time(void)
{
  ng_tmv_s tv;
  ng_gettimeofday(&tv);
  starcycles = rte_get_timer_cycles();
  startime   = tv.tv_sec+tv.tv_usec/1000000;
}

#define __ng_YEARS_N  251
#define __ng_MONTHS_N 11
#define __ng_DAYS_N   31
#define __ng_HOURS_N  23

static const ng_uint64_t __ng_YEARS[256] = {
  UINT64_C(         0),
  UINT64_C(  31536000), UINT64_C(  63072000), UINT64_C(  94694400), 
  UINT64_C( 126230400), UINT64_C( 157766400), UINT64_C( 189302400),
  UINT64_C( 220924800), UINT64_C( 252460800), UINT64_C( 283996800), 
  UINT64_C( 315532800), UINT64_C( 347155200), UINT64_C( 378691200),
  UINT64_C( 410227200), UINT64_C( 441763200), UINT64_C( 473385600), 
  UINT64_C( 504921600), UINT64_C( 536457600), UINT64_C( 567993600),
  UINT64_C( 599616000), UINT64_C( 631152000), UINT64_C( 662688000), 
  UINT64_C( 694224000), UINT64_C( 725846400), UINT64_C( 757382400),
  UINT64_C( 788918400), UINT64_C( 820454400), UINT64_C( 852076800), 
  UINT64_C( 883612800), UINT64_C( 915148800), UINT64_C( 946684800),
  UINT64_C( 978307200), UINT64_C(1009843200), UINT64_C(1041379200), 
  UINT64_C(1072915200), UINT64_C(1104537600), UINT64_C(1136073600),
  UINT64_C(1167609600), UINT64_C(1199145600), UINT64_C(1230768000), 
  UINT64_C(1262304000), UINT64_C(1293840000), UINT64_C(1325376000),
  UINT64_C(1356998400), UINT64_C(1388534400), UINT64_C(1420070400), 
  UINT64_C(1451606400), UINT64_C(1483228800), UINT64_C(1514764800),
  UINT64_C(1546300800), UINT64_C(1577836800), UINT64_C(1609459200), 
  UINT64_C(1640995200), UINT64_C(1672531200), UINT64_C(1704067200),
  UINT64_C(1735689600), UINT64_C(1767225600), UINT64_C(1798761600), 
  UINT64_C(1830297600), UINT64_C(1861920000), UINT64_C(1893456000),
  UINT64_C(1924992000), UINT64_C(1956528000), UINT64_C(1988150400), 
  UINT64_C(2019686400), UINT64_C(2051222400), UINT64_C(2082758400),
  UINT64_C(2114380800), UINT64_C(2145916800), UINT64_C(2177452800), 
  UINT64_C(2208988800), UINT64_C(2240611200), UINT64_C(2272147200),
  UINT64_C(2303683200), UINT64_C(2335219200), UINT64_C(2366841600), 
  UINT64_C(2398377600), UINT64_C(2429913600), UINT64_C(2461449600),
  UINT64_C(2493072000), UINT64_C(2524608000), UINT64_C(2556144000), 
  UINT64_C(2587680000), UINT64_C(2619302400), UINT64_C(2650838400),
  UINT64_C(2682374400), UINT64_C(2713910400), UINT64_C(2745532800), 
  UINT64_C(2777068800), UINT64_C(2808604800), UINT64_C(2840140800),
  UINT64_C(2871763200), UINT64_C(2903299200), UINT64_C(2934835200), 
  UINT64_C(2966371200), UINT64_C(2997993600), UINT64_C(3029529600),
  UINT64_C(3061065600), UINT64_C(3092601600), UINT64_C(3124224000), 
  UINT64_C(3155760000), UINT64_C(3187296000), UINT64_C(3218832000),
  UINT64_C(3250454400), UINT64_C(3281990400), UINT64_C(3313526400), 
  UINT64_C(3345062400), UINT64_C(3376684800), UINT64_C(3408220800),
  UINT64_C(3439756800), UINT64_C(3471292800), UINT64_C(3502915200), 
  UINT64_C(3534451200), UINT64_C(3565987200), UINT64_C(3597523200),
  UINT64_C(3629145600), UINT64_C(3660681600), UINT64_C(3692217600), 
  UINT64_C(3723753600), UINT64_C(3755376000), UINT64_C(3786912000),
  UINT64_C(3818448000), UINT64_C(3849984000), UINT64_C(3881606400), 
  UINT64_C(3913142400), UINT64_C(3944678400), UINT64_C(3976214400),
  UINT64_C(4007836800), UINT64_C(4039372800), UINT64_C(4070908800), 
  UINT64_C(4102444800), UINT64_C(4133980800), UINT64_C(4165516800),
  UINT64_C(4197052800), UINT64_C(4228588800), UINT64_C(4260211200), 
  UINT64_C(4291747200), UINT64_C(4323283200), UINT64_C(4354819200),
  UINT64_C(4386441600), UINT64_C(4417977600), UINT64_C(4449513600), 
  UINT64_C(4481049600), UINT64_C(4512672000), UINT64_C(4544208000),
  UINT64_C(4575744000), UINT64_C(4607280000), UINT64_C(4638902400), 
  UINT64_C(4670438400), UINT64_C(4701974400), UINT64_C(4733510400),
  UINT64_C(4765132800), UINT64_C(4796668800), UINT64_C(4828204800), 
  UINT64_C(4859740800), UINT64_C(4891363200), UINT64_C(4922899200),
  UINT64_C(4954435200), UINT64_C(4985971200), UINT64_C(5017593600), 
  UINT64_C(5049129600), UINT64_C(5080665600), UINT64_C(5112201600),
  UINT64_C(5143824000), UINT64_C(5175360000), UINT64_C(5206896000), 
  UINT64_C(5238432000), UINT64_C(5270054400), UINT64_C(5301590400),
  UINT64_C(5333126400), UINT64_C(5364662400), UINT64_C(5396284800), 
  UINT64_C(5427820800), UINT64_C(5459356800), UINT64_C(5490892800),
  UINT64_C(5522515200), UINT64_C(5554051200), UINT64_C(5585587200), 
  UINT64_C(5617123200), UINT64_C(5648745600), UINT64_C(5680281600),
  UINT64_C(5711817600), UINT64_C(5743353600), UINT64_C(5774976000), 
  UINT64_C(5806512000), UINT64_C(5838048000), UINT64_C(5869584000),
  UINT64_C(5901206400), UINT64_C(5932742400), UINT64_C(5964278400), 
  UINT64_C(5995814400), UINT64_C(6027436800), UINT64_C(6058972800),
  UINT64_C(6090508800), UINT64_C(6122044800), UINT64_C(6153667200), 
  UINT64_C(6185203200), UINT64_C(6216739200), UINT64_C(6248275200),
  UINT64_C(6279897600), UINT64_C(6311433600), UINT64_C(6342969600), 
  UINT64_C(6374505600), UINT64_C(6406128000), UINT64_C(6437664000),
  UINT64_C(6469200000), UINT64_C(6500736000), UINT64_C(6532358400), 
  UINT64_C(6563894400), UINT64_C(6595430400), UINT64_C(6626966400),
  UINT64_C(6658588800), UINT64_C(6690124800), UINT64_C(6721660800), 
  UINT64_C(6753196800), UINT64_C(6784819200), UINT64_C(6816355200),
  UINT64_C(6847891200), UINT64_C(6879427200), UINT64_C(6911049600), 
  UINT64_C(6942585600), UINT64_C(6974121600), UINT64_C(7005657600),
  UINT64_C(7037280000), UINT64_C(7068816000), UINT64_C(7100352000), 
  UINT64_C(7131888000), UINT64_C(7163510400), UINT64_C(7195046400),
  UINT64_C(7226582400), UINT64_C(7258118400), UINT64_C(7289654400), 
  UINT64_C(7321190400), UINT64_C(7352726400), UINT64_C(7384262400),
  UINT64_C(7415884800), UINT64_C(7447420800), UINT64_C(7478956800), 
  UINT64_C(7510492800), UINT64_C(7542115200), UINT64_C(7573651200),
  UINT64_C(7605187200), UINT64_C(7636723200), UINT64_C(7668345600), 
  UINT64_C(7699881600), UINT64_C(7731417600), UINT64_C(7762953600),
  UINT64_C(7794576000), UINT64_C(7826112000), UINT64_C(7857648000), 
  UINT64_C(7889184000), UINT64_C(7920806400), UINT64_C(7952342400)};

static const ng_uint32_t __ng_MONTHS[16] = {
  UINT32_C(       0), UINT32_C( 2678400), UINT32_C( 5097600),  
  UINT32_C( 7776000), UINT32_C(10368000), UINT32_C(13046400), 
  UINT32_C(15638400), UINT32_C(18316800), UINT32_C(20995200), 
  UINT32_C(23587200), UINT32_C(26265600), UINT32_C(28857600), 
  UINT32_C(31536000)};
  
static const ng_uint32_t __ng_MONTHS_leap[16] = {
  UINT32_C(       0), UINT32_C( 2678400), UINT32_C( 5184000), 
  UINT32_C( 7862400), UINT32_C(10454400), UINT32_C(13132800), 
  UINT32_C(15724800), UINT32_C(18403200), UINT32_C(21081600), 
  UINT32_C(23673600), UINT32_C(26352000), UINT32_C(28944000), 
  UINT32_C(31622400)};
  
static const ng_uint32_t __ng_DAYS[32] = {
  UINT32_C(      0), UINT32_C(  86400), UINT32_C( 172800), 
  UINT32_C( 259200), UINT32_C( 345600), UINT32_C( 432000), 
  UINT32_C( 518400), UINT32_C( 604800), UINT32_C( 691200), 
  UINT32_C( 777600), UINT32_C( 864000), UINT32_C( 950400), 
  UINT32_C(1036800), UINT32_C(1123200), UINT32_C(1209600), 
  UINT32_C(1296000), UINT32_C(1382400), UINT32_C(1468800), 
  UINT32_C(1555200), UINT32_C(1641600), UINT32_C(1728000), 
  UINT32_C(1814400), UINT32_C(1900800), UINT32_C(1987200), 
  UINT32_C(2073600), UINT32_C(2160000), UINT32_C(2246400), 
  UINT32_C(2332800), UINT32_C(2419200), UINT32_C(2505600), 
  UINT32_C(2592000), UINT32_C(2678400)};
  
static const ng_uint32_t __ng_HOURS[32] = {
  UINT32_C(    0), UINT32_C( 3600), UINT32_C( 7200), 
  UINT32_C(10800), UINT32_C(14400), UINT32_C(18000), 
  UINT32_C(21600), UINT32_C(25200), UINT32_C(28800), 
  UINT32_C(32400), UINT32_C(36000), UINT32_C(39600), 
  UINT32_C(43200), UINT32_C(46800), UINT32_C(50400), 
  UINT32_C(54000), UINT32_C(57600), UINT32_C(61200), 
  UINT32_C(64800), UINT32_C(68400), UINT32_C(72000), 
  UINT32_C(75600), UINT32_C(79200), UINT32_C(82800), 
  UINT32_C(86400)};
  
static const ng_uint32_t __ng_MINUTES[64] = {
  UINT32_C(   0), UINT32_C(  60), UINT32_C( 120), UINT32_C( 180), 
  UINT32_C( 240), UINT32_C( 300), UINT32_C( 360), UINT32_C( 420), 
  UINT32_C( 480), UINT32_C( 540), UINT32_C( 600), UINT32_C( 660), 
  UINT32_C( 720), UINT32_C( 780), UINT32_C( 840), UINT32_C( 900), 
  UINT32_C( 960), UINT32_C(1020), UINT32_C(1080), UINT32_C(1140), 
  UINT32_C(1200), UINT32_C(1260), UINT32_C(1320), UINT32_C(1380), 
  UINT32_C(1440), UINT32_C(1500), UINT32_C(1560), UINT32_C(1620), 
  UINT32_C(1680), UINT32_C(1740), UINT32_C(1800), UINT32_C(1860), 
  UINT32_C(1920), UINT32_C(1980), UINT32_C(2040), UINT32_C(2100), 
  UINT32_C(2160), UINT32_C(2220), UINT32_C(2280), UINT32_C(2340), 
  UINT32_C(2400), UINT32_C(2460), UINT32_C(2520), UINT32_C(2580), 
  UINT32_C(2640), UINT32_C(2700), UINT32_C(2760), UINT32_C(2820), 
  UINT32_C(2880), UINT32_C(2940), UINT32_C(3000), UINT32_C(3060), 
  UINT32_C(3120), UINT32_C(3180), UINT32_C(3240), UINT32_C(3300), 
  UINT32_C(3360), UINT32_C(3420), UINT32_C(3480), UINT32_C(3540), 
  UINT32_C(3600)};
  
static const ng_uint32_t __ng_mon_DAYS[32] = {
  UINT32_C(31), UINT32_C(28), UINT32_C(31), UINT32_C(30), 
  UINT32_C(31), UINT32_C(30), UINT32_C(31), UINT32_C(31), 
  UINT32_C(30), UINT32_C(31), UINT32_C(30), UINT32_C(31),
  UINT32_C(31), UINT32_C(29), UINT32_C(31), UINT32_C(30), 
  UINT32_C(31), UINT32_C(30), UINT32_C(31), UINT32_C(31), 
  UINT32_C(30), UINT32_C(31), UINT32_C(30), UINT32_C(31)};
  
static const ng_uint32_t __ng_year_DAYS[32] = {
  UINT32_C(  0), UINT32_C( 31), UINT32_C( 59), UINT32_C( 90), 
  UINT32_C(120), UINT32_C(151), UINT32_C(181), UINT32_C(212), 
  UINT32_C(243), UINT32_C(273), UINT32_C(304), UINT32_C(334), 
  UINT32_C(365),
  UINT32_C(  0), UINT32_C( 31), UINT32_C( 60), UINT32_C( 91), 
  UINT32_C(121), UINT32_C(152), UINT32_C(182), UINT32_C(213), 
  UINT32_C(244), UINT32_C(274), UINT32_C(305), UINT32_C(335), 
  UINT32_C(366)};

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
        + tm->tm_sec
        + tm->tm_gmtoff;
              
  tv->tv_usec = tm->tm_ms * MS_PER_S;
  return ng_ERR_NONE;              
}

#define SECSPERYEAR   UINT32_C(31536000)  /* 3600*24*365 */
#define SECSPERMONTH  UINT32_C(2419200)   /* 3600*24*28 */
#define SECSPERDAY    UINT32_C(86400)     /* 3600*24 */
#define JAN11900DOW   UINT32_C(4)         /* Jan 1, 1970 was a Wensday */
#define ZONE_OFFSET   UINT32_C(28800)

#define __ng_div_find(t, m, i) do { \
  while (m[i+1] < t) \
    i++; \
}while(0)

ng_result_t
ng_unix2tm_time(ng_rtc_time_s *tm, ng_tmv_s *tv, int tz_offset) 
{
  int i, leap = 0;
  const ng_uint32_t *m;
  ng_time_t day_in_epoch;
  ng_time_t t = tv->tv_sec;
  
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

/* "Date: %a, %d %b %Y %H:%M:%S GMT\r\n" 
 *  Date: Wed, 08 Jan 2025 05:42:00 GMT
 */
int 
ng_http_date2gm(const char *buf, int len, ng_rtc_time_s *tm)
{
  const char *p = buf;
  
  if (len != 29)
  {
    return -1;
  }
  
  if (p[3] != ',' || p[4] != ' ' 
    || *(ng_uint32_t *)&p[25] != *(const ng_uint32_t *)" GMT")
  {
    return -1;
  }
  
  switch (p[0])
  {
    case 'S':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"un")
        tm->tm_wday = 0;
      else if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"at")
        tm->tm_wday = 6;
      else
        return -1;
      break;
      
    case 'T':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ue")
        tm->tm_wday = 2;
      else if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"hu")
        tm->tm_wday = 4;
      else
        return -1;
      break;
      
    case 'M':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"on")
        tm->tm_wday = 1;
      else if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ay")
        tm->tm_wday = 4;
      else
        return -1;
      break;
      
    case 'W':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ed")
        tm->tm_wday = 3;
      else
        return -1;
      break;
      
    case 'F':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ed")
        tm->tm_wday = 5;
      else
        return -1;
      break;
      
    default:
      return -1;
  }
  
  p += 5;
  if (p[2] != ' ' || !__ng_isdigit(p[0]) || !__ng_isdigit(p[1]))
  {
    return -1;
  }
  
  tm->tm_mday = __ng_hex2int(p[0])*10 +  __ng_hex2int(p[1]);
  if (tm->tm_mday > 31)
  {
    return -1;
  }
  
  p += 3;
  if (__ng_islower(p[0]) || p[3] != ' ')
    return -1;

  switch (p[0])
  {
    case 'J':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"an")
        tm->tm_mon = 0;
      else if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"un")
        tm->tm_mon = 5;
      else if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ul")
        tm->tm_mon = 6;
      else
        return -1;
      break;
    case 'F':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"eb")
        tm->tm_mon = 1;
      else
        return -1;
      break;
    case 'M':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ar")
        tm->tm_mon = 2;
      else
        return -1;
      break;
    case 'A':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"pr")
        tm->tm_mon = 3;
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ug")
        tm->tm_mon = 7;
      else
        return -1;
      break;
    case 'S':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ep")
        tm->tm_mon = 8;
      else
        return -1;
      break;
    case 'O':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ct")
        tm->tm_mon = 9;
      else
        return -1;
      break;
    case 'N':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ov")
        tm->tm_mon = 10;
      else
        return -1;
      break;
    case 'D':
      if (*(ng_uint16_t*)&p[1] == *(const ng_uint16_t*)"ec")
        tm->tm_mon = 11;
      else
        return -1;
      break;
    default:
      return -1;
  }
  
  if (tm->tm_mday > __ng_mon_DAYS[tm->tm_mon])
    return -1;

  p += 3;
  if (p[4] != ' ' || !__ng_isdigit(p[0]) || !__ng_isdigit(p[1]) 
    || !__ng_isdigit(p[2]) || !__ng_isdigit(p[3]))
    return -1;

  tm->tm_year =  __ng_hex2int(p[0])*1000 
     + __ng_hex2int(p[1])*100
     + __ng_hex2int(p[2])*10
     + __ng_hex2int(p[3]);
  if (tm->tm_year - 1900 > __ng_YEARS_N)
    return -1;
  tm->tm_year -= 1900;

  p += 5;
  if ( !__ng_isdigit(p[0]) || !__ng_isdigit(p[1]) || p[2] != ':' 
    || !__ng_isdigit(p[3]) || !__ng_isdigit(p[4]) || p[5] != ':'
    || !__ng_isdigit(p[6]) || !__ng_isdigit(p[7]))
    return -1;

  tm->tm_hour = __ng_hex2int(p[0])*10 +  __ng_hex2int(p[1]);
  if (tm->tm_hour > 23)
    return -1;

  tm->tm_min = __ng_hex2int(p[3])*10 +  __ng_hex2int(p[4]);
  if (tm->tm_min > 59)
    return -1;

  tm->tm_sec = __ng_hex2int(p[6])*10 +  __ng_hex2int(p[7]);
  if (tm->tm_sec > 60)
    return -1;

  tm->tm_gmtoff = ng_os_info.tz_offset;
  tm->tm_zone   = " GMT";
  return 0;
}

static const char *min_sec_hour_day_name_u32[] = 
  {
    " 00 "," 01 "," 02 "," 03 "," 04 "," 05 "," 06 "," 07 "," 08 ",
    " 09 "," 10 "," 11 "," 12 "," 13 "," 14 "," 15 "," 16 "," 17 ",
    " 18 "," 19 "," 20 "," 21 "," 22 "," 23 "," 24 "," 25 "," 26 ",
    " 27 "," 28 "," 29 "," 30 "," 31 "," 32 "," 33 "," 34 "," 35 ",
    " 36 "," 37 "," 38 "," 39 "," 40 "," 41 "," 42 "," 43 "," 44 ",
    " 45 "," 46 "," 47 "," 48 "," 49 "," 50 "," 51 "," 52 "," 53 ",
    " 54 "," 55 "," 56 "," 57 "," 58 "," 59 "," 60 "
  };
static const char *min_sec_hour_day_name_u16[] = 
  {
    "00: ","01: ","02: ","03: ","04: ","05: ","06: ","07: ","08: ",
    "09: ","10: ","11: ","12: ","13: ","14: ","15: ","16: ","17: ",
    "18: ","19: ","20: ","21: ","22: ","23: ","24: ","25: ","26: ",
    "27: ","28: ","29: ","30: ","31: ","32: ","33: ","34: ","35: ",
    "36: ","37: ","38: ","39: ","40: ","41: ","42: ","43: ","44: ",
    "45: ","46: ","47: ","48: ","49: ","50: ","51: ","52: ","53: ",
    "54: ","55: ","56: ","57: ","58: ","59: ","60: "
  };
static int __raw_ng_http_tm(ng_rtc_time_s *tm, char *buf, int len, int isHttp, 
  const char *tz)
{
  char *p = buf;

  if (isHttp)
  {
    *(ng_uint64_t*)p = *(const ng_uint64_t*)"Date:   ";
    p += 6;
  }
  *(ng_uint32_t*)p = *(const ng_uint32_t*)week_name[tm->tm_wday];
  p += 4;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)min_sec_hour_day_name_u32[tm->tm_mday];
  p += 4;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)month_name[tm->tm_mon];
  p += 4;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)year_name[tm->tm_year - 70];
  p[4] = ' ';
  p += 5;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)(min_sec_hour_day_name_u16[tm->tm_hour]);
  p += 3;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)(min_sec_hour_day_name_u16[tm->tm_min]);
  p += 3;
  *(ng_uint16_t*)p = *(const ng_uint16_t*)(min_sec_hour_day_name_u16[tm->tm_sec]);
  p += 2;
  if (tz == NULL) 
    tz = ng_os_info.tz;
  *(ng_uint32_t*)p = *(const ng_uint32_t*)tz;
  p += 4;
  if (isHttp)
  {
    *(ng_uint16_t*)p = *(const ng_uint16_t*)"\r\n";
    p += 2;
  }
  return p - buf;
}

int raw_ng_http_tm(ng_rtc_time_s *tm, char *buf, int len, int isHttp)
{
  ng_tmv_s tv;
  ng_result_t r = ng_tm2unix_time(tm, &tv);
  
  if (r != ng_ERR_NONE || ng_unix2tm_time(tm, &tv, 0) != ng_ERR_NONE)
    return -1;
  
  return __raw_ng_http_tm(tm, buf, len, isHttp, tm->tm_zone);
}

int raw_ng_tm(ng_rtc_time_s *tm, char *buf, int len)
{
  ng_tmv_s tv;
  ng_result_t r = ng_tm2unix_time(tm, &tv);
  
  if (r != ng_ERR_NONE || ng_unix2tm_time(tm, &tv, 0) != ng_ERR_NONE)
    return -1;
  
  return __raw_ng_http_tm(tm, buf, len, 0, tm->tm_zone);
}

int raw_ng_http_date(ng_time_t t, char *buf, int len, int isHttp, 
  const char *tz)
{
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

  if (t)
    tv.tv_sec = t;
  else
    tv.tv_sec = ng_get_time();
  tv.tv_usec = 0;
  if (ng_unix2tm_time(&tm, &tv, tz_offset) != ng_ERR_NONE)
    return -1;

  return __raw_ng_http_tm(&tm, buf, len, isHttp, tz);
}

int __ng_http_date(char *buf, int len, int isHttp, 
  const char *tz)
{
  return raw_ng_http_date(0, buf, len, isHttp, tz);
}

int ng_http_date(ng_buffer_s *b)
{
  return __ng_http_date(b->buf, b->len, 1, " GMT");
}

int ng_http_date_s(char *buf, int len)
{
  return __ng_http_date(buf, len, 1, " GMT");
}

void ng_date_print(void)
{
  char date[HTTPD_DATE_STRLEN_MIN];
  ng_buffer_s b;
  BUF_SET(&b, date, sizeof(date));
  __ng_http_date(b.buf, b.len, 0, NULL);
  log_print("\nSystem is already running %"PRIu64" seconds\n", 
    ng_get_time() - (ng_uint64_t)startime);
  log_print("Date   : %.*s\n\n", (int)b.len, b.cptr);
  
}
