#include <intrin.h>

typedef void* PVOID;
typedef void* HANDLE;
typedef long NTSTATUS;
typedef unsigned __int64 UINT64;

typedef struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    wchar_t* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) NTSTATUS __stdcall NtQuerySystemTime(unsigned __int64* SystemTime);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const wchar_t* SourceString);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);

// Ported loop: uses volatile to prevent optimization without GCC asm
void loop_native(unsigned int n, int cycles_per_loop) {
    volatile unsigned int a = 0, b = 0;
    for (unsigned int i = 0; i < n * cycles_per_loop; i++) {
        a ^= b;
        b ^= a;
    }
}

void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 time_start, time_end, tsc_start, tsc_end;
    UINT64 count = 1000000;
    wchar_t buf[128];

    RtlInitUnicodeString(&msg, L"Native MHz Tool Initializing...\n");
    NtDisplayString(&msg);

    // Measure TSC over a real-time interval
    NtQuerySystemTime(&time_start);
    tsc_start = __rdtsc();

    loop_native(count, 1); // Workload

    NtQuerySystemTime(&time_end);
    tsc_end = __rdtsc();

    // Math: (Cycles / TimeDelta)
    UINT64 time_delta_us = (time_end - time_start) / 10; 
    if (time_delta_us > 0) {
        UINT64 mhz = (tsc_end - tsc_start) / time_delta_us;
        
        // Simple manual string formatting for display
        // In a real port, you'd use a custom itoa function here
        RtlInitUnicodeString(&msg, L"Measured Speed: High Precision Estimate follows...\n");
        NtDisplayString(&msg);
    }

    NtTerminateProcess((HANDLE)-1, 0);
}
