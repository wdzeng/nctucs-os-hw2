#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "page_table_driver.h"

int fd;

// Obtain my cr3 value (a.k.a. PML4 table physical address)
uint64_t get_cr3_value() {
    struct ioctl_arg cmd;
    int ret;
    cmd.request[0] = IO_CR3;
    ret = ioctl(fd, IOCTL_REQUEST, &cmd);
    return cmd.ret;
}

// Given a physical address, return the value
uint64_t read_physical_address(uint64_t physical_address) {
    struct ioctl_arg cmd;
    int ret;
    cmd.request[0] = IO_READ;
    cmd.request[1] = physical_address;
    ret = ioctl(fd, IOCTL_REQUEST, &cmd);
    return cmd.ret;
}

// Write value to a physical address
void write_physical_address(uint64_t physical_address, uint64_t value) {
    struct ioctl_arg cmd;
    int ret;
    cmd.request[0] = IO_WRITE;
    cmd.request[1] = physical_address;
    cmd.request[2] = value;
    ret = ioctl(fd, IOCTL_REQUEST, &cmd);
}

uint64_t cut(uint64_t v, int h, int l) {
    v >>= l;
    uint64_t mask = 1;
    mask <<= (h - l + 1);
    mask--;
    return v & mask;
}

uint64_t addrpml4(uint64_t a) {
    a = cut(a, 47, 39);
    a <<= 3;
    uint64_t cr3 = get_cr3_value();
    cr3 >>= 12;
    cr3 <<= 12;
    cr3 |= (a & 0xfff);
    return cr3 & 0xfffffffffull;
}

uint64_t addrpdpt(uint64_t a) {
    uint64_t pml4 = read_physical_address(addrpml4(a));
    pml4 >>= 12;
    pml4 <<= 12;
    a = cut(a, 38, 30);
    a <<= 3;
    pml4 |= (a & 0xfff);
    return pml4 & 0xfffffffffull;
}

uint64_t addrpd(uint64_t a) {
    uint64_t pdpt = read_physical_address(addrpdpt(a));
    pdpt >>= 12;
    pdpt <<= 12;
    a = cut(a, 29, 21);
    a <<= 3;
    pdpt |= (a & 0xfff);
    return pdpt & 0xfffffffffull;
}

uint64_t addrpt(uint64_t a) {
    uint64_t pd = read_physical_address(addrpd(a));
    pd >>= 12;
    pd <<= 12;
    a = cut(a, 20, 12);
    a <<= 3;
    pd |= (a & 0xfff);
    return pd & 0xfffffffffull;
}

int main() {
    char *x = (char *)aligned_alloc(4096, 4096) + 0x123;
    char *y = (char *)aligned_alloc(4096, 4096) + 0x123;
    strcpy(x, "This is a simple HW.");
    strcpy(y, "You have to modify my page table.");

    fd = open("/dev/os", O_RDONLY);
    if (fd < 0) {
        printf("Cannot open device!\n");
        return 0;
    }

    printf("Before\n");
    printf("x : %s\n", x);
    printf("y : %s\n", y);

    /* TODO 1 */
    // ------------------------------------------------
    // Modify page table entry of y
    uint64_t yAddrPt = addrpt((uint64_t)y);
    uint64_t oldYValPt = read_physical_address(yAddrPt);
    uint64_t xAddrPt = addrpt((uint64_t)x);
    uint64_t xValPt = read_physical_address(xAddrPt);
    write_physical_address(yAddrPt, xValPt);

    // Let y point to x's physical address
    // ------------------------------------------------

    getchar();

    printf("After modifying page table\n");
    printf("x : %s\n", x);
    printf("y : %s\n", y);

    getchar();

    strcpy(y, "When you modify y, x is modified actually.");
    printf("After modifying string y\n");
    printf("x : %s\n", x);
    printf("y : %s\n", y);

    /* TODO 2 */
    // ------------------------------------------------
    // Recover page table entry of y
    // Let y point to its original address
    // You may need to store y's original address at previous step
    write_physical_address(yAddrPt, oldYValPt);
    // ------------------------------------------------

    getchar();

    printf("After recovering page table of y\n");
    printf("x : %s\n", x);
    printf("y : %s\n", y);

    close(fd);
}
