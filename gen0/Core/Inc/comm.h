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
#define PAYLOAD
#define COMMAND
#define COMMAND_MASK

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
USERSTRUCT PAYLOAD typedef struct __attribute__ ((__packed__ )) com_def_imu_s
{
    float fX;
    float fY;
    float fZ;
} ComDefImu_TypeDef;



/**
 * @brief ComDefMode_TypeDef
 */
USERSTRUCT PAYLOAD typedef struct __attribute__((__packed__)) com_def_mode_s
{
    uint8_t u8Mode;
} ComDefMode_TypeDef;

/**
 * @brief ComDefAngle_TypeDef
 */
USERSTRUCT PAYLOAD typedef struct __attribute__((__packed__)) com_def_angle_s
{
    uint16_t u16Angle;
} ComDefAngle_TypeDef;


USERSTRUCT PAYLOAD typedef struct __attribute__((__packed__)) com_def_sampling_rate_s
{
    uint16_t u16SamplingRate;
} ComDefSamplingRate_TypeDef;

/**
 * @brief ComDefCommandMask_TypeDef
 */
USERENUM COMMAND typedef enum com_def_command_mask_e
{
    ComDefCommandMask  	  = 0x0F,
    ComDefCommandMaskGet  = 0x01,
    ComDefCommandMaskSet  = 0x02,
    ComDefCommandMaskRet  = 0x04, ///< this is what slave or master returns.
} ComDefCommandMask_TypeDef;


/**
 * @brief ComDefCommands_TypeDef
 */
USERENUM COMMAND typedef enum com_def_commands_e
{
    ComDefCommandModeMask     = 0xF0,
    ComDefCommandMode         = 0x10,
    ComDefCommandImu          = 0x20,
    ComDefCommandSamplingRate = 0x40,
    ComDefCommandAngle        = 0x80,
} ComDefCommands_TypeDef;

//USERENUM PAYLOAD typedef enum com_def_command_status_e
//{
//
//} ComDefCommandStatus_TypeDef;


/**
 * @brief ComDef_Modes_TypeDef
 * @note This is Fucking for ComdefCommandMode
 */
USERENUM PAYLOAD typedef enum com_def_modes_e
{
    ComDefModeStream    = 0x01,
    ComDefModeOnRequest = 0x02,
    ComDefModeStop      = 0x04,
} ComDef_Modes_TypeDef;




#define ComDef_xu8CommandMask(__COMMAND, __MASK)  \
  ( (__COMMAND) | (__MASK) )


#define ComDef_xu8GetHeading(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) ) ->u8Heading )

#define ComDef_xu8GetCommand(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u8Command )

#define ComDef_xu16GetPayloadLength(__MSG__)  \
( ( (ComDef_TypeDef*) ((__MSG__)) )->u16PayloadLength )

#define ComDef_xu8GetCrc(__MSG__)  \
( * (  ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +4 ) )

#define ComDef_xu8GetEnd(__MSG__)  \
( * ( ( (uint8_t*)((__MSG__)) ) + ComDef_xu16GetPayloadLength(__MSG__) +5 ) )

#define ComDef_xpu8DeclareBuffer(__BUFFERNAME, __PAYLOADTYPE) \
uint8_t __BUFFERNAME [sizeof( __PAYLOADTYPE) + 6] = {0}

#define ComDef_xpu8GetPayload(__MSG__)  \
( * ( ( (uint8_t *)&( ComDef_xu16GetPayloadLength(__MSG__)) ) +2) )

#define ComDef_xpu32CalculateLength(__MSG__) \
( ComDef_xu16GetPayloadLength(__MSG__) +6 )




#ifdef __cplusplus
}
#endif

#endif // COMM_H
