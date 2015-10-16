#include "url_util.h"
#include <ctype.h>

/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
  char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/* Returns a url-decoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_decode(char *str) {
  char *pstr = str, *buf = (char *)malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

const int 
url_parse(const char *Uri, char *RecvHost, const int RecvHostLen,
	char *RecvToken, const int RecvTokenLen)
{
	// "http://db3.notify.live.net/throttledthirdparty/01.00/AAEj2xatFBJVSZTDulDtwdwOAgAAAAADAQAAAAQUZm52OjIzOEQ2NDJDRkI5MEVFMEQ"
	int iUriLen = strlen(Uri);
	char *pHost = strstr(Uri, "//");
	if (NULL == pHost)
	{
		return -1;
	}
	
	pHost += strlen("//");
	int iHeadLen = pHost - Uri;
	if (iHeadLen >= iUriLen)
	{
		return -2;
	}
	
	char *pToken = strstr(pHost, "/");
	if (NULL == pToken)
	{
		return -3;
	}
	
	int iHostLen = pToken - pHost;	
	if (iHostLen > RecvHostLen)
	{
		return -4;
	}
	strncpy(RecvHost, pHost, iHostLen);
	
	int iTokenlen = iUriLen - (pToken - Uri);
	if (iTokenlen > RecvTokenLen)
	{
		return -5;
	}
	strncpy(RecvToken, pToken, iTokenlen);
	return 0;
}

