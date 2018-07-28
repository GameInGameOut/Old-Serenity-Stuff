#include "Hooking.h"
#include "SDK.h"

std::map<DWORD, hookfun_t> Hook::hooks = std::map<DWORD, hookfun_t>( );
 
void Hook::Initialise( )
{
    AddVectoredExceptionHandler( 1, &Hook::ExceptionFilter );
}
 
void Hook::NewHook( DWORD address, hookfun_t hookFunction )
{
    hooks[address] = hookFunction;
    DWORD old;
    VirtualProtect( (LPVOID)address, 1, PAGE_NOACCESS, &old );
}
 
void Hook::RemoveHook( DWORD address )
{
    hooks[address] = NULL;
    DWORD old;
    VirtualProtect( (LPVOID)address, 1, PAGE_EXECUTE, &old );
}
 
DWORD lastUnHookedLocation;
long __stdcall Hook::ExceptionFilter( PEXCEPTION_POINTERS info )
{
    DWORD old;
 
    if( info->ExceptionRecord->ExceptionCode != STATUS_ACCESS_VIOLATION && info->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP )
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
 
    if( info->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP )
    {
        VirtualProtect( (LPVOID)lastUnHookedLocation, 1, PAGE_NOACCESS, &old );
        return EXCEPTION_CONTINUE_EXECUTION; 
    }
 
    std::map<DWORD, hookfun_t>::iterator it = hooks.find( info->ContextRecord->Eip );
    if( it != hooks.end( ) )
    {
        ( *it ).second( info->ContextRecord );
    }
    
    lastUnHookedLocation = info->ContextRecord->Eip; 
    VirtualProtect( (LPVOID)info->ContextRecord->Eip, 1, PAGE_EXECUTE, &old );
    info->ContextRecord->EFlags |= 0x100;
    return EXCEPTION_CONTINUE_EXECUTION;
}