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

// ntdll imports
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const WCHAR* SourceString);

// Simple Integer to WCHAR conversion (since we can't use swprintf)
void IntToWStr(UINT64 n, WCHAR* str) {
    WCHAR temp[20];
    int i = 0, j = 0;
    if (n == 0) temp[i++] = L'0';
    while (n > 0) {
        temp[i++] = (n % 10) + L'0';
        n /= 10;
    }
    while (i > 0) str[j++] = temp[--i];
    str[j] = L'\0';
}

// Entry point for Native Subsystem
void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end, mhz;
    WCHAR mhz_buf[64];
    WCHAR final_msg[128];
    
    // 1. Initial Greeting
    RtlInitUnicodeString(&msg, L"\nXP NATIVE FREQUENCY MONITOR\n---------------------------\n");
    NtDisplayString(&msg);

    // 2. The Measurement Loop
    // We will run this in a loop so it keeps updating until you "stop" it
    for(int loops = 0; loops < 10; loops++) {
        start = __rdtsc();
        // Delay loop for approx 1 second
        for (volatile int i = 0; i < 200000000; i++); 
        end = __rdtsc();

        mhz = (end - start) / 1000000;

        // Convert and Display
        IntToWStr(mhz, mhz_buf);
        
        // Manual string concatenation for the display
        RtlInitUnicodeString(&msg, L"Current Speed: ");
        NtDisplayString(&msg);
        RtlInitUnicodeString(&msg, mhz_buf);
        NtDisplayString(&msg);
        RtlInitUnicodeString(&msg, L" MHz\n");
        NtDisplayString(&msg);
    }

    // 3. Exit sequence
    RtlInitUnicodeString(&msg, L"\nMeasurement finished. Returning to Windows XP...\n");
    NtDisplayString(&msg);

    // Final short pause so you can see the last result
    for (volatile int i = 0; i < 300000000; i++);

    NtTerminateProcess((HANDLE)-1, 0);
}
