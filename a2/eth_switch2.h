/*
 *  eth_switch2.h
 *  Author: Jonatan Schroeder
 */

#ifndef _ETH_SWITCH2_H_
#define _ETH_SWITCH2_H_

#include <stdint.h>

#include "eth_switch_limits.h"

typedef struct bounded_queue {
	int buffer[BUFFER_SIZE+1];
	int head;
	int tail;
	int size; 
} *bounded_queue;

bounded_queue create_bounded_queue(void);
void destroy_bounded_queue(bounded_queue queue);
uint16_t dequeue_bounded(bounded_queue queue);
int enqueue_bounded(bounded_queue queue, uint16_t frameid);
int queue_is_empty(bounded_queue queue);
int queue_is_full(bounded_queue queue);

#endif

