/*
 * File: service.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "service.h"
#include "util.h"

void handle_client(int socket) {
    
    /* TODO Loop receiving requests and sending appropriate responses,
     *      until one of the conditions to close the connection is
     *      met.
     */
    int MAXDATASIZE = 100;
    char *buf[MAXDATASIZE];
    int bytes_read  = 1;
    //int i = 0;
    while (1)
    {
    	bytes_read = recv(socket, buf, MAXDATASIZE - 1, 0);
    	/*
    	for (i = 0; i < MAXDATASIZE - 1; i++)
    	{
    		printf("%s ", buf[i]);
    	}
    	*/
    	//char *uri = http_parse_uri(*buf);
	    //fprintf(stderr, "%s", uri);
			char *msg = "HTTP/1.1 200 OK\r\n";
			int len = strlen(msg);
    	if (bytes_read > 0)
    	{
    		send(socket, msg, len, 0);
    		//close(socket); 
    	}
    	*buf = '\0';
    }
    
    return;
}
