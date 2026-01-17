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

// Native API Imports
__declspec(dllimport) NTSTATUS __stdcall NtDisplayString(PUNICODE_STRING String);
__declspec(dllimport) NTSTATUS __stdcall NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
__declspec(dllimport) void __stdcall RtlInitUnicodeString(PUNICODE_STRING DestinationString, const WCHAR* SourceString);

// Fixed-point math: Converts cycles to a "00.00 MHz" string
void FormatMhzDecimal(UINT64 cycles, WCHAR* out_buf) {
    // Math logic:
    // If we sample for roughly 1 second:
    // MHz = Cycles / 1,000,000
    // To get 2 decimals, we calculate (Cycles / 10,000)
    UINT64 mhz_x100 = cycles / 10000; 
    UINT64 whole = mhz_x100 / 100;
    UINT64 decimal = mhz_x100 % 100;

    int pos = 0;
    WCHAR temp[20];
    int i = 0;

    // Convert Whole number
    if (whole == 0) temp[i++] = L'0';
    while (whole > 0) { temp[i++] = (whole % 10) + L'0'; whole /= 10; }
    while (i > 0) out_buf[pos++] = temp[--i];

    out_buf[pos++] = L'.'; // Decimal dot

    // Convert Decimal (padded to 2 digits)
    out_buf[pos++] = (decimal / 10) + L'0';
    out_buf[pos++] = (decimal % 10) + L'0';

    // Labels
    out_buf[pos++] = L' '; out_buf[pos++] = L'M'; out_buf[pos++] = L'H'; out_buf[pos++] = L'z';
    out_buf[pos++] = L'\n'; // New line for a scrolling log
    out_buf[pos++] = L'\0';
}

void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    UINT64 start, end, diff;
    WCHAR display_buf[64];
    
    RtlInitUnicodeString(&msg, L"--- NATIVE MHZ SCROLLING LOG ---\n");
    NtDisplayString(&msg);
    RtlInitUnicodeString(&msg, L"Change DOSBox-X cycles now (F11/F12)...\n\n");
    NtDisplayString(&msg);

    // Run for 50 updates
    for(int i = 0; i < 50; i++) {
        start = __rdtsc();
        
        // This loop controls the "update rate". 
        // In DOSBox-X, 100 million iterations is roughly 1 second at 100MHz.
        for (volatile int d = 0; d < 100000000; d++); 
        
        end = __rdtsc();
        diff = end - start;
        
        FormatMhzDecimal(diff, display_buf);
        
        RtlInitUnicodeString(&msg, display_buf);
        NtDisplayString(&msg);
    }

    RtlInitUnicodeString(&msg, L"\nLog finished. Continuing boot...");
    NtDisplayString(&msg);
    
    // Final wait so you can see the last line
    for (volatile int d = 0; d < 200000000; d++);
    NtTerminateProcess((HANDLE)-1, 0);
}
