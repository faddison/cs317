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
    int MAXDATASIZE = 100000;
    char *buf[MAXDATASIZE];
    int bytes_read  = 1;
	char *res = "";
		
		// need to look for eof
    while (1)
    {
		res = "";
    	bytes_read = recv(socket, buf, MAXDATASIZE - 1, 0);
			//printf("\n#bytes: %d\n", bytes_read);
    	
    	if (bytes_read > 1)
		{	
			printf("\n%s\n\n", buf);
			char *uri = http_parse_uri(buf);
			char *path = http_parse_path(uri);
			if (strlen(uri) > 0) printf("%s\n\n", uri);
			if (strlen(path) > 0) printf("%s\n\n", path);
			if (strcmp(path, "/knock") == 0)
				res = build_response(res);
			else
				res = build_status_str(res, "200 OK");
		}
		
    	if (bytes_read > 0)
    	{
    		send(socket, res, strlen(res), 0);
    		//close(socket); 
    	}
    	*buf = '\0';
    }
    
    return;
}

char *append(char *first, char *second) 
{
	char * s = malloc(snprintf(NULL, 0, "%s%s", first, second) + 1);
	sprintf(s, "%s%s", first, second);
	return s;
}

char *build_response(char *res)
{
	char *body = "Who's theres?";
	char body_len[15];
	sprintf(body_len, "%d", strlen(body));
	
	res = build_status_str(res, "200 OK");
	res = build_connection_str(res, "keep-alive");
	res = build_cache_control_str(res, "public");
	res = build_content_length_str(res, body_len);
	res = build_content_type_str(res, "text/plain");
	res = build_date_str(res, "336 am");
	res = build_body_str(res, body);
	return res;
}

char *build_status_str(char *res, char *param)
{
	res = append(res, "HTTP/1.1 ");
	return build_param_str(res, param);
}

char *build_connection_str(char *res, char *param)
{
	res = append(res, "Connection: ");
	return build_param_str(res, param);
}

char *build_cache_control_str(char *res, char *param)
{
	res = append(res, "Cache-Control: ");
	return build_param_str(res, param);
}

char *build_content_length_str(char *res, char *param)
{
	res = append(res, "Content-Length: ");
	return build_param_str(res, param);
}

char *build_content_type_str(char *res, char *param)
{
	res = append(res, "Content-Type: ");
	return build_param_str(res, param);
}

char *build_date_str(char *res, char *param)
{
	res = append(res, "Date: ");
	return build_param_str(res, param);
}

char *build_body_str(char *res, char *param)
{
	res = append(res, "\r\n");
	return build_param_str(res, param);
}

char *build_param_str(char *res, char *param)
{
	res = append(res, param);
	res = append(res, nextline());
	return res;
}

char *nextline()
{
	return "\r\n";
}
