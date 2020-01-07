/*
 * Test program to check the performance of
 * PMEM + swapcache promotion
 *
 * Author: Feng Tang <feng.tang@intel.com>
 *
 */
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DATA_SIZE	(1 << 30)	/* 1GB */
#define NR_TASKS	72			

/* return (tv2 - tv1) */
static struct timeval tv_sub(struct timeval tv1, struct timeval tv2)
{
	struct timeval delta;

	delta.tv_sec = tv2.tv_sec - tv1.tv_sec;

	if (tv1.tv_usec > tv2.tv_usec) {
		delta.tv_usec = tv2.tv_usec + 1000000 - tv1.tv_usec;
		delta.tv_sec--;
	} else
		delta.tv_usec = tv2.tv_usec - tv1.tv_usec;

	return delta;
}

int main(int argc, char *argv[])
{
	int i, j;
	unsigned char *buf;
	pid_t pid, fid;
	struct timeval tv1, tv2, tv_delta;
	int ret;

	#if 1
	system("echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_kswapd_sleep/enable");
	system("echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_kswapd_wake/enable");
	system("echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_wakeup_kswapd/enable");

//	system("echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_direct_reclaim_begin/enable");
//	system("echo 1 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_direct_reclaim_end/enable");
	#endif

	for (i = 0; i < NR_TASKS; i++) {

		/* Following are all Child's job */
		buf = malloc(DATA_SIZE);
		if (!buf) {
			fprintf(stderr, "Child[%d]: Malloc fail!\n", i);
			return -1;
		}

		/* Manipulate the 2nd half memroy to make it hot */
		ret = gettimeofday(&tv1, NULL);
		for (j = 0; j < DATA_SIZE; j += 4096)
			buf[j] = 0;
		ret = gettimeofday(&tv2, NULL);
		tv_delta = tv_sub(tv1, tv2);

		fprintf(stderr, "[%d]: It took %ld:%ld to get memory\n",
			i,
			tv_delta.tv_sec,
			tv_delta.tv_usec);
	}

	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_kswapd_sleep/enable");
	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_kswapd_wake/enable");
	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_wakeup_kswapd/enable");
	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_node_reclaim_begin/enable");
	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_node_reclaim_end/enable");


	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_direct_reclaim_begin/enable");
	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/mm_vmscan_direct_reclaim_end/enable");

	system("echo 0 > /sys/kernel/debug/tracing/events/vmscan/enable");

	system(" free -m >> /dev/shm/result.log");

	sleep(5);

	exit(EXIT_SUCCESS);
}

