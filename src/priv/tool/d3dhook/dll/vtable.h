#pragma once

#include <garnet/GNbase.h>

#define HOOK_ERROR_LOG(...)   GN_ERROR(GN::getLogger("GN.d3dhook"))(__VA_ARGS__)
#define HOOK_WARN_LOG(...)    GN_WARN(GN::getLogger("GN.d3dhook"))(__VA_ARGS__)
#define HOOK_INFO_LOG(...)    GN_INFO(GN::getLogger("GN.d3dhook"))(__VA_ARGS__)
#define HOOK_VERBOSE_LOG(...) GN_VERBOSE(GN::getLogger("GN.d3dhook"))(__VA_ARGS__)
#define HOOK_ASSERT(x)        GN_ASSERT(x)
#define HOOK_RIP()            GN_UNEXPECTED()

#undef  BEGIN_INTERFACE
#undef  END_INTERFACE
#define BEGIN_INTERFACE  union { struct {
#define END_INTERFACE    }; void * method0; };

// -----------------------------------------------------------------------------
#define VTABLE_MAX_COUNT 16
template<typename T>
struct VTable
{
    T    tables[VTABLE_MAX_COUNT];
    UINT count;
    VTable() : count(0)
    {
    }
};

template<SIZE_T SIZE>
struct UpdatePageProtection
{
    void * address;
    HANDLE process;
    DWORD  oldProtection;
    bool   succeeded;
    const char * interfaceName;

    UpdatePageProtection(void * addr_, const char * interfaceName_)
        : address(addr_)
        , succeeded(false)
        , interfaceName(interfaceName_)
    {
        process = ::GetCurrentProcess();
        if (::VirtualProtectEx( process, addr_, SIZE, PAGE_READWRITE, &oldProtection ))
        {
            succeeded = true;
        }
        else
        {
            HOOK_ERROR_LOG("Failed to update %s vtable: changing page protection failed.", interfaceName);
        }
    }

    ~UpdatePageProtection()
    {
        if (succeeded && !::VirtualProtectEx( process, address, SIZE, oldProtection, &oldProtection ))
        {
            HOOK_ERROR_LOG("Failed to restore %s vtable page protection.", interfaceName);
        }
    }
};

// -----------------------------------------------------------------------------
template<typename VTABLE_STRUCT>
inline void RealToHooked_General(
    VTABLE_STRUCT &         vtable,
    VTable<VTABLE_STRUCT> & origin,
    VTable<VTABLE_STRUCT> & hooked,
    const char *            interfaceName)
{
    UpdatePageProtection<sizeof(VTABLE_STRUCT)> upp(&vtable, interfaceName);
    if (!upp.succeeded) return;

    for (UINT t = 0; t < origin.count; ++t)
    {
        VTABLE_STRUCT & o = origin.tables[t];
        VTABLE_STRUCT & h = hooked.tables[t];
        if (0 == memcmp(&vtable, &o, sizeof(vtable)))
        {
            vtable = h;
            return;
        }
        if (0 == memcmp(&vtable, &h, sizeof(vtable)))
        {
            // already hooked
            return;
        }
    }

    // This is a new vtable for this class that we never seen before. Need to remember it.
    HOOK_INFO_LOG("New vtable (%d) for interface: %s", origin.count+1, interfaceName);

    if (origin.count >= VTABLE_MAX_COUNT)
    {
        HOOK_ERROR_LOG("Too many vtables for class %s", interfaceName);
        return;
    }

    VTABLE_STRUCT & o = origin.tables[origin.count];
    VTABLE_STRUCT & h = hooked.tables[origin.count];
    o = vtable;
    vtable = h;
    ++origin.count;
}

// -----------------------------------------------------------------------------
// common call trace utilities
// -----------------------------------------------------------------------------

namespace calltrace
{
    /// Global switch to turn on/off call tracking
    extern bool g_callTraceEnabled;

    /// return call level (including current call)
    int enter(const wchar_t * text);

    /// return call level (including current call)
    int enter(const char * text);

    void leave();

    /// auotmatic call trace helper
    class AutoTrace
    {
        bool _enabled;

    public:

        AutoTrace(const wchar_t * text) : _enabled(g_callTraceEnabled)
        {
            if (_enabled)
            {
                enter(text);
            }
        }

        AutoTrace(const char * text) : _enabled(g_callTraceEnabled)
        {
            if (_enabled)
            {
                enter(text);
            }
        }

        ~AutoTrace()
        {
            if (_enabled)
            {
                leave();
            }
        }
    };
};
