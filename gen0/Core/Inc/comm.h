/**
 * @file
 */

#ifndef COMM_H
#define COMM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define DONTUSE
#define USERSTRUCT
#define USERENUM

/**
 * @brief ComDef_TypeDef
 * General Message Structure,
 * Access the elements with macro, dont use structItself
 *
 */
DONTUSE typedef struct __attribute__((__packed__)) com_def_s
{
    uint8_t u8Heading;
    uint8_t u8Command;
    uint16_t u16PayloadLength;
    // PAYLOAD
    // CRC
    // END
} ComDef_TypeDef;

/**
 * @brief ComDefImu_TypeDef
 */
USERSTRUCT typedef struct __attribute__ ((__packed__ )) com_def_imu_s
{
    float fX;
    float fY;
    float fZ;
} ComDefImu_TypeDef;



/**
 * @brief ComDefMode_TypeDef
 */
USERSTRUCT typedef struct __attribute__((__packed__)) com_def_mode_s
{
    uint8_t u8Mode;
} ComDefMode_TypeDef;

/**
 * @brief ComDefAngle_TypeDef
 */
USERSTRUCT typedef struct __attribute__((__packed__)) com_def_angle_s
{
    uint16_t u16Angle;
} ComDefAngle_TypeDef;

/**
 * @brief ComDefCommandMask_TypeDef
 */
USERENUM typedef enum com_def_command_mask_e
{
    ComDefCommandMaskXet  = 0x0F,
    ComDefCommandMaskGet  = 0x01,
    ComDefCommandMaskSet  = 0x02,
    ComDefCommandMaskRet  = 0x04,
} ComDefCommandMask_TypeDef;


/**
 * @brief ComDefCommands_TypeDef
 */
USERENUM typedef enum com_def_commands_e
{
    ComDefCommandModeMask     = 0xF0,
    ComDefCommandMode         = 0x10,
    ComDefCommandAngle        = 0x20,
    ComDefCommandSamplingRate = 0x40,
} ComDefCommands_TypeDef;


/**
 * @brief ComDef_Modes_TypeDef
 */
USERENUM typedef enum com_def_modes_e
{
    ComDefModeStream    = 0x01,
    ComDefModeOnRequest = 0x02,
} ComDef_Modes_TypeDef;



// #define ComDef_xu8CommandGet(__COMMAND)  \
//   ( (__COMMAND) | CommandMaskGet )
//
// #define ComDef_xu8CommandSet(__COMMAND)  \
//   ( (__COMMAND) | CommandMaskGet )
//
// #define ComDef_xu8CommandRet(__COMMAND)  \
//   ( (__COMMAND) | CommandMaskRet )


#define ComDef_xu8CommandMask(__COMMAND, __MASK)  \
  ( (__COMMAND) | (__MASK) )


#define ComDef_xu8GetHeading(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) ) ->u8Heading )

#define ComDef_xu8GetCommand(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u8Command )

#define ComDef_xu16GetPayloadLength(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u16PayloadLength )

#define ComDef_xu8GetCrc(__MSG__)  \
( * (  ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +3 ) )

#define ComDef_xu8GetEnd(__MSG__)  \
( * ( ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +4 ) )

#define ComDef_xpu8DeclareBuffer(__BUFFERNAME, __PAYLOADTYPE) \
uint8_t __BUFFERNAME [sizeof( __PAYLOADTYPE) + 5] = {0}

#define ComDef_xpu8GetPayload(__MSG__)  \
( * ( ( (uint8_t *)&( ComDef_xu16GetPayloadLength(__MSG__)) ) +1) )

#define ComDef_xpu32CalculateLength(__MSG__) \
( ComDef_xu16GetPayloadLength(__MSG__) +5 )




#ifdef __cplusplus
}
#endif

#endif // COMM_H
