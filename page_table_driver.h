#ifndef _PAGE_TABLE_DRIVER_H
#define _PAGE_TABLE_DRIVER_H

#include <linux/ioctl.h>

#define IOC_MAGIC '\xFF'
#define IOCTL_REQUEST _IOWR(IOC_MAGIC, 0, struct ioctl_arg)

#define IO_CR3      0
#define IO_READ     1
#define IO_WRITE    2

struct ioctl_arg
{
	uint64_t ret, request[3];
};

#endif
