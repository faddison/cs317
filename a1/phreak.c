/*
 *  phreak.c
 *  Author: Jonatan Schroeder
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "phreak.h"

int debug = 0;

/* Initialize the system with a state where no lines are connected or in use. */
void initialize_system_state(system_state *state) 
{
	int i;
	int j;
	int k;

  for (i = 0; i < NUM_TRUNKS; i++)
	{
		for (j = 0; j < NUM_TRUNK_PORTS; j++)
		{
			(*state).switch_state[i].switch_port[j] = get_empty_conn();
			(*state).root_state.trunk_port[i][j] = get_empty_conn();
		}	

		for (k = 0; k < NUM_PHONES_PER_SWITCH; k++)
		{
			(*state).switch_state[i].phone_port[k] = get_empty_conn();
		}
	}
}

connection get_empty_conn()
{
	connection conn;
	conn.type = 0;
	conn.switch_number = 0;
	conn.phone_number = 0;
	conn.trunk_index = 0;
	return conn;
}

/* Free potential dynamically allocated space and close any resources no longer in use. */
void destroy_system_state(system_state *state) {

  // TODO
}

char * signal_type_to_string(signal signal)
{
	switch (signal.type) 
	{
		case SIGNAL_CONNECT_REQUEST:
			return "connect_req";
			break;
		case SIGNAL_DISCONNECTED:
			return "disconnected";
			break;
		case SIGNAL_CONNECTED:
			return "connected";
			break;
		case SIGNAL_BUSY:
			return "busy";
			break;
		default:
			return "invalid";
			break;
	}
}			

void print_phreak_signal(entity from, int switch_number, signal signal, int is_root) 
{
  if (is_root)
		printf("Signal R to ");
	else
		printf("Signal S%d to " , switch_number);
  switch (from.type) 
	{
		case ENTITY_PHONE:
		  printf("%c%d%d", 'P', from.switch_number, from.phone_number); 
			break;
		case ENTITY_SWITCH:
		  printf("%c%d", 'S', from.switch_number); 
			break;
		case ENTITY_ROOT:
		  printf("%c", 'R'); 
			break;
		default: break;
  }
  
  if (signal.type < 0 || signal.type >= NUM_POSSIBLE_SIGNALS) signal.type = SIGNAL_INVALID;
  {
		printf(": %s %c%d%d", signal_type_to_string(signal),
		 signal.source.type, signal.source.switch_number, signal.source.phone_number);
	}
  if (signal.destination.type)
	{
    printf(" to %c%d%d", signal.destination.type,
	   signal.destination.switch_number, signal.destination.phone_number);
	}
  printf("\n");
}

/* Returns the status of the current connection linked to connection
   conn in switch conn.switch_number. If the connection is not
   currently linked to anything, returns a connection with type
   CONN_NONE. */
connection switch_current_connection(system_state *state, connection conn) {
  return get_connection(state, conn.switch_number, conn, 0);
  // TODO
}

/* Handles an incoming signal received by the root switch, coming from
   the entity represented by the parameter 'from' (assumed to be a
   switch). If the result of this function is the generation of a new
   signal, this function will call forward_signal with appropriate
   parameters. */
void root_process_signal(system_state *state, entity from, signal recv_signal) 
{
  process_signal(state, 0, from, recv_signal, 1);
}

/* Handles an incoming signal received by the switch identified by
   'switch_number', coming from the entity represented by the
   parameter 'from' (the root trunk or a phone). If the result of this
   function is the generation of a new signal, this function will call
   forward_signal with appropriate parameters. */
void switch_process_signal(system_state *state, int switch_number, entity from, signal recv_signal) 
{
	process_signal(state, switch_number, from, recv_signal, 0);
}

void process_signal(system_state *state, int switch_number, entity from, signal recv_signal, int is_root) 
{
	switch(recv_signal.type) 
	{
		case SIGNAL_CONNECT_REQUEST:
			process_connect(state, switch_number, from, recv_signal, is_root);
		  break;
		case SIGNAL_DISCONNECTED:
			process_disconnect(state, switch_number, from, recv_signal, is_root);
			break;
		case SIGNAL_CONNECTED:
			process_connected(state, switch_number, from, recv_signal, is_root);
			break;
		default:
			break;
  }
}

/* Returns the status of the current connection linked to connection
   conn in the root. If the connection is not currently linked to
   anything, returns a connection with type CONN_NONE. */
connection root_current_connection(system_state *state, connection conn) {
  return get_connection(state, conn.switch_number, conn, 1);
}

