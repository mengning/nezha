
#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdlib.h>
#include<stdio.h>

#define debug           

#define SUCCESS 		0
#define FAILURE 		(-1)

#define	true		1
#define false		0

/*definition of TRACE_WANTED*/
#ifndef		TRACE_WANTED
#define		TRACE_WANTED
#endif
/*#undef		TRACE_WANTED*/

/* DEBUG_PRINTF for system init/shut trace */
#ifdef TRACE_WANTED
#define DEBUG_PRINTF(fmt) 					printf(fmt)
#define DEBUG_PRINTF1(fmt,arg1) 			printf(fmt,arg1)       	
#define DEBUG_PRINTF2(fmt,arg1,arg2)		printf(fmt,arg1,arg2)       	
#define DEBUG_PRINTF3(fmt,arg1,arg2,arg3)  	printf(fmt,arg1,arg2,arg3)       	
#else
#define DEBUG_PRINTF(fmt)
#define DEBUG_PRINTF1(fmt,arg1) 
#define DEBUG_PRINTF2(fmt,arg1,arg2)  
#define DEBUG_PRINTF3(fmt,arg1,arg2,arg3)    	
#endif /* TRACE_WANTED */

#endif
