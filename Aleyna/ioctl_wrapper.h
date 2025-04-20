/**
 *
 *
 */

#ifndef IOCTL_WRAPPER
#define IOCTL_WRAPPER

// I left it to user, If there is need to run on different OS, implementation can be abstracted here
// #define _GNU_SOURCE
// #define IOCTL_WRAPPER_POLL_WRITE
// // #define IOCTL_WRAPPER_AFFINITY_COREID (2U)
// #define IOCTL_WRAPPER_POLL_WRITE_TIME (1000000U)
// #define IOCTL_WRAPPER_USE_AFFINITY


/* Includes */

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stddef.h>


typedef struct uart_instance_s
{
  int32_t FileDescriptor;      
  const char * pDeviceName;
  uint32_t Baudrate;

  int32_t PollTime;
  int32_t IsOpen; /**< 1 Open, 0 CLose */
} UartInstance_TypeDef;

// #if defined (IOCTL_WRAPPER_POLL_WRITE)
#include <poll.h>

// #if ! defined (IOCTL_WRAPPER_POLL_WRITE_TIME)
// #warning "poll time is not specified, usleep(1000) is set default"
// #define IOCTL_WRAPPER_POLL_WRITE_TIME (1000U)
// #endif // defined (IOCTL_WRAPPER_POLL_WRITE_TIME)

// #endif // defined (IOCTL_WRAPPER_POLL_WRITE)


#if defined(IOCTL_WRAPPER_USE_AFFINITY)
#include <sched.h>

#if !defined(IOCTL_WRAPPER_AFFINITY_COREID)
#warning "core id is not defined, '0' will be used as default"
#define IOCTL_WRAPPER_AFFINITY_COREID (0U)

#endif // !defined(IOCTL_WRAPPER_AFFINITY_COREID)

#endif // defined(IOCTL_WRAPPER_USE_AFFINITY)

/* Functions */
#if defined(IOCTL_WRAPPER_USE_AFFINITY)
void setCpuAffinity();
#endif // defined(IOCTL_WRAPPER_USE_AFFINITY)

int32_t uart_Init(UartInstance_TypeDef * pInit, const char * pDeviceName, uint32_t Baudrate, uint32_t PollTime);

void uartConfigure(UartInstance_TypeDef * pInit);
int32_t uartWrite (UartInstance_TypeDef * pInit, void const * const pBuffer, size_t Length);
int32_t uartRead (UartInstance_TypeDef * pInit, void * const pBuffer, size_t Length);
int32_t uartClose (UartInstance_TypeDef * pInit);

#endif // IOCTL_WRAPPER
