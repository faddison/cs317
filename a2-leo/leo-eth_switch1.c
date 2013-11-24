/*
 * eth_switch1.c
 * Author:
 */

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "eth_switch1.h"

/* Creates a new empty switch table (with no addresses associated to
 * any port).
 */
switch_table create_switch_table(void) {
	switch_table new_switch_table = malloc(sizeof(switch_table));
	new_switch_table->port_number = 0;
	new_switch_table->mac_address = 0;
	new_switch_table->next = NULL;
	new_switch_table->prev = NULL;
	return new_switch_table;
}

/* Handles an incoming Ethernet frame. This function is responsible
 * with updating the switch table with the data that can be deduced
 * from the frame, and returning a set of ports where the frame must
 * be forwarded to. The set of ports is represented by the port_set
 * type, and the function may return one of the following macros:
 *
 * - PORT_SET_NONE - Forward to no port at all (drop frame);
 * - PORT_SET_ALL - Forward to all ports;
 * - PORT_SET_ADD_PORT(s, p) - Forward to all ports in set s and to p (e.g., 
 *     PORT_SET_ADD_PORT(PORT_SET_NONE, 2) for port 2 only);
 * - PORT_SET_REMOVE_PORT(s, p) - Forward to all ports in set s but not to p (e.g., 
 *     PORT_SET_REMOVE_PORT(PORT_SET_ALL, 7) for all ports except 7);
 */
port_set forward_incoming_frame(switch_table *table, uint8_t port, uint16_t destination,
		uint16_t source, uint16_t frameid) {

	//Keep track of head
	switch_table switch_table_head = *table;
    (*table)->frameid = frameid;
	// Check for base scenario when switch table is empty
	if ((*table)->port_number == 0 && (*table)->next == NULL) {
		(*table)->port_number = port;
		(*table)->mac_address = source;
	}
	else {
		//Check case where switch table only has 1 entry
		if ((*table)->next == NULL) {
			//update switch table if address exists
			if ((*table)->mac_address == source) {
				(*table)->port_number = port;
			}
			//add new switch table entry if not
			else {
				switch_table new_switch_table = create_switch_table();
				new_switch_table->port_number = port;
				new_switch_table->mac_address = source;
				if (source < (*table)->mac_address) {
					switch_table_head = new_switch_table;
					new_switch_table->next = *table;
					(*table)->prev = new_switch_table;
				}
				else {
					(*table)->next = new_switch_table;
					new_switch_table->prev = (*table);
				}
			}
		}
		else {
			//while not empty check all switch tables
			while ((*table)->next != NULL) {
				//if mac address already exists in table, update the port
				if ((*table)->mac_address == source) {
					(*table)->port_number = port;
					break;
				}
				*table = (*table)->next;
			}
			// check if we reached the end of the switch table without finding a match
			// must add to switch table
			if ((*table)->next == NULL) {
				//must check last element
				if ((*table)->mac_address == source) {
					(*table)->port_number = port;
				}
				//reached end of table finding no match
				else {
					//reset pointer back to head
					while ((*table)->prev !=NULL) {
						*table = (*table)->prev;
					}
					//Prep new switch table entry
					switch_table new_switch_table = create_switch_table();
					new_switch_table->port_number = port;
					new_switch_table->mac_address = source;
					//Check if it should be added as the new head
					if (source < (*table)->mac_address) {
						switch_table_head = new_switch_table;
						new_switch_table->next = *table;
						(*table)->prev = new_switch_table;
					}
					else {
						while ((*table)->next != NULL && source > (*table)->next->mac_address) {
							*table = (*table)->next;		
						}
						// Reached end of table
						if ((*table)->next == NULL) {
							(*table)->next = new_switch_table;
							new_switch_table->prev = *table;
						}	
						// Adding in middle
						else {
							switch_table temp_switch_table = (*table)->next;
							(*table)->next = new_switch_table;
							new_switch_table->prev = *table;
							new_switch_table->next = temp_switch_table;
							temp_switch_table->prev = new_switch_table;
						}
					}
				}
			}
		}
	}
	//reset pointer back to head, check table for return value
	while ((*table)->prev != NULL) {
		*table = (*table)->prev;
	}	

	if (destination == source) {
		return PORT_SET_NONE;
	}

	if (destination == (*table)->mac_address) {
		return PORT_SET_ADD_PORT(PORT_SET_NONE, (*table)->port_number);
	}
	else {
		while ((*table)->next != NULL) {
			if ((*table)->mac_address == destination) {
				uint8_t port_number = (*table)->port_number;
				while ((*table)->prev != NULL) {
					*table = (*table)->prev;
				}
				return PORT_SET_ADD_PORT(PORT_SET_NONE, port_number);
			}
			*table = (*table)->next;
		}
		if ((*table)->mac_address == destination) {
			uint8_t port_number = (*table)->port_number;
			while ((*table)->prev != NULL) {
				*table = (*table)->prev;
			}
			return PORT_SET_ADD_PORT(PORT_SET_NONE, port_number);
		}
	}
	while ((*table)->prev != NULL) {
		*table = (*table)->prev;
	}
	return PORT_SET_REMOVE_PORT(PORT_SET_ALL, port);
}

/* Prints all the elements in the switch table in numerical order of
 * the MAC address. Elements are printed one per line, with each line
 * containing the MAC address (represented in hexadecimal with 4
 * characters) followed by a space and the port number (prefixed with
 * a P and with no leading zeros).
 */
void print_switch_table(switch_table table, FILE *output) {
	if (table->port_number != 0 && table->next == NULL) {
		fprintf(output, "%04"PRIx16" P%"PRIu8"\n", table->mac_address, table->port_number);
	}

	else {
		while (table->next != NULL) {
			fprintf(output, "%04"PRIx16" P%"PRIu8"\n", table->mac_address, table->port_number);
			table = table->next;
		}
		fprintf(output, "%04"PRIx16" P%"PRIu8"\n", table->mac_address, table->port_number);
	}
	/* for each element in the switch table do:
	   fprintf(output, "%04"PRIx16" P%"PRIu8"\n", mac_address, port);
	 */
}

/* Frees any dynamically allocated space used by the switch table.
 */
void destroy_switch_table(switch_table table) {
	if (table != NULL) {
		while (table->next !=NULL) {
			switch_table previous = table;
			table = table->next;
			free(previous);
		}
	}
}

