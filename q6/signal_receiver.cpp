/*
 * Purpose: Receive an 8-bit number from a sender program using SIGUSR1 and SIGUSR2 signals
 * 
 * Reception Strategy:
 * - SIGUSR1 represents receiving a bit with value 0
 * - SIGUSR2 represents receiving a bit with value 1
 * - Bits are received from MSB to LSB and gradually built into a complete number
 * 
 * Global Variables Usage:
 * - Must be volatile sig_atomic_t for signal handling safety
 * - volatile - ensures the compiler doesn't perform optimizations that could cause issues
 * - sig_atomic_t - type that guarantees atomic read/write operations (without tearing)
 * 
 * Handling Critical Issues:
 * 1. Using sigaction instead of signal - more advanced and safer
 * 2. Using sa_mask to block SIGUSR1 and SIGUSR2 during handling - prevents race conditions
 * 3. Using pause() instead of sleep() - doesn't lose signals that arrive during wait time
 * ===============================================================================
 */

#include <iostream>
#include <csignal>
#include <unistd.h>


volatile sig_atomic_t bit_count = 0;  // Counter for the number of bits received (0-8)
volatile sig_atomic_t result = 0;     // The received number, built bit by bit

/*
 * Function: handle_sigusr
 * -----------------------
 * Signal Handler - automatically triggered when SIGUSR1 or SIGUSR2 is received
 * 
 * Parameters:
 *   sig - the signal number received (SIGUSR1 or SIGUSR2)
 * 
 * Number Construction Algorithm:
 * ------------------------------
 * 1. Left shift the current result - make room for the new bit
 * 2. If SIGUSR2 was received (bit=1), add 1 using OR
 * 3. Update the bit counter
 * 4. After 8 bits - print the result and exit
 */
void handle_sigusr(int sig)
{
    // Step 1: Left shift result by one position - make room for the new bit
    // Example: 00000101 << 1 = 00001010
    result <<= 1;

    // Step 2: If SIGUSR2 was received, it means the bit is 1
    // Use OR (|=) to add 1 to the rightmost bit
    // Example: 00001010 | 1 = 00001011
    if (sig == SIGUSR2)
    {
        result |= 1;
    }
    // If SIGUSR1 was received, the bit is 0 - no need to do anything (already 0)

    // Step 3: Increment the bit counter
    bit_count++;

    // Step 4: Check if we received all 8 bits
    if (bit_count == 8)
    {
        // Print the constructed number
        std::cout << "Received " << result << std::endl;
        exit(0);
    }
}

/*
 * Main function - prepares the program to receive signals
 * 
 * Program Flow:
 * 1. Print the PID of the current process (so the sender can identify us)
 * 2. Set up an advanced signal handler with sigaction
 * 3. Wait indefinitely for signals with pause()
 */
int main()
{
    // Print the process PID - the user will enter this in the sender program
    std::cout << "My PID is " << getpid() << std::endl;

    /* 
     * Why sigaction and not signal?
     * ------------------------------
     * 1. sigaction provides more control and safety
     * 2. Allows defining a mask - blocking signals during handling
     * 3. Consistent behavior across different operating systems
     * 4. signal is old and less reliable (deprecated in most modern standards)
     */
    struct sigaction sa;
    
    // Set the function that will handle the signals
    sa.sa_handler = handle_sigusr;

    /*
     * Setting Signal Mask - list of signals to block during handling
     * When the handler runs, the system blocks SIGUSR1 and SIGUSR2.
     * If a signal arrives during this time, it waits in queue and will be handled only after the current handler finishes.
     */
    sigemptyset(&sa.sa_mask);        // Empty the mask (start with an empty list)
    sigaddset(&sa.sa_mask, SIGUSR1); // Add SIGUSR1 to the list of signals to block
    sigaddset(&sa.sa_mask, SIGUSR2); // Add SIGUSR2 to the list of signals to block

    /*
     * Every time the process receives SIGUSR1 or SIGUSR2,
     * the handle_sigusr function will be activated with the mask we defined
     */
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGUSR2, &sa, nullptr);

    /*
     * 1. pause() sleeps until a signal arrives
     * 2. Once the signal is handled, pause wakes up and is called again immediately
     */
    while (true)
    {
        pause(); // Wait for signals
    }

    return 0;
}
