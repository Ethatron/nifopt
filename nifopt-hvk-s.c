/* file: havoks.c */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "nifopt-hvk.h"
#include <windows.h>

handle_t havok_IfHandle;

unsigned char * pszProtocolSequence = (unsigned char *)"ncacn_np";
unsigned char * pszSecurity         = NULL;
unsigned char * pszEndpoint         = (unsigned char *)"\\pipe\\havok";

bool HavokServer()
{
    RPC_STATUS status;
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;

    /* 16MB stack per call
    status = RpcMgmtSetServerStackSize(16 * 1024 * 1024);

    if (status)
      return false; */

    status = RpcServerUseProtseqEp(pszProtocolSequence,
                                   RPC_C_LISTEN_MAX_CALLS_DEFAULT,
                                   pszEndpoint,
                                   pszSecurity);

    if (status)
      return false;

    status = RpcServerRegisterIf(havok_v1_0_s_ifspec,
                                 NULL,
                                 NULL);

    if (status)
      return false;

    status = RpcServerListen(cMinCalls,
                             RPC_C_LISTEN_MAX_CALLS_DEFAULT,
                             fDontWait);

    if (status)
      return false;

    return true;
}

/* add this function to hellop.c */
void HavokShutdown(void)
{
    RPC_STATUS status;

    status = RpcMgmtStopServerListening(NULL);

    if (status)
      return;

    status = RpcServerUnregisterIf(NULL, NULL, FALSE);

    if (status)
      return;

    return;
} //end Shutdown

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