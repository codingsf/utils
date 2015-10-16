/*************************************************
*** defined by maobaichen at 2014/3/28 
*** version v1.0 
*** revision 
**************************************************/

#ifndef __LARGE_MALLOC_H__ 
#define __LARGE_MALLOC_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define LAEGEMEMSIZE   1024 * 1024
	
#ifdef __cplusplus
extern "C"
{
#endif

//free the current thread used resource
void	cJSON_Free();
//reset the current thread used resource
void	cJSON_Reset();
// inital resource
void	cJSON_Init(unsigned long blocksize = LAEGEMEMSIZE);
// free used resource
void	cJSON_Release();

#ifdef __cplusplus
}
#endif

#endif
