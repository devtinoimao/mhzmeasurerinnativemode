#include <ntddk.h> // From the Windows Driver Kit (WDK)

// Define the function prototype for the screen output
NTSYSAPI NTSTATUS NTAPI NtDisplayString(PUNICODE_STRING String);
NTSYSAPI NTSTATUS NTAPI NtDelayExecution(BOOLEAN Alertable, PLARGE_INTEGER DelayInterval);

void NtProcessStartup(PVOID StartupArgument) {
    UNICODE_STRING msg;
    LARGE_INTEGER delay;
    unsigned __int64 start_tsc, end_tsc;
    WCHAR buffer[128];

    // 1. Get initial timestamp
    start_tsc = __rdtsc();

    // 2. Wait for 1 second (10,000,000 hundreds of nanoseconds)
    // Negative value means relative time
    delay.QuadPart = -10000000; 
    NtDelayExecution(FALSE, &delay);

    // 3. Get end timestamp
    end_tsc = __rdtsc();

    // 4. Calculate MHz (roughly)
    unsigned __int64 mhz = (end_tsc - start_tsc) / 1000000;

    // 5. Format and display
    swprintf(buffer, L"CPU Speed: %llu MHz\nExiting in 5 seconds...", mhz);
    RtlInitUnicodeString(&msg, buffer);
    NtDisplayString(&msg);

    // Wait 5 seconds so you can actually read the screen
    delay.QuadPart = -50000000;
    NtDelayExecution(FALSE, &delay);

    NtTerminateProcess(NtCurrentProcess(), 0);
}
