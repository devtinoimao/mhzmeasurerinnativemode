// Define types manually to be 100% independent
typedef void* PVOID;
typedef void* HANDLE;
typedef long NTSTATUS;
typedef unsigned __int64 UINT64;
typedef unsigned short WCHAR; // Manual wchar_t replacement

typedef struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// ntdll.dll imports
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const WCHAR* SourceString);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);

// Entry point
void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end;
    
    // We use L"text" which creates WCHAR (unsigned short) arrays
    RtlInitUnicodeString(&msg, L"Native MHz Test Starting...\n");
    NtDisplayString(&msg);

    for(int i = 0; i < 50; i++) {
        start = __rdtsc();
        for (volatile int d = 0; d < 10000000; d++); 
        end = __rdtsc();

        // Print a dot for each measurement to show activity
        RtlInitUnicodeString(&msg, L".");
        NtDisplayString(&msg);
    }

    RtlInitUnicodeString(&msg, L"\nDone! Returning to Shell.\n");
    NtDisplayString(&msg);

    NtTerminateProcess((HANDLE)-1, 0);
}
