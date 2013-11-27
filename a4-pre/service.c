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

int MAXDATASIZE = 100000;

void handle_client(int socket) {
    
    /* TODO Loop receiving requests and sending appropriate responses,
     *      until one of the conditions to close the connection is
     *      met.
     */
    
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
				res = process_request(res, buf);
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

Response build_fake_response()
{
	char *body = "Who's theres?";
	
	Response resobj;
	
	resobj.status = "200 OK";
	resobj.connection = "keep-alive";
	resobj.cache_control = "public";
	resobj.content_length = int_to_string(strlen(body));
	resobj.content_type = "text/plain";
	resobj.date = "336 am";
	resobj.body = body;
	
	return resobj;
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

void ifprint(char *text)
{
	if (strlen(text) > 0) printf("%s\n", text);
}

char *process_request(char *res, char *reqstr)
{
	Request reqobj = parse_request(reqstr);
	//res = process_action(res, reqobj);
	Response resobj = build_fake_response();
	res = response_obj_to_string(resobj);
	
	return res;
}

char *process_action(char *res, Request reqobj)
{
	//if (strcmp(uri, "/knock") == 0)
		//res = build_response(res);
	//else
		//res = build_status_str(res, "200 OK");
	return res;
}

Request parse_request(char *reqstr)
{
	http_method method = http_parse_method(reqstr);
	char *uri = http_parse_uri(reqstr);
	char *path = http_parse_path(uri);
	// field 1
	// field 2
	// field n
	// body;
	
	Request reqobj;
	
	reqobj.uri = uri;
	reqobj.path = path;
	
	return reqobj;
}

char *response_obj_to_string(Response resobj)
{
	char *res = "";
	
	res = status_to_string(res, resobj.status);
	res = connection_to_string(res, resobj.connection);
	res = cache_control_to_string(res, resobj.cache_control);
	res = content_length_to_string(res, resobj.content_length);
	res = content_type_to_string(res, resobj.content_type);
	res = date_to_string(res, resobj.date);
	res = body_to_string(res, resobj.body);
	
	return res;
}

char *request_obj_to_string(Request reqobj)
{
	return "";
}

char *status_to_string(char *res, char *param)
{
	res = append(res, "HTTP/1.1 ");
	return build_param_str(res, param);
}

char *connection_to_string(char *res, char *param)
{
	res = append(res, "Connection: ");
	return build_param_str(res, param);
}

char *cache_control_to_string(char *res, char *param)
{
	res = append(res, "Cache-Control: ");
	return build_param_str(res, param);
}

char *content_length_to_string(char *res, char *param)
{
	res = append(res, "Content-Length: ");
	return build_param_str(res, param);
}

char *content_type_to_string(char *res, char *param)
{
	res = append(res, "Content-Type: ");
	return build_param_str(res, param);
}

char *date_to_string(char *res, char *param)
{
	res = append(res, "Date: ");
	return build_param_str(res, param);
}

char *body_to_string(char *res, char *param)
{
	res = append(res, "\r\n");
	return build_param_str(res, param);
}

char *int_to_string(int n)
{
	char *body_len = malloc(MAXDATASIZE);
	sprintf(body_len, "%d", n);
	return body_len;
}








