/*
 * File: service.h
 */

#ifndef _SERVICE_H_
#define _SERVICE_H_

void handle_client(int socket);
char *append(char *old_string, char *new_string);

char *build_response(char *res);
char *nextline();
char *build_body_str(char *res, char *param);
char *build_param_str(char *res, char *param);
char *build_date_str(char *res, char *param);
char *build_content_type_str(char *res, char *param);
char *build_content_length_str(char *res, char *param);
char *build_cache_control_str(char *res, char *param);
char *build_connection_str(char *res, char *param);
char *build_status_str(char *res, char *param);

#endif
