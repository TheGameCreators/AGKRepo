/*
 * Copyright (c) 2009-2012 Petri Lehtinen <petri@digip.org>
 *
 * Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef UTF_H
#define UTF_H


#ifdef _WIN32
typedef int int32_t;
#else /* !_WIN32 */
/* Assume a standard environment */
#include <inttypes.h>
#endif /* _WIN32 */


int utf8_encode(int codepoint, char *buffer, int *size);

int utf8_check_first(char byte);
int utf8_check_full(const char *buffer, int size, int32_t *codepoint);
const char *utf8_iterate(const char *buffer, int32_t *codepoint);

int utf8_check_string(const char *string, int length);

#endif
