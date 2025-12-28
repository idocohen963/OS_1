/**
 * @file PB2add.cpp
 * @brief Exercise 7 - Add entry to phonebook (companion program to findPhone)
 * 
 * @details This program adds a new entry to the phonebook.txt file.
 *          -  file I/O using open(2) and write(2)
 *          - Manual string manipulation without string APIs
 *          - Input validation for security
 * 
 * @note Format: "Full Name,Phone-Number\n"
 *       Example: "Nezer Zaidenberg,054-5531415\n"
 */
#include <iostream>
#include <fstream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

/**
 * @brief Validates that a name contains only letters, spaces, and apostrophes
 * 
 * @param name The name string to validate
 * @return true if name is valid, false otherwise
 */
bool is_valid_name(const char* name) {
    for (int i = 0; name[i] != '\0'; ++i) {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '\'') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validates that a phone number contains only digits and hyphens
 * 
 * @param phone The phone number string to validate
 * @return true if phone is valid, false otherwise
 */
bool is_valid_phone(const char* phone) {
    for (int i = 0; phone[i] != '\0'; ++i) {
        if (!isdigit(phone[i]) && phone[i] != '-') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Main function - Adds a new entry to the phonebook
 * 
 * @param argc Argument count (must be at least 3)
 * @param argv Argument vector - argv[1..n-1] = name parts, argv[n] = phone
 * @return 0 on success, 1 on error
 * 
 * @details Usage: ./PB2add <full name> <phone number>
 *          Example: ./PB2add John Doe 123-4567890
 *          
 *          The name can contain multiple words (e.g., "John Doe" or "Sheva Bat").
 *          The last argument is always treated as the phone number.
 */
int main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc < 3) {
        cerr << "Usage: ./PB2add  <full name> <phone number>\n";
        cerr << "run example: ./PB2add  John Doe 123-4567890\n";
        return 1;
    }

    /**
     * Manually concatenate name from all argv except the last one
     * The loop combines argv[1] through argv[argc-1] with spaces between them.
     */
    char name[256] = {0};  // Initialize to zeros
    int nameIndex = 0;
    
    // Iterate through all arguments except the last (which is the phone number)
    for (int i = 1; i < argc - 1; ++i) {
        int j = 0;
        // Copy each character from argv[i] to name
        while (argv[i][j] != '\0' && nameIndex < 255) {
            name[nameIndex++] = argv[i][j++];
        }
        // Add space between name parts (but not after the last part)
        if (i < argc - 2 && nameIndex < 255) {
            name[nameIndex++] = ' ';
        }
    }
    name[nameIndex] = '\0';  // Null-terminate the string

    // Validate the name (security check)
    if (!is_valid_name(name)) {
        cerr << "invalid name: Name must contain letters or spaces only\n";
        return 1;
    }

    // Extract phone number from last argument
    const char* phone = argv[argc - 1];

    // Validate the phone number (security check)
    if (!is_valid_phone(phone)) {
        cerr << "invalid phone: Phone number must contain digits or hyphens only\n";
        return 1;
    }

    int fd = open("phonebook.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    
    // Check if the file was opened successfully
     // O_WRONLY: Open for writing only
     // O_APPEND: Append to the end of the file
     // O_CREAT: Create the file if it does not exist
     // 0644: File permissions (read/write for owner, read for group and others)

    if (fd < 0) {
        cerr << "Error: failed to open the file\n";
        return 1;
    }

    char buffer[512];
    int index = 0;

    // Copy name to buffer character by character
    for (int i = 0; name[i] != '\0'; ++i) {
        buffer[index++] = name[i];
    }

    // Add comma separator (required format)
    buffer[index++] = ',';
    
    // Copy phone number to buffer character by character
    for (int i = 0; phone[i] != '\0'; ++i) {
        buffer[index++] = phone[i];
    }
    
    // Add newline character (each entry must be on its own line)
    buffer[index++] = '\n';
    buffer[index] = '\0';  // Null-terminate (for safety, not required for write)

    // Write to the file
    write(fd, buffer, index);
    
    // Close the file descriptor using close(2)
    close(fd);
    
    cout << "Added successfully to phonebook!" <<endl;
    return 0;
}