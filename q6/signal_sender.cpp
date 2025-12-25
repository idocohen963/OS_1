/*
 * Signal Sender Program

 * Purpose: Send an 8-bit number to a receiver program using SIGUSR1 and SIGUSR2 signals
 * 
 * Communication Strategy:
 * - SIGUSR1 represents a bit with value 0
 * - SIGUSR2 represents a bit with value 1
 * - Bits are sent from MSB (Most Significant Bit) to LSB (Least Significant Bit)
 * 
 * Handling Communication Issues:
 * 1. Preventing signal loss - using usleep(100000) ensures a 100ms delay between
 *    each two signals, allowing the receiver to process each signal before receiving the next
 * 2. No signal queue - if sent too quickly, signals may be lost
 */

#include <iostream>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <limits>

/*
 * Function: send_bit
 * ------------------
 * Sends a single bit to the receiver program using signals
 * 
 * Parameters:
 *   pid - Process ID of the receiver program
 *   bit - The value of the bit to send (0 or 1)
 * 
 * Algorithm:
 *   1. Choose the appropriate signal: SIGUSR1 for 0, SIGUSR2 for 1
 *   2. Send the signal using the kill() system call
 *   3. Check if the sending succeeded - if not, print error and exit
 *   4. Wait 100ms (100,000 microseconds) before returning
 * 
 * Important note: The 100ms delay is critical to prevent signal loss!
 *                 It gives the receiver enough time to process each signal before receiving the next one.
 */
void send_bit(pid_t pid, int bit)
{
    // Choose the appropriate signal according to the bit value
    // SIGUSR1 = 0, SIGUSR2 = 1
    int sig = (bit == 0) ? SIGUSR1 : SIGUSR2;
    
    // Send the signal to the receiver process
    // kill() returns -1 in case of error
    if (kill(pid, sig) == -1)
    {
        perror("Failed to send signal");
        exit(1);
    }

    usleep(100000); // 100 milliseconds
}

/*
 * Main function - performs the complete sending process
 * 
 * Program Flow:
 * 1. Get the PID of the receiver process from the user (with validation)
 * 2. Get the number to send (0-255, corresponding to 8 bits)
 * 3. Convert the number to binary representation and send each bit separately
 */
int main()
{
    pid_t receiver_pid;  // Process ID of the receiver program
    int number;          // The number to send (must be in range 0-255 for 8 bits)

    // ===== Part 1: Get and validate PID =====
    // Loop that continues until a valid and available PID is received
    while (1)
    {
        std::cout << "Enter receiver PID: ";
        std::cin >> receiver_pid;

        // Input validation:
        // 1. cin.fail() - checks if the input is a valid number
        // 2. receiver_pid <= 0 - PID must be positive
        // 3. kill(receiver_pid, 0) - trick to check if the process exists
        //    Sending signal 0 does nothing, but returns -1 if the process doesn't exist
        if (std::cin.fail() || receiver_pid <= 0 || kill(receiver_pid, 0) == -1)
        {
            std::cin.clear();                                                   // Clear cin error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore rest of invalid input
            std::cout << "Invalid or unavailable PID." << std::endl;
            continue;  // Return to start of loop for new input
        }
        break; // Exit the loop - input is valid
    }

    // ===== Part 2: Get and validate the number to send =====
    // Loop that continues until a valid number in range 0-255 is received
    while (1)
    {
        std::cout << "Enter message: ";
        std::cin >> number;

        // Check 1: Is the input a valid integer?
        if (std::cin.fail())
        {
            std::cin.clear();                                                   // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::cout << "Invalid input. Please enter a valid number between 0 and 255." << std::endl;
            continue;
        }

        // Check 2: Are there additional characters after the number?
        // peek() returns the next character without removing it from the stream
        // If the next character is not newline, there's invalid input (e.g., "123abc")
        if (std::cin.peek() != '\n')
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number between 0 and 255." << std::endl;
            continue;
        }

        // Check 3: Is the number in the valid range for 8 bits? (0-255)
        // 8 bits can represent numbers from 0 (00000000) to 255 (11111111)
        if (number < 0 || number > 255)
        {
            std::cout << "Invalid number. Please enter a number between 0 and 255." << std::endl;
            continue;
        }

        break; // Exit the loop - input is valid
    }

    for (int i = 7; i >= 0; --i)
    {
        // Extract the i-th bit from the number (starting at 7 and ending at 0)
        int bit = (number >> i) & 1;
        
        // Send the bit to the receiver process (including 100ms delay)
        send_bit(receiver_pid, bit);
    }

    return 0;
}
