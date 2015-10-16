#ifndef _URL_UTIL_H
#define _URL_UTIL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../common/common_types.h"
#include "../util.h"

char from_hex(char ch);				// Converts a hex character to its integer value
char to_hex(char code);				// Converts an integer value to its hex character
char *url_encode(char *str);		// Returns a url-encoded version of str IMPORTANT: be sure to free() the returned string after use
char *url_decode(char *str);		// Returns a url-decoded version of str  IMPORTANT: be sure to free() the returned string after use 
const int url_parse(const char*, 	// parse url to host and address
	char*, const int, char*, const int);
#ifdef __cplusplus
}
#endif

#endif  /* _URL_UTIL_H */