void process_connect(system_state *state, int switch_number, entity from, signal recv_signal, int is_root)
{
	signal new_signal = recv_signal;
	
	if (recv_signal.source.switch_number != recv_signal.destination.switch_number || is_root)
	{
		new_signal = process_request(state, switch_number, from, recv_signal, is_root);
		if (is_root)
		{
			from.switch_number = new_signal.source.switch_number;
		}
		if (new_signal.source.type == CONN_TRUNK && !(is_root))
			from.type = ENTITY_ROOT;
		print_phreak_signal(from, switch_number, new_signal, is_root);
		if (new_signal.type != SIGNAL_BUSY)
			build_connection(state, switch_number, new_signal, recv_signal.source, is_root);
	}
	else
	{
		new_signal = build_connection(state, switch_number, new_signal, ((connection){.type = CONN_NONE}), is_root);
		print_phreak_signal(from, switch_number, new_signal, is_root);
	}
}

signal build_connection(system_state *state, int switch_number, signal new_signal, connection recv_source_conn, int is_root)
{
	connection src_conn = get_connection(state, switch_number, new_signal.source, is_root);
	connection dest_conn;
	
	if (recv_source_conn.type == CONN_NONE)
		dest_conn = get_connection(state, switch_number, new_signal.destination, is_root);
	else
		dest_conn = get_connection(state, switch_number, recv_source_conn, is_root);

	if (src_conn.type != CONN_NONE)
	{
		new_signal.type = SIGNAL_BUSY;
		disconnect(state, switch_number, new_signal.source, src_conn, is_root);
	}
	else if (dest_conn.type != CONN_NONE)
	{
		new_signal.type = SIGNAL_BUSY;
	}
	else // connect!
	{
		new_signal.type = SIGNAL_CONNECTED;
		if (recv_source_conn.type == CONN_NONE)
			connect(state, switch_number, new_signal.source, new_signal.destination, is_root);
		else
			connect(state, switch_number, new_signal.source, recv_source_conn, is_root);
	}
	return new_signal;
}

signal process_request(system_state *state, int switch_number, entity from, signal recv_signal, int is_root)
{
	recv_signal.type = SIGNAL_CONNECT_REQUEST;
	connection new_source_conn = find_free(state, switch_number, from, recv_signal, is_root);
	if (connections_equal(new_source_conn, get_empty_conn()))
	{
		disconnect(state, switch_number, recv_signal.destination, get_connection(state, switch_number, recv_signal.destination, is_root), is_root);
		recv_signal.type = SIGNAL_BUSY;
	}
	else
		recv_signal.source = new_source_conn;
	return recv_signal;
}

connection find_free(system_state *state, int switch_number, entity from, signal recv_signal, int is_root)
{
	if (is_root)
		return find_connections(state,recv_signal.destination.switch_number,1,0,get_empty_conn(), is_root);
	else
		return find_connections(state, switch_number ,1,0,get_empty_conn(), is_root);
}

signal process_disconnect(system_state *state, int switch_number, entity from, signal recv_signal, int is_root)
{
	connection new_conn;
	if (is_root)
		new_conn = get_connection(state, recv_signal.destination.switch_number, recv_signal.source, is_root);
	else
		new_conn = get_connection(state, switch_number, recv_signal.source, is_root);
	disconnect(state, switch_number, recv_signal.source, new_conn, is_root);
	signal result_signal = recv_signal;
	result_signal.type = SIGNAL_DISCONNECTED;
	result_signal.source = new_conn;
	if (is_root)
	{
		from.switch_number = result_signal.source.switch_number;
	}
	else if (new_conn.type == CONN_TRUNK)
		from.type = ENTITY_ROOT;
	else //(new_conn.type == CONN_LINE)
	{
		from.type = ENTITY_PHONE;
		from.phone_number = new_conn.phone_number;
	}
	print_phreak_signal(from, switch_number, result_signal, is_root);
	return result_signal;
}

void disconnect(system_state *state, int switch_number, connection src, connection dest, int is_root)
{
	set_connection_state(state, switch_number, src, dest, 1, is_root);
}

void connect(system_state *state, int switch_number, connection src, connection dest, int is_root)
{
	if (debug) { printf("Connecting...\n"); print_conn(src); print_conn(dest);}
	set_connection_state(state, switch_number, src, dest, 0, is_root);
}

