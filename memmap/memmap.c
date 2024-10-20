#include <fcntl.h>
#ifdef __linux__
#include <sys/mman.h>
#endif
#ifdef __windows__
#include <windows.h>
#endif
#include <unistd.h>
#include <stdio.h>

#ifdef __linux__
unsigned char* memmap_signal_linux(char* filename, int size) {
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    ftruncate(fd, size);
    

    unsigned char *mapped = (unsigned char *) mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    // munmap(mapped, size);
    // close(fd);

    return mapped;
}
#endif

#ifdef __windows__
unsigned char* memmap_signal_windows(char* filename, int size) {
    HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, size, NULL);

    unsigned char *mapped = (unsigned char *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (mapped == NULL) {
        printf("Could not map view of file (%d).\n", GetLastError());
    }

    // UnmapViewOfFile(pBuf);
    // CloseHandle(hMapFile);
    // CloseHandle(hFile);
    return mapped;
}
#endif

unsigned char* memmap_signal(char* filename, int size) {
#ifdef __linux__
    return memmap_signal_linux(filename, size);
#endif
#ifdef __windows__
    return memmap_signal_windows(filename, size);
#endif
}