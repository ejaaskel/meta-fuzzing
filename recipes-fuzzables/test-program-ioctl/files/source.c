#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_PATH "/dev/ioctl_example"
#define IOCTL_CMD_PARSE_STRING _IOW(0, 0, char *)

int main(int argc, char *argv[])
{
    int fd;
    char input_buffer[256];

    // Check if the user provided input
    if (argc != 2) {
        printf("Usage: %s <string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the device file
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Copy the user-provided string to the input buffer
    strncpy(input_buffer, argv[1], sizeof(input_buffer) - 1);
    input_buffer[sizeof(input_buffer) - 1] = '\0';  // Ensure null-termination

    // Call ioctl with the provided string
    if (ioctl(fd, IOCTL_CMD_PARSE_STRING, input_buffer) < 0) {
        perror("Failed to perform ioctl");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("ioctl call succeeded\n");

    // Close the device file
    close(fd);

    return EXIT_SUCCESS;
}

