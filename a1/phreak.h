/*
 *  phreak.h
 *  Author: Jonatan Schroeder
 */

#ifndef _PHREAK_H_
#define _PHREAK_H_

#include "phreak_limits.h"

typedef struct connection {
  enum { CONN_NONE = 0, CONN_TRUNK = 'T', CONN_LINE = 'L' } type;
  int switch_number;
  union {
    int phone_number;
    int trunk_index;
  };
} connection;

typedef struct entity {
  enum { ENTITY_NONE = 0, ENTITY_PHONE = 'P', ENTITY_SWITCH = 'S', ENTITY_ROOT = 'R' } type;
  int switch_number;
  int phone_number;
} entity;

typedef struct signal {
  // NUM_POSSIBLE_SIGNALS added as a way to count possible signals, it's not a valid signal
  enum { SIGNAL_INVALID = 0, SIGNAL_CONNECT_REQUEST, SIGNAL_CONNECTED,
	 SIGNAL_BUSY, SIGNAL_DISCONNECTED, NUM_POSSIBLE_SIGNALS } type;
  connection source;
  connection destination; // Should be ignored in a DISCONNECT signal
} signal;

typedef struct root_state {
  
  connection trunk_port[NUM_TRUNKS][NUM_TRUNK_PORTS];
} root_state;

typedef struct switch_state {
  
  connection switch_port[NUM_TRUNK_PORTS];
	connection phone_port[NUM_PHONES_PER_SWITCH];
	
} switch_state;

typedef struct system_state {
  
  root_state root_state;
  switch_state switch_state[NUM_TRUNKS];
  
} system_state;

// Non-static functions in main.c
void forward_signal(system_state *state, entity from, entity to, signal signal);

// Non-static functions in phreak.c
void initialize_system_state(system_state *state);
void destroy_system_state(system_state *state);
void switch_process_signal(system_state *state, int switch_number,
			   entity from, signal recv_signal);
connection switch_current_connection(system_state *state, connection conn);
void root_process_signal(system_state *state, entity from, signal recv_signal);
connection root_current_connection(system_state *state, connection conn);

//
connection get_connection(system_state *state, int switch_number, connection conn, int is_root);
void connect(system_state *state, int switch_number, connection src, connection dest, int is_root);
void disconnect(system_state *state, int switch_number, connection src, connection dest, int is_root);
void process_connect(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);
signal process_disconnect(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);
void print_phreak_signal(entity from, int switch_number, signal signal, int is_root);
void set_connection_state(system_state *state, int switch_number, connection src, connection dest, int is_disconnection, int is_root);
char * signal_type_to_string(signal signal);
signal process_request(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);
connection find_free(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);
signal build_connection(system_state *state, int switch_number, signal new_signal, connection recv_source_conn, int is_root);
void process_connected(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);
int connections_equal(connection conn1, connection conn2);
connection find_connections(system_state *state, int switch_number, int is_switch, int is_signal_connected, connection target_connection, int is_root);
void print_conn(connection conn);
connection get_empty_conn();
void process_signal(system_state *state, int switch_number, entity from, signal recv_signal, int is_root);

#endif
