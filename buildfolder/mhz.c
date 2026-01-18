#include <intrin.h> // For __rdtsc

typedef void* PVOID;
typedef void* HANDLE;
typedef long NTSTATUS;
typedef unsigned short WCHAR;
typedef unsigned long ULONG;
typedef unsigned __int64 UINT64;

typedef struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// ntdll.dll imports
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const WCHAR* SourceString);

void FormatMhz(UINT64 cycles, WCHAR* out_buf) {
    UINT64 mhz_x100 = cycles / 10000; 
    UINT64 whole = mhz_x100 / 100;
    UINT64 decimal = mhz_x100 % 100;

    int pos = 0;
    WCHAR temp[20];
    int i = 0;

    if (whole == 0) temp[i++] = L'0';
    while (whole > 0) { temp[i++] = (whole % 10) + L'0'; whole /= 10; }
    while (i > 0) out_buf[pos++] = temp[--i];

    out_buf[pos++] = L'.';
    out_buf[pos++] = (decimal / 10) + L'0';
    out_buf[pos++] = (decimal % 10) + L'0';
    out_buf[pos++] = L' '; out_buf[pos++] = L'M'; out_buf[pos++] = L'H'; out_buf[pos++] = L'z';
    out_buf[pos++] = L'\n';
    out_buf[pos++] = L'\0';
}

// Ensure the entry point is decorated correctly for x86
void __stdcall NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end;
    WCHAR display_buf[64];
    
    RtlInitUnicodeString(&msg, L"--- LIVE MHZ MONITOR ---\n");
    NtDisplayString(&msg);

    for(int i = 0; i < 50; i++) {
        start = __rdtsc();
        for (volatile int d = 0; d < 50000000; d++); 
        end = __rdtsc();

        FormatMhz(end - start, display_buf);
        RtlInitUnicodeString(&msg, display_buf);
        NtDisplayString(&msg);
    }

    NtTerminateProcess((HANDLE)-1, 0);
}
