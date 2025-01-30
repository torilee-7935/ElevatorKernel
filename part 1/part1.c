// #include <unistd.h>

// int main() {
//     fork();
//     getpid();
//     getgid();
//     getppid();
//     getuid();
//     return 0;
// }


#include <unistd.h>
#include <sys/syscall.h> // For syscall numbers

int main() {
    // using the functions above to ensure they are executed using syscalls, but return 0
    syscall(SYS_getpid);
    syscall(SYS_getppid);
    syscall(SYS_getuid);
    syscall(SYS_getgid);

    return 0;
}
