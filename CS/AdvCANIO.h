#ifndef _AdvCanIO_H
#define _AdvCanIO_H



#define SUCCESS                           0                                       //Status definition : success
#define OPERATION_ERROR                   -1                                      //Status definition : device error or parameter error
#define TIME_OUT                          -2                                      //Status definition : time out

#pragma pack(1)
/*****************************************************************************
 DESCRIPTION: Standard baud  
*****************************************************************************/
#define CAN_TIMING_10K          10
#define CAN_TIMING_20K          20
#define CAN_TIMING_50K          50	
#define CAN_TIMING_100K         100
#define CAN_TIMING_125K         125
#define CAN_TIMING_250K         250
#define CAN_TIMING_500K         500	
#define CAN_TIMING_800K         800	
#define CAN_TIMING_1000K        1000

/*****************************************************************************
 DESCRIPTION: Acceptance filter mode  
*****************************************************************************/
#define PELICAN_SINGLE_FILTER   1
#define PELICAN_DUAL_FILTER     0

/*****************************************************************************
 DESCRIPTION: CAN data length  
*****************************************************************************/
#define DATALENGTH              8                       /* CAN data length */


/*****************************************************************************
 DESCRIPTION: CAN frame flag 
*****************************************************************************/
#define MSG_RTR                 (1<<0)                  /* RTR Message */
#define MSG_OVR                 (1<<1)                  /* CAN controller Msg overflow error */
#define MSG_EXT                 (1<<2)                  /* Extended message format */
#define MSG_SELF                (1<<3)                  /* Message received from own tx */
#define MSG_PASSIVE             (1<<4)                  /* CAN Controller in error passive */
#define MSG_BUSOFF              (1<<5)                  /* CAN Controller Bus Off  */
#define MSG_BOVR                (1<<7)                  /* Receive buffer overflow */


/*****************************************************************************
 DESCRIPTION: For CAN frame id. if flags of frame point out 
 some errors(MSG_OVR, MSG_PASSIVE, MSG_BUSOFF, MSG_BOUR), 
 then id of frame is equal to ERRORID
*****************************************************************************/
#define ERRORID                 0xffffffff
        

/*****************************************************************************
 DESCRIPTION: CAN frame use by driver 
*****************************************************************************/
typedef struct CanMsg
{
    int                     flags;                  /* Flags, indicating or controlling special message properties */
    int                     cob;                    /* CAN object number, used in Full CAN */
    ULONG                   id;                     /* CAN message ID, 4 bytes  */
    short int               length;                 /* Number of bytes in the CAN message */
    UCHAR                   data[DATALENGTH];       /* Data, 0...8 bytes */
}CanMsg_t;


/*****************************************************************************
 DESCRIPTION: IOCTL code 
*****************************************************************************/
#define ADS_TYPE FILE_DEVICE_UNKNOWN
#define ADS_IOCTL_INDEX  0x950

