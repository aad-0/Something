// #include "ioctl_wrapper.h"
#include "ioctl_wrapper.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// #include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
// #include <termbits.h>

// UART file descriptor

static int getBaud(int Baud)
{
    switch (Baud) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default: 
        return -1;
    }
}

// #if defined(IOCTL_WRAPPER_USE_AFFINITY)
// void setCpuAffinity() {
//     cpu_set_t set;
//     CPU_ZERO(&set);
//     CPU_SET(IOCTL_WRAPPER_AFFINITY_COREID, &set);
//     assert (sched_setaffinity(0, sizeof(set), &set) != -1);
//     // if (sched_setaffinity(0, sizeof(set), &set) == -1) {
//     //     perror("Failed to set CPU affinity");
//     //     exit(1);
//     // }
//     printf("Process affinity set to CPU core %d\n", IOCTL_WRAPPER_AFFINITY_COREID);
// }
// #endif // defined(IOCTL_WRAPPER_USE_AFFINITY)

// #if defined (IOCTL_WRAPPER_POLL_WRITE)
int32_t uartWaitReady(UartInstance_TypeDef * pInit) {
    struct pollfd pfd;
    pfd.fd = pInit->FileDescriptor;
    pfd.events = POLLOUT; // Check if the UART is ready for writing

    int ret = poll(&pfd, 1, 0); // Timeout for 1 second
    if (ret > 0) {
        if (pfd.revents & POLLOUT) {
            return 0; // UART is ready for writing
        }
    }
    return 1; // UART is not ready
}
// #endif 

int32_t uart_Init(UartInstance_TypeDef * pInit, const char * pDeviceName, uint32_t Baudrate, uint32_t PollTime)
{
    pInit->pDeviceName = pDeviceName;
    pInit->Baudrate = Baudrate;

    pInit->PollTime = PollTime;

    // uartConfigure (pInit);

    return 0;
}


void uartConfigure(UartInstance_TypeDef * pInit) {
    // #if defined(IOCTL_WRAPPER_USE_AFFINITY)
    // setCpuAffinity();
    // #endif // defined(IOCTL_WRAPPER_USE_AFFINITY)

    pInit->FileDescriptor = open(pInit->pDeviceName, O_RDWR | O_NOCTTY | O_NDELAY); //O_NONBLOCK); // Open UART device file
    // uart_fd = open("/dev/ttyS7", O_RDWR | O_NOCTTY | O_NONBLOCK); // Open UART device file
    assert (pInit->FileDescriptor != -1);

    // Get current UART settings
    struct termios options;
    tcgetattr(pInit->FileDescriptor, &options);

    // Set baud rate to 9600
    cfsetispeed(&options, getBaud(pInit->Baudrate));
    cfsetospeed(&options, getBaud(pInit->Baudrate));

    // Set 8 data bits, no parity, 1 stop bit (8N1)
    options.c_cflag &= ~PARENB;    // No parity
    options.c_cflag &= ~CSTOPB;    // 1 stop bit
    options.c_cflag &= ~CSIZE;     // Clear size bits
    options.c_cflag |= CS8;        // 8 data bits

    // Enable receiver and set local mode (ignore modem lines)
    options.c_cflag |= (CREAD | CLOCAL);

    // options.c_cflag &= ~CRTSCTS;

    // Set raw input/output mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);


    // options.c_iflag &= ~(IXON | IXOFF | IXANY);  // Disable flow control
    // options.c_oflag &= ~OPOST; // Raw output mode
    options.c_oflag &= ~(OPOST | ONLCR);
    
    options.c_cc[VMIN] = 0;   // Minimum 1 karakter bekle
    options.c_cc[VTIME] = 5; // Maksimum 100 ms (1 * 100ms) bekle

    // options.c_cc[VSTART] = 0xAA; // Start character
    // options.c_cc[VSTOP] = 0xBB;  // Stop character


    // cfmakeraw(&options);

    // Apply the configuration
    tcsetattr(pInit->FileDescriptor, TCSANOW, &options);

    pInit->IsOpen = 1;
}

int32_t uartWrite (UartInstance_TypeDef * pInit, void const * const pBuffer, size_t Length)
{
    if (! pInit->IsOpen) return -1;
    
    // #if defined (IOCTL_WRAPPER_POLL_WRITE)
    while ((pInit->PollTime != 0 ) && uartWaitReady(pInit)) usleep(pInit->PollTime);
    // #endif 
    int32_t bytesWritten = write(pInit->FileDescriptor, pBuffer, Length);
    // tcdrain(pInit->FileDescriptor);
    // printf("\r\n W %d", bytesWritten);
    return bytesWritten;
}
int32_t uartRead (UartInstance_TypeDef * pInit, void * const pBuffer, size_t Length)
{  
    if (! pInit->IsOpen) return -1;
    
    int32_t bytesRead = read(pInit->FileDescriptor, pBuffer, Length);
    // printf("\r\n R %d", bytesRead);
    return bytesRead;
}

int32_t uartClose (UartInstance_TypeDef * pInit)
{
    pInit->IsOpen = 0;
    return close (pInit->FileDescriptor);
}
