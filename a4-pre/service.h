/*
 * File: service.h
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

typedef struct
{
	char *method;
	char *uri;
	char *path;
	char *accept_charset;
	char *accept_encoding;
	char *accept_language;
	char *connection;
	char *host;
	char *user_agent;
	char *body;
} Request;

typedef struct
{
	char *date;
	char *status;
	char *connection;
	char *cache_control;
	char *content_length;
	char *content_type;
	char *body;
} Response;

void handle_client(int socket);
char *append(char *old_string, char *new_string);

Response build_fake_response();
char *nextline();

char *response_obj_to_string(Response resobj);
char *request_obj_to_string(Request reqobj);
char *status_to_string(char *res, char *param);
char *connection_to_string(char *res, char *param);
char *cache_control_to_string(char *res, char *param);
char *content_length_to_string(char *res, char *param);
char *content_type_to_string(char *res, char *param);
char *date_to_string(char *res, char *param);
char *body_to_string(char *res, char *param);
char *int_to_string(int n);


char *process_action(char *res, Request reqobj);
void ifprint(char *text);
Request parse_request(char *res);
char *process_request(char *res, char *reqstr);

#endif
