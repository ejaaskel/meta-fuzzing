#ifndef _IOCTL_STRING_PARSE_H
#define _IOCTL_STRING_PARSE_H

#define DEVICE_NAME "ioctl_example"
#define CLASS_NAME "ioctl_class"
#define IOCTL_CMD_PARSE_STRING _IOW(0, 0, char *)
#define POOL_SIZE 10
#define BUFFER_SIZE 256

#endif
