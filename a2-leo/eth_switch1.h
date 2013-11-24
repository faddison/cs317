/*
 *  eth_switch1.h
 *  Author: Jonatan Schroeder
 */

#ifndef _ETH_SWITCH1_H_
#define _ETH_SWITCH1_H_

#include <stdio.h>
#include <stdint.h>

#include "eth_switch_limits.h"

typedef struct switch_table {
  uint8_t port_number;
  uint16_t mac_address;
    uint16_t frameid;
  struct switch_table *next;
  struct switch_table *prev; 
} *switch_table;

// Type representing a set of ports. It is used to represent the ports
// where a frame is expected to be forwarded to.
typedef uint64_t port_set;

// The following macros may be used as return value of forward_incoming_frame
#define PORT_SET_NONE                   ((port_set) 0)
#define PORT_SET_ALL                    ((port_set) ((1<<(NUM_PORTS))-1))
#define PORT_SET_ONE_PORT(port)         ((port_set) (1<<(port)))
#define PORT_SET_ADD_PORT(set, port)    ((port_set) ((set)|PORT_SET_ONE_PORT(port)))
#define PORT_SET_REMOVE_PORT(set, port) ((port_set) ((set)&~PORT_SET_ONE_PORT(port)))
#define PORT_SET_HAS_PORT(set, port)    ((set) & PORT_SET_ONE_PORT(port))

switch_table create_switch_table(void);
void print_switch_table(switch_table table, FILE *output);
void destroy_switch_table(switch_table table);

port_set forward_incoming_frame(switch_table *table, uint8_t port, uint16_t destination,
				uint16_t source, uint16_t frameid);

#endif

