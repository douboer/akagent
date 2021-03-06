#ifndef __MANGOSTEEN_STRINGS_H__
#define __MANGOSTEEN_STRINGS_H__

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int string_strcmp(const char *s1,const char *s2);

void string_trim_blank(char *pStr);

void string_trim_zero(char *pStr);

int string_string_to_hex(char *str, unsigned char *out);

void string_escape_slash(char *org ,int size);

char *string_string_to_lower(char *str);

void string_itoa(int n,char *s);

char *string_trim_hspace(char *buf);

void string_trim_tab(char *buf);

int string_find_delim(char *buffer ,char delim ,int size);

#endif