void set_connection_state(system_state *state, int switch_number, connection src, connection dest, int is_disconnection, int is_root)
{
	connection new_dest = dest;
	connection new_src = src;
	
	if (is_disconnection)
	{
		new_dest = get_empty_conn();
		new_src = get_empty_conn();
	}
	
	switch(src.type)
	{
		case CONN_TRUNK:
			switch(dest.type)
			{
				case CONN_TRUNK:
					// root connection
					(*state).root_state.trunk_port[src.switch_number][src.trunk_index] = new_dest;
					(*state).root_state.trunk_port[dest.switch_number][dest.trunk_index] = new_src;
					break;
				case CONN_LINE: // source = trunk, dest = line
					(*state).switch_state[switch_number].switch_port[src.trunk_index] = new_dest;
					(*state).switch_state[switch_number].phone_port[dest.phone_number] = new_src;
					break;
				default:
					break;
			}
			break;
		case CONN_LINE:
			switch(dest.type)
			{
				case CONN_TRUNK: // source = line, dest = trunk
					(*state).switch_state[switch_number].phone_port[src.phone_number] = new_dest;
					(*state).switch_state[switch_number].switch_port[dest.trunk_index] = new_src;
					break;
				case CONN_LINE: // source = line, dest = line
					(*state).switch_state[switch_number].phone_port[src.phone_number] = new_dest;
					(*state).switch_state[switch_number].phone_port[dest.phone_number] = new_src;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

connection get_connection(system_state *state, int switch_number, connection conn, int is_root)
{
	switch(conn.type)
	{
		case CONN_TRUNK:
			if (is_root)
				return (*state).root_state.trunk_port[switch_number][conn.trunk_index];
			else
				return (*state).switch_state[switch_number].switch_port[conn.trunk_index];
			break;
		case CONN_LINE:
			return (*state).switch_state[switch_number].phone_port[conn.phone_number];
			break;
		default:
			return get_empty_conn();
			break;
	}
}

connection find_connections(system_state *state, int switch_number, int is_switch, int is_signal_connected, connection target_conn, int is_root)
{
	int i;
	
	if (is_switch && !is_root)
	{
		for (i = 0; i < NUM_TRUNK_PORTS; i++)
		{
			connection tmp = (*state).switch_state[switch_number].switch_port[i];
			if (connections_equal(tmp, target_conn))
				return ((connection){ .type = CONN_TRUNK, .switch_number = switch_number, .trunk_index = i });
		}
	}
	else if (is_root)
	{
		for (i = 0; i < NUM_TRUNK_PORTS; i++)
		{
			if (debug) printf("switch:%d, i:%d\n", switch_number, i);
			connection tmp = (*state).root_state.trunk_port[switch_number][i];
			if (connections_equal(tmp, target_conn))
				return ((connection){ .type = CONN_TRUNK, .switch_number = switch_number, .trunk_index = i });
		}
	}
	else
	{
		for (i = 0; i < NUM_PHONES_PER_SWITCH; i++)
		{
			connection tmp = (*state).switch_state[switch_number].phone_port[i];
			if (connections_equal(tmp, target_conn))
				return ((connection){ .type = CONN_LINE, .switch_number = switch_number, .phone_number = i });
		}
	}
	return get_empty_conn();
}

void print_conn(connection conn)
{
	printf("Connection - type:%d, switch:%d, phone:%d, trunk:%d\n",conn.type, conn.switch_number, conn.phone_number, conn.trunk_index); 
}

int connections_equal(connection conn1, connection conn2)
{
	if (debug) printf("Comparing...\n");
	if (debug) print_conn(conn1);
	if (debug) print_conn(conn2);
	
	if (conn1.type == conn2.type && conn1.switch_number == conn2.switch_number && (conn1.phone_number == conn2.phone_number || conn1.trunk_index == conn2.trunk_index))
	{
		if (debug) printf("Equal\n");
		return 1;
	}
	else
	{
		if (debug) printf("Not Equal\n");
		return 0;
	}
}

void process_connected(system_state *state, int switch_number, entity from, signal recv_signal, int is_root)
{
	connection src_conn = find_connections(state,switch_number,0,1,recv_signal.source, is_root);
	recv_signal.source = src_conn;
	recv_signal.type = SIGNAL_CONNECTED;
	if (from.type == ENTITY_ROOT)
	{
		from.type = ENTITY_PHONE;
		from.switch_number = src_conn.switch_number;
		from.phone_number = src_conn.phone_number;
	}
	print_phreak_signal(from, switch_number, recv_signal, is_root);
}








