/* IOCTL command request */
#define CAN_IOCTL_COMMAND \
CTL_CODE(ADS_TYPE, ADS_IOCTL_INDEX,    METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CAN_COMMAND CAN_IOCTL_COMMAND

/* IOCTL configuration request */
#define CAN_IOCTL_CONFIG \
CTL_CODE(ADS_TYPE, ADS_IOCTL_INDEX+1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CAN_CONFIG CAN_IOCTL_CONFIG

/* IOCTL status request */
#define CAN_IOCTL_STATUS \
CTL_CODE(ADS_TYPE, ADS_IOCTL_INDEX+5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CAN_STATUS CAN_IOCTL_STATUS


/*****************************************************************************
 DESCRIPTION: IOCTL command request parameter structure 
*****************************************************************************/
struct Command_par {
        int                     cmd;                    /* special driver command */
        int                     target;                 /* special configuration target */
        ULONG                   val1;                   /* 1. parameter  */
        ULONG                   val2;                   /* 2. parameter  */
        int                     error;                  /* return value */
        ULONG                   retval;                 /* return value */
        
};


/*****************************************************************************
 DESCRIPTION: IOCTL command cmd targets
*****************************************************************************/
#define CMD_START               1                       /* Start chip */
#define CMD_STOP                2                       /* Stop chip */
#define CMD_RESET               3                       /* Reset chip */
#define CMD_CLEARBUFFERS        4                       /* Clear the receive buffer */


/*****************************************************************************
 DESCRIPTION: IOCTL command request parameter structure
*****************************************************************************/
typedef struct Command_par Command_par_t ; 


/*****************************************************************************
 DESCRIPTION: IOCTL configure cmd targets
*****************************************************************************/
#define CONF_ACC                0                       /* Acceptance code register and mask register*/
#define CONF_ACCM               1                       /* Acceptance mask only */
#define CONF_ACCC               2                       /* Acceptance code only */
#define CONF_TIMING             3                       /* Bit timing */
#define CONF_LISTEN_ONLY_MODE   8                       /* For SJA1000 PeliCAN */
#define CONF_SELF_RECEPTION     9                       /* Self reception */
#define CONF_TIMEOUT            13                      /* Configure read and write timeout one time */
#define CONF_ACC_FILTER         20                      /* Acceptance filter mode: 1-Single, 0-Dual*/

/*****************************************************************************
 DESCRIPTION: IOCTL configuration request parameter structure
*****************************************************************************/
typedef struct Command_par  Config_par_t ; 


/*****************************************************************************
 DESCRIPTION: IOCTL generic CAN controller status request parameter structure 
*****************************************************************************/
typedef struct CanStatusPar{
        unsigned int            baud;                   /* Actual bit rate */
        unsigned int            status;                 /* CAN controller status register, address 2 */
        unsigned int            error_warning_limit;    /* The error warning limit, address 13 */
        unsigned int            rx_errors;              /* Content of RX error counter, address 14 */
        unsigned int            tx_errors;              /* Content of TX error counter, address 15 */
        unsigned int            error_code;             /* Content of error code register, address 12 */
        unsigned int            rx_buffer_size;         /* Size of rx buffer*/
        unsigned int            rx_buffer_used;         /* number of messages*/
        unsigned int            tx_buffer_size;         /* Size of tx buffer for wince, windows not use tx buffer*/
        unsigned int            tx_buffer_used;         /* Number of message for wince, windows not use tx buffer s*/
        ULONG                   retval;                 /* Return value*/
        unsigned int            type;                   /* CAN controller/driver type*/
        unsigned int            acceptancecode;         /* Acceptance code */
        unsigned int            acceptancemask;         /* Acceptance mask */
        unsigned int            acceptancemode;         /**< Acceptance Filter Mode: 1:Single 0:Dual */
        unsigned int            selfreception;          /* Self reception */
        unsigned int            readtimeout;            /* Read timeout */
        unsigned int            writetimeout;           /* Write timeout */
}CanStatusPar_t;

/*****************************************************************************
*
*    acCanOpen
*
*    Purpose:
*        Open can port by name 
*		
*
*    Arguments:
*        PortName            - port name
*        synchronization     - TRUE, synchronization ; FALSE, asynchronous
*
*    Returns:
*        Hanlde of device 
*
*****************************************************************************/
HANDLE acCanOpen(TCHAR *PortName, BOOL synchronization);

/*****************************************************************************
*
*    acCanClose
*
*    Purpose:
*        Close can port by handle 
*		
*
*    Arguments:
*        hDevice          - handle of device
*
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acCanClose(HANDLE hDevice);

/*****************************************************************************
*
*    acEnterResetMode
*
*    Purpose:
*        Enter reset mode.
*		
*
*    Arguments:
*        hDevice            - handle of device
*
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acEnterResetMode(HANDLE hDevice);

/*****************************************************************************
*
*    acEnterWorkMode
*
*    Purpose:
*        Enter work mode 
*		
*
*    Arguments:
*        hDevice        - handle of device
*
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acEnterWorkMode(HANDLE hDevice);

/*****************************************************************************
*
*    acClearRxFifo
*
*    Purpose:
*        Clear can port rx buffer by handle 
*		
*
*    Arguments:
*        hDevice        - handle of device
*
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acClearRxFifo(HANDLE hDevice);

/*****************************************************************************
*
*    acSetBaud
*
*    Purpose:
*        Set baudrate of the CAN Controller.The two modes of configuring
*     baud rate are custom mode and standard mode.
*     -   Custom mode
*         If Baud Rate value is user defined, driver will write the first 8
*         bit of low 16 bit in BTR0 of SJA1000.
*         The lower order 8 bit of low 16 bit will be written in BTR1 of SJA1000.
*     -   Standard mode
*         Target value     BTR0      BTR1      Setting value 
*           10K            0x31      0x1c      10 
*           20K            0x18      0x1c      20 
*           50K            0x09      0x1c      50 
*          100K            0x04      0x1c      100 
*          125K            0x03      0x1c      125 
*          250K            0x01      0x1c      250 
*          500K            0x00      0x1c      500 
*          800K            0x00      0x16      800 
*         1000K            0x00      0x14      1000 
*		
*
*    Arguments:
*        hDevice        - handle of device
*        nBaud          - baudrate will be set
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetBaud(HANDLE hDevice, unsigned int nBaud);

/*****************************************************************************
*
*    acSetBaudRegister
*
*    Purpose:
*        Configures baud rate by custom mode.
*		
*
*    Arguments:
*        hDevice        - handle of device
*        Btr0           - BTR0 register value.
*        Btr1           - BTR1 register value.
*    Returns:
*	 =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetBaudRegister(HANDLE hDevice, unsigned char Btr0, unsigned char Btr1);

/*****************************************************************************
*
*    acSetTimeOut
*
*    Purpose:
*        Set Timeout for read and write
*		
*
*    Arguments:
*        hDevice           - handle of device
*        ulReadTimeOut     - ms
*         ulWriteTimeOut   - ms
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetTimeOut(HANDLE hDevice, ULONG ulReadTimeOut, ULONG ulWriteTimeOut);

/*****************************************************************************
*
*    acSetSelfReception
*
*    Purpose:
*        Set Self Reception mode
*		
*
*    Arguments:
*        hDevice        - handle of device
*        bSelfFlag      - TRUE, open self reception; FALSE close self reception
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetSelfReception(HANDLE hDevice, BOOL bSelfFlag);

/*****************************************************************************
*
*    acSetListenOnlyMode
*
*    Purpose:
*        Set listen only mode
*	
*
*    Arguments:
*        hDevice            - Handle of device
*        bListerOnlyFlag    - TRUE, open only listen mode; FALSE, close only listen mode
*    Returns:
*        =0 succeeded; or <0 Failed 
*
*****************************************************************************/
int acSetListenOnlyMode(HANDLE hDevice,BOOL   bListerOnlyFlag);

/*****************************************************************************
*
*    acSetAcceptanceFilterMode
*
*    Purpose:
*        Set acceptance filter mode
*	
*
*    Arguments:
*        hDevice         - Handle of device
*        nFilterMode     - PELICAN_SINGLE_FILTER, single filter mode; PELICAN_DUAL_FILTER, dule filter mode
*    Returns:
*        =0 succeeded; or <0 Failed 
*
*****************************************************************************/
int acSetAcceptanceFilterMode(HANDLE hDevice, int nFilterMode);

/*****************************************************************************
*
*    acSetAcceptanceFilterCode
*
*    Purpose:
*        Set acceptance code
*		
*
*    Arguments:
*        hDevice              - handle of device
*        ulAcceptanceCode		- acceptance code
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetAcceptanceFilterCode(HANDLE hDevice, ULONG ulAcceptanceCode);

/*****************************************************************************
*
*    acSetAcceptanceFilterMask
*
*    Purpose:
*        Set acceptance mask
*		
*
*    Arguments:
*        hDevice              - handle of device
*        ulAcceptanceMask     - accept mask code
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetAcceptanceFilterMask(HANDLE hDevice, ULONG ulAcceptanceMask);

/*****************************************************************************
*
*    acSetAcceptanceFilter
*
*    Purpose:
*        Set acceptance code and Mask
*		
*
*    Arguments:
*        hDevice                 - handle of device
*        ulAcceptanceCode        - acceptance code
*        ulAcceptanceMask        - acceptance mask
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acSetAcceptanceFilter(HANDLE hDevice, ULONG ulAcceptanceCode, ULONG ulAcceptanceMask);

/*****************************************************************************
*
*    acCanWrite
*
*    Purpose:
*        Write can msg
*		
*
*    Arguments:
*        hDevice                      - handle of device
*        pbyData                      - data buffer
*        ulWriteCount                 - msgs number
*        pulNumberofWritten           - real msgs have written
*        ov                           - synchronization event
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acCanWrite(HANDLE hDevice, CanMsg_t *pbyData, ULONG ulWriteCount, ULONG *pulNumberofWritten, OVERLAPPED *ov);

/*****************************************************************************
*
*    acCanRead
*
*    Purpose:
*        Read can message
*		
*
*    Arguments:
*        hDevice           - handle of device
*        pbyData           - data buffer
*        ulReadCount       - msg number of data buffer size
*        pulNumberofRead   - real msgs have read
*        ov                - synchronization event
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acCanRead(HANDLE hDevice, CanMsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, OVERLAPPED *ov);

/*****************************************************************************
*
*    acGetStatus
*
*    Purpose:
*        Get driver current status
*		
*
*    Arguments:
*        hDevice           - handle of device
*        CanStatus         - status buffer
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acGetStatus(HANDLE hDevice, CanStatusPar_t * CanStatus);

/*****************************************************************************
*
*    acClearCommError
*
*    Purpose:
*        Execute ClearCommError of AdvCan.
*		
*
*    Arguments:
*        hDevice           - handle of device
*        ErrorCode         - error code if the CAN Controller occur error
* 
* 
*    Returns:
*        TRUE SUCCESS; or FALSE failure 
*
*****************************************************************************/
BOOL acClearCommError(HANDLE hDevice, ULONG *ulErrorCode);

/*****************************************************************************
*
*    acSetCommMask
*
*    Purpose:
*        Execute SetCommMask.
*		
*
*    Arguments:
*        hDevice           - handle of device
*        EvtMask           - event type
* 
* 
*    Returns:
*        TRUE SUCCESS; or FALSE failure 
*
*****************************************************************************/
BOOL acSetCommMask(HANDLE hDevice, ULONG ulEvtMask);

/*****************************************************************************
*
*    acGetCommMask
*
*    Purpose:
*        Execute GetCommMask.
*		
*
*    Arguments:
*        hDevice         - handle of device
*        EvtMask         - event type
* 
* 
*    Returns:
*        TRUE SUCCESS; or FALSE failure 
*
*****************************************************************************/
BOOL acGetCommMask(HANDLE hDevice, ULONG *ulEvtMask);

/*****************************************************************************
*
*    acWaitEvent
*
*    Purpose:
*        Wait can message or error of the CAN Controller.
*		
*
*    Arguments:
*        hDevice              - handle of device
*        pbyData              - buffer for read
*        nReadCount           - msgs number
*        pulNumberofRead      - real msgs have read
*        ErrorCode            - return error code when the CAN Controller has error
*        ov                   - synchronization event
* 
*    Returns:
*        =0 SUCCESS; or <0 failure 
*
*****************************************************************************/
int acWaitEvent(HANDLE hDevice, CanMsg_t *pbyData, ULONG ulReadCount, ULONG *pulNumberofRead, ULONG *ErrorCode, OVERLAPPED *ov);

#endif