// Basic types for the compiler
typedef void* PVOID;
typedef void* HANDLE;
typedef long NTSTATUS;
typedef unsigned short WCHAR;
typedef unsigned long ULONG;
typedef unsigned __int64 UINT64;

// The structure Windows uses for strings in Native Mode
typedef struct _UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// Function prototypes - linked from ntdll.lib
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const WCHAR* SourceString);

// Simple loop-based delay for the measurement window
void SimpleDelay() {
    for (volatile int i = 0; i < 100000000; i++); 
}

// The Entry Point (Replaces main)
void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end;
    
    // 1. Greet the user
    RtlInitUnicodeString(&msg, L"GEMINI NATIVE MHZ MEASURER\nStarting calibration...\n");
    NtDisplayString(&msg);

    // 2. Measure Cycles
    start = __rdtsc();
    SimpleDelay();
    end = __rdtsc();
    
    // 3. Simple logic to show we did something
    // (Note: Formatting integers to strings in Native mode usually requires 
    // custom code, so we'll print a success message first)
    if (end > start) {
        RtlInitUnicodeString(&msg, L"Measurement Complete. CPU is alive!\nExiting to Windows...");
        NtDisplayString(&msg);
    }

    // 4. Give the user 3 seconds to read before Windows takes over
    for (volatile int i = 0; i < 500000000; i++);

    // 5. Hand control back to the Session Manager
    NtTerminateProcess((HANDLE)-1, 0);
}
