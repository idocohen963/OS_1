
/**
 * @file findPhone.cpp
 * 
 * @details This program demonstrates the use of:
 *          - fork(2): Creating child processes
 *          - execve(2): Executing external commands (grep, sed, awk)
 *          - pipe(2): Inter-process communication
 *          - dup2(2): File descriptor duplication for I/O redirection
 
 * The program searches for a phone number in phonebook.txt by:
 * 1. grep - Filtering lines containing the name
 * 2. sed  - Replacing spaces with '#' (s/ /#/g)
 * 3. sed  - Replacing comma with space (s/,/ /)
 * 4. awk  - Extracting the phone number ({print $2})
 * 
 * All operations are performed using child processes connected via pipes.
 */
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cctype>
#include <cstdlib>
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
 * @brief Main function - Creates a pipeline of child processes to search for phone numbers
 * 
 * @param argc Argument count
 * @param argv Argument vector - argv[1] should contain the name to search
 * @return 0 on success, 1 on error
 * 
 * @details Process pipeline (using fork(2) and execve(2)):
 *          Parent -> Child1(grep) -> Child2(sed) -> Child3(sed) -> Child4(awk)
 *          
 *          Communication via pipe(2):
 *          - pipe1: grep -> sed1
 *          - pipe2: sed1 -> sed2
 *          - pipe3: sed2 -> awk
 *          
 *          Each child process uses dup2(2) to redirect stdin/stdout to pipes.
 */
int main(int argc, char *argv[]) {
    // Validate command line arguments
    if (argc < 2) {
        cerr << "Usage: ./findPhone <first name>\n";
        return 1;
    }
    cout << "Notice:the program use the first name only (argv[1]) " << endl;

    char* name = argv[1];
    
    // Validate input to prevent command 
    if (!is_valid_name(name)) {
        cerr << "invalid name: Name must contain letters or spaces only\n";
        return 1;
    }

    // Array to store PIDs of 4 child processes
    // This is essential for the exercise - demonstrating fork(2) usage
    pid_t pidArr[4];

    /**
     * Create 3 pipes using pipe(2) for inter-process communication
     * This is a KEY requirement of the exercise - using pipe(2)
     * 
     * pipe1[0] = read end,  pipe1[1] = write 
     */
    int pipe1[2], pipe2[2], pipe3[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("pipe failed");
        return 1;
    }

    //CHILD PROCESS 1: grep command
    
    pidArr[0] = fork();  // fork(2) - Create child process
    if (pidArr[0] == -1) {
        perror("fork failed before grep");
        return 1;
    } else if (pidArr[0] == 0) {
        // Child process - searches for name in phonebook.txt
        
        // dup2(2) - Redirect stdout to pipe1 write end
        // This connects grep's output to sed1's input
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);  // Close unused read end
        close(pipe1[1]);  // Close original write end (already duplicated)

        // Close all other pipes - child doesn't need them
        // Good practice: close unused file descriptors
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        // execlp(3) - Execute grep command (uses execve(2) internally)
        // This is a KEY requirement - using execve to run external commands
        // After exec, this process becomes grep
        execlp("grep", "grep", name, "phonebook.txt", NULL);
        perror("execlp grep failed");
        exit(1);
    }

    /**
     * CHILD PROCESS 2: sed command (replace spaces with #)
     */
    pidArr[1] = fork();  // fork(2) - Create second child process
    if (pidArr[1] == -1) {
        perror("fork failed before sed1");
        return 1;
    } else if (pidArr[1] == 0) {
        // Child process - replaces spaces with '#' using sed
        
        // dup2(2) - Redirect stdin from pipe1 (reads grep output)
        dup2(pipe1[0], STDIN_FILENO);
        // dup2(2) - Redirect stdout to pipe2 (sends to next sed)
        dup2(pipe2[1], STDOUT_FILENO);
        
        // Close all pipe file descriptors after duplication
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        // execlp(3) - Execute sed command (uses execve(2) internally)
        // Replaces all spaces with '#' (s/ /#/g)
        execlp("sed", "sed", "s/ /#/g", NULL);
        perror("execlp sed1 failed");
        exit(1);
    }

    /**
     * CHILD PROCESS 3: sed command (replace comma with space)
     */
    pidArr[2] = fork();  // fork(2) - Create third child process
    if (pidArr[2] == -1) {
        perror("fork failed before sed2");
        return 1;
    } else if (pidArr[2] == 0) {
        // Child process - replaces comma with space using sed
        
        // dup2(2) - Redirect stdin from pipe2 (reads first sed output)
        dup2(pipe2[0], STDIN_FILENO);
        // dup2(2) - Redirect stdout to pipe3 (sends to awk)
        dup2(pipe3[1], STDOUT_FILENO);
        
        // Close all pipe file descriptors
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        // execlp(3) - Execute sed command (uses execve(2) internally)
        // Replaces comma with space (s/,/ /)
        execlp("sed", "sed", "s/,/ /", NULL);
        perror("execlp sed2 failed");
        exit(1);
    }

    /**
     * CHILD PROCESS 4: awk command (extract phone number)
     */
    pidArr[3] = fork();  // fork(2) - Create fourth child process
    if (pidArr[3] == -1) {
        perror("fork failed before awk");
        return 1;
    } else if (pidArr[3] == 0) {
        // Child process - extracts second field (phone number) using awk
        
        // dup2(2) - Redirect stdin from pipe3 (reads second sed output)
        // stdout remains connected to terminal (prints result to user)
        dup2(pipe3[0], STDIN_FILENO);
        
        // Close all pipe file descriptors
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        close(pipe3[0]); close(pipe3[1]);

        // execlp(3) - Execute awk command (uses execve(2) internally)
        // Prints the second field ($2) which is the phone number
        execlp("awk", "awk", "{print $2}", NULL);
        perror("execlp awk failed");
        exit(1);
    }

    /**
     * PARENT PROCESS close all pipe 
     */
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);
    close(pipe3[0]); close(pipe3[1]);

    /**
     * Wait for all child processes to complete
     * 
     * Uses waitpid(2) to prevent zombie processes.
     * The parent must wait for all 4 children created with fork(2).
     */
    cout << "The phone number/s: " << endl;
    for (int i = 0; i < 4; ++i) {
        waitpid(pidArr[i], NULL, 0);  // Wait for each child to terminate
    }

    cout << "The program has finished " << endl;

    return 0;
}

