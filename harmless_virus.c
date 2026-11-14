#include <stdio.h>
#include <string.h>

/* Declare the Elbrus assembly function */
extern void write_to_file_asm(const char* filename, const char* data);

void open_w(const char *path, const char *data) {
    FILE *f = fopen(path, "w");
    if (f) {
        fputs(data, f);
        fclose(f);
    }
}

int main(void) {
    char filename[256];
    const char* message = "Hello from the Elbrus E2K assembly routine!\n";

    printf("Enter filename to write to: ");
    if (!fgets(filename, sizeof(filename), stdin)) {
        fprintf(stderr, "Failed to read filename.\n");
        return 1;
    }
    /* Strip the trailing newline fgets() leaves in, if present. */
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    printf("Attempting to write to %s...\n", filename);

    /* Call the assembly function */
    write_to_file_asm(filename, message);

    /* Also exercise the plain C open_w helper on the same file */
    open_w(filename, message);

    printf("Done. Check the directory for the file.\n");
    return 0;
}
