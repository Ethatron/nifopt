/* file: havokc.c */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "nifopt-hvk.h"
#include <windows.h>

unsigned char * pszUuid             = NULL;
unsigned char * pszProtocolSequence = (unsigned char *)"ncacn_np";
unsigned char * pszNetworkAddress   = NULL;
unsigned char * pszEndpoint         = (unsigned char *)"\\pipe\\havok";
unsigned char * pszOptions          = NULL;
unsigned char * pszStringBinding    = NULL;
unsigned char * pszString           = (unsigned char *)"havok, world";

bool HavokClientInit()
{
    RPC_STATUS status;
    unsigned long ulCode;

    status = RpcStringBindingCompose(pszUuid,
                                     pszProtocolSequence,
                                     pszNetworkAddress,
                                     pszEndpoint,
                                     pszOptions,
                                     &pszStringBinding);
    if (status)
      return false;

    status = RpcBindingFromStringBinding(pszStringBinding, &havok_IfHandle);

    if (status)
      return false;

#if 0
    RpcTryExcept
    {
        HelloProc(pszString);
        Shutdown();
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept
#endif

    return true;
}

bool HavokClientExit() {
    RPC_STATUS status;

    status = RpcStringFree(&pszStringBinding);

    if (status)
      return false;

    status = RpcBindingFree(&havok_IfHandle);

    if (status)
      return false;

    return true;
}

/******************************************************/
/*         MIDL allocate and free                     */
/******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}