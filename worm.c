#include <stdio.h>

// Declare the Elbrus assembly function
extern void make_executable_and_run(const char *filename);

// ==========================================
// Function 1: main
// ==========================================
int main(void) {
    // Main is now extremely simple. It only passes the target filename.
    make_executable_and_run("./hello_e2k.sh");
    return 0;
}
