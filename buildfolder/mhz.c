// No #includes allowed to be safe
typedef void* PVOID;
typedef void* HANDLE;
typedef long NTSTATUS;
typedef unsigned __int64 UINT64;

typedef struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    wchar_t* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// Manual imports from ntdll.dll
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const wchar_t* SourceString);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);

// Assembly-free RDTSC for MSVC
#define rdtsc __rdtsc

void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end;
    
    RtlInitUnicodeString(&msg, L"Testing MHz... (Press F12 in DOSBox-X)\n");
    NtDisplayString(&msg);

    for(int i = 0; i < 100; i++) {
        start = rdtsc();
        // A simple workload that won't be optimized away
        for (volatile int d = 0; d < 10000000; d++); 
        end = rdtsc();

        // For now, just print a dot to prove it works
        // Complex math/formatting often requires CRT libraries which fail here
        RtlInitUnicodeString(&msg, L".");
        NtDisplayString(&msg);
    }

    RtlInitUnicodeString(&msg, L"\nDone!\n");
    NtDisplayString(&msg);

    NtTerminateProcess((HANDLE)-1, 0);
}
