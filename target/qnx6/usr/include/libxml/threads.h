/**
 * Summary: interfaces for thread handling
 * Description: set of generic threading related routines
 *              should work with pthreads, Windows native or TLS threads
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_THREADS_H__
#define __XML_THREADS_H__

#include <libxml/xmlversion.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * xmlMutex are a simple mutual exception locks.
 */
typedef struct _xmlMutex xmlMutex;
typedef xmlMutex *xmlMutexPtr;

/*
 * xmlRMutex are reentrant mutual exception locks.
 */
typedef struct _xmlRMutex xmlRMutex;
typedef xmlRMutex *xmlRMutexPtr;

#ifdef __cplusplus
}
#endif
#include <libxml/globals.h>
#ifdef __cplusplus
extern "C" {
#endif
XMLPUBFUN xmlMutexPtr XMLCALL
			xmlNewMutex	(void);
XMLPUBFUN void XMLCALL
			xmlMutexLock	(xmlMutexPtr tok);
XMLPUBFUN void XMLCALL
			xmlMutexUnlock	(xmlMutexPtr tok);
XMLPUBFUN void XMLCALL
			xmlFreeMutex	(xmlMutexPtr tok);

XMLPUBFUN xmlRMutexPtr XMLCALL
			xmlNewRMutex	(void);
XMLPUBFUN void XMLCALL
			xmlRMutexLock	(xmlRMutexPtr tok);
XMLPUBFUN void XMLCALL
			xmlRMutexUnlock	(xmlRMutexPtr tok);
XMLPUBFUN void XMLCALL
			xmlFreeRMutex	(xmlRMutexPtr tok);

/*
 * Library wide APIs.
 */
XMLPUBFUN void XMLCALL
			xmlInitThreads	(void);
XMLPUBFUN void XMLCALL
			xmlLockLibrary	(void);
XMLPUBFUN void XMLCALL
			xmlUnlockLibrary(void);
XMLPUBFUN int XMLCALL
			xmlGetThreadId	(void);
XMLPUBFUN int XMLCALL
			xmlIsMainThread	(void);
XMLPUBFUN void XMLCALL
			xmlCleanupThreads(void);
XMLPUBFUN xmlGlobalStatePtr XMLCALL
			xmlGetGlobalState(void);

#if defined(HAVE_WIN32_THREADS) && !defined(HAVE_COMPILER_TLS) && defined(LIBXML_STATIC_FOR_DLL)
int XMLCALL xmlDllMain(void *hinstDLL, unsigned long fdwReason, void *lpvReserved);
#endif

#ifdef __cplusplus
}
#endif


#endif /* __XML_THREADS_H__ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/xml2/public/libxml/threads.h $ $Rev: 680336 $")
#endif
