#pragma once
#include "SDK.h"

typedef void( *hookfun_t )( PCONTEXT pointers );
class Hook
{
private:
    static std::map<DWORD, hookfun_t> hooks;
public:
    static void Initialise();
    static void NewHook(DWORD address, hookfun_t hookFunction);
    static void RemoveHook( DWORD address );
    static long __stdcall ExceptionFilter( PEXCEPTION_POINTERS info );
};