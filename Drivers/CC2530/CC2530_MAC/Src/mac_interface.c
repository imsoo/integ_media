#include <string.h>                 //for memcpy()
#include "mac_interface.h"
#include "mac_interface_uart.h"
#include "stm32f4xx_hal.h"
#include "integ_mac.h"

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

#define ZMAC_SHORT_ADDRESS 0x53
#define ZMAC_EXTENDED_ADDRESS 0xE2

#define PAN_COORDINATOR 0x00

unsigned char cc2530_mac_addr[8];
unsigned char cc2530_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned char startMac(unsigned char deviceType) 
{
  deviceType = PAN_COORDINATOR;
  unsigned char attrValue[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  unsigned char attrValue2[16] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
#define ZMAC_SHORT_ADDRESS 0x53
#define ZMAC_EXTENDED_ADDRESS 0xE2
#define ZMAC_ASSOCIATION_PERMIT 0x41
#define ZMAC_RX_ON_IDLE 0x52
#define ZMAC_AUTO_REQUEST 0x42
#define ZMAC_BEACON_ORDER 0x47  
  
#define ENERGY_DETECT 0x00  
#define ACTIVE_SCAN 0x01
#define PASSIVE 0x02  
#define ORPHAN 0x03
  
  // SYS_RESET_REQ
  macReset();
  
  // MAC_RESET_REQ
  macSoftReset();
  HAL_Delay(100);
  
  // call back Enable
  utilCallbackSubCmd();
  HAL_Delay(100);
  
  // mtUtilGetPrimaryIEEE
  mtUtilGetPrimaryIEEE();
  
  if(deviceType == PAN_COORDINATOR) {
    // MAC_SET_REQ [SHORT ADDRESS, EXT ADDRESS]
    // set the parameter
    macSetReq(ZMAC_SHORT_ADDRESS, cc2530_mac_addr);
    macSetReq(ZMAC_EXTENDED_ADDRESS, cc2530_mac_addr);
    
    // auto request false;
    attrValue[0] = 0x00;
    macSetReq(ZMAC_AUTO_REQUEST, attrValue2);
    HAL_Delay(100);
    
    // MAC_SCAN_REQ
    macScanReq(ACTIVE_SCAN);
    HAL_Delay(100);
    
    // MAC_START_REQ
    macStartReq();
    HAL_Delay(100);
    
    // MAC_SET_REQ [ASSOCIATION_PERMIT, RX_ON_IDLE]
    // set the parameter
    attrValue[0] = 0x01;
    macSetReq(ZMAC_ASSOCIATION_PERMIT, attrValue);
    HAL_Delay(100);
    
    macSetReq(ZMAC_RX_ON_IDLE, attrValue);
    HAL_Delay(100);
  }
  
  // Not PAN
  else {
    // 초기화 완료(네트워크 참여) 될 때 까지 무한 반복
    stateFlag = STATE_INIT;
    while(stateFlag != STATE_INIT_FINISH) {
      HAL_Delay(500);
      switch(stateFlag) {
      case STATE_INIT:
        printf("INIT...\r\n");
        
        // 장치 64비트 주소 설정
        macSetReq(ZMAC_EXTENDED_ADDRESS, attrValue2);
        HAL_Delay(100);
        
        // auto request on  : not receive beacon;
        attrValue2[0] = 0x01;
        macSetReq(ZMAC_AUTO_REQUEST, attrValue2);
        HAL_Delay(100);
        
        // 수신 대기 모드로 설정
        attrValue2[0] = 0x01;
        macSetReq(ZMAC_RX_ON_IDLE, attrValue2);
        HAL_Delay(100);
        
        stateFlag = STATE_SCAN;
        break;
        
      case STATE_SCAN:
        printf("SCAN...\r\n");
        macScanReq(ENERGY_DETECT);
        HAL_Delay(100);
        break;
        
      case STATE_SYNC:
        printf("SYNC...\r\n");
        
        // auto request off receive beacon;
        attrValue2[0] = 0x00;
        macSetReq(ZMAC_AUTO_REQUEST, attrValue2);
        HAL_Delay(100);
        
        // beacon order;
        attrValue2[0] = 0x09;
        macSetReq(ZMAC_BEACON_ORDER, attrValue2);
        HAL_Delay(100);
        
        // COORD_SHORT
#define ZMAC_COORD_SHORT_ADDRESS 0x4B
        attrValue2[0] = 0x11;
        attrValue2[1] = 0x11;
        macSetReq(ZMAC_COORD_SHORT_ADDRESS, attrValue2);
        HAL_Delay(100);
        
        // COORD_EXT
#define ZMAC_COORD_EXTENDED_ADDRESS 0x4A
        attrValue2[0] = 0x11;
        attrValue2[1] = 0x11;
        attrValue2[2] = 0x11;
        attrValue2[3] = 0x11;
        attrValue2[4] = 0x11;
        attrValue2[5] = 0x11;
        attrValue2[6] = 0x11;
        attrValue2[7] = 0x11;
        macSetReq(ZMAC_COORD_EXTENDED_ADDRESS, attrValue2);
        HAL_Delay(100);
        
        // PAN_ID
#define ZMAC_PANID 0x50
        attrValue2[0] = 0xAA;
        attrValue2[1] = 0xAA;
        macSetReq(ZMAC_PANID, attrValue2);
        HAL_Delay(100);
        
        macSyncReq();
        HAL_Delay(100);
        
        // auto request on : not receive beacon;
        attrValue2[0] = 0x01;
        macSetReq(ZMAC_AUTO_REQUEST, attrValue2);
        HAL_Delay(100);
        
        break;
        
      case STATE_ASSOCIATE:
        printf("ASSOCIATE...\r\n");
        
        macAssociateReq();
        HAL_Delay(100);
        break;
      }
    }
    printf("INIT End...\r\n");
    HAL_Delay(500);
  }
  
  /*
  macGetReq(0x47);
  macGetReq(ZMAC_RX_ON_IDLE);  
  macGetReq(ZMAC_SHORT_ADDRESS);
  macGetReq(ZMAC_EXTENDED_ADDRESS);  
  macGetReq(0xE1);
  macGetReq(0x50);
  macGetReq(0x41);
  */
  
  
  return R_SUCCESS;
}

unsigned char* macReset()
{
  HAL_Delay(100);
  //RADIO_OFF();
  HAL_GPIO_WritePin(CC_RESET_GPIO_Port , CC_RESET_Pin , GPIO_PIN_RESET);
  HAL_Delay(100);
  //RADIO_ON();
  HAL_GPIO_WritePin(CC_RESET_GPIO_Port , CC_RESET_Pin , GPIO_PIN_SET);
  HAL_Delay(100);
  return (macBuf+U_SRSP_PAYLOAD_START);  //the beginning of the reset indication string
}

#define UITL_CALLBACK_SUB_CMD 0x2706
#define UITL_CALLBACK_SUB_CMD_LEN 0x03
void utilCallbackSubCmd()
{
  macBuf[0] = UITL_CALLBACK_SUB_CMD_LEN;
  macBuf[1] = MSB(UITL_CALLBACK_SUB_CMD);
  macBuf[2] = LSB(UITL_CALLBACK_SUB_CMD);  
  macBuf[3] = 0xFF;
  macBuf[4] = 0xFF;
  macBuf[5] = 0x01;
  uartSreq(STATE_SREQ);
}

#define MAC_RESET_PAYLOAD_LEN 0x01
#define MAC_RESET_REQ 0x2201
void macSoftReset()
{
  macBuf[0] = MAC_RESET_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_RESET_REQ);
  macBuf[2] = LSB(MAC_RESET_REQ);  
  macBuf[3] = 0x01;
  uartSreq(STATE_SREQ);
  //printMacBuf();
}

#define MAC_SET_REQ 0x2209
#define MAC_SET_REQ_PAYLOAD_LEN 0x11
void macSetReq(unsigned char attr, unsigned char *attrValue)
{
  macBuf[0] = MAC_SET_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_SET_REQ);
  macBuf[2] = LSB(MAC_SET_REQ);
  macBuf[3] = attr;
  memcpy(macBuf + 4, attrValue, 16);
  uartSreq(STATE_SREQ);
}

#define MAC_GET_REQ 0x2208
#define MAC_GET_REQ_PAYLOAD_LEN 0x01
void macGetReq(unsigned char attr)
{
  macBuf[0] = MAC_GET_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_GET_REQ);
  macBuf[2] = LSB(MAC_GET_REQ);
  macBuf[3] = attr;
  uartSreq(STATE_SREQ);
}

#define MAC_ASSOCIATE_RSP 0x4250
#define MAC_ASSOCIATE_RSP_PAYLOAD_LEN 0x0B
void macAssociateRsp(void *arg)
{
  macBuf[0] = MAC_ASSOCIATE_RSP_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_ASSOCIATE_RSP);
  macBuf[2] = LSB(MAC_ASSOCIATE_RSP);
  
  // ExtAddr[8]
  macBuf[3] = 0x22;
  macBuf[4] = 0x22;
  macBuf[5] = 0x22;
  macBuf[6] = 0x22;
  macBuf[7] = 0x22;
  macBuf[8] = 0x22;
  macBuf[9] = 0x22;
  macBuf[10] = 0x22;
  
  // AssocShortAddress[2]
  macBuf[11] = 0x3c;
  macBuf[12] = 0x4d;
  
  // AssocStatus
  macBuf[13] = 0x00;
  
  uartSreq(STATE_SREQ);
  
  // wait MAC_COMM_STATUS_IND
  sreqFlag = STATE_ASSOCIATE_RSP_CNF;
  while(sreqFlag != STATE_IDLE);
  
  printf("AssociateRsp\r\n$ ");
}


#define MAC_SYNC_REQ 0x2204
#define MAC_SYNC_REQ_PAYLOAD_LEN 0x03
void macSyncReq()
{
  macBuf[0] = MAC_SYNC_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_SYNC_REQ);
  macBuf[2] = LSB(MAC_SYNC_REQ);
  
  // LogicalChannel 
  macBuf[3] = 0x0B;
  
  // ChannelPage 
  macBuf[4] = 0x00;
  
  // TrackBeacon [True : continue tracking]
  macBuf[5] = 0x01;
  
  uartSreq(STATE_SREQ);
  
  // wait MAC_BEACON_NOTIFY_IND
  sreqFlag = STATE_SYNC_CNF;
  while(sreqFlag != STATE_IDLE);
  
  // SYNC 성공 시 ASSOCIATE 단계로
  if(stateStatus == 0x00)
    stateFlag = STATE_ASSOCIATE;
  
}

#define MAC_ASSOCIATE_REQ 0x2206
#define MAC_ASSOCIATE_REQ_PAYLOAD_LEN 0x19
void macAssociateReq()
{
  macBuf[0] = MAC_ASSOCIATE_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_ASSOCIATE_REQ);
  macBuf[2] = LSB(MAC_ASSOCIATE_REQ);
  
  // LogicalChannel
  macBuf[3] = 0x0B;
  
  // ChannelPage
  macBuf[4] = 0x00;
  
  // CoordAddressMode
  macBuf[5] = 0x03;
  
  // CoordAddress[8]
  macBuf[6] = 0x11;
  macBuf[7] = 0x11;
  macBuf[8] = 0x11;
  macBuf[9] = 0x11;
  macBuf[10] = 0x11;
  macBuf[11] = 0x11;
  macBuf[12] = 0x11;
  macBuf[13] = 0x11;
  
  // CoordPanId[2]
  macBuf[14] = 0xAA;
  macBuf[15] = 0xAA;
  
  // CapabilityInformation
  macBuf[16] = 0x8E;
  //macBuf[16] = 0x0;
  
  // KeySource[8]
  macBuf[17] = 0x00;
  macBuf[18] = 0x00;
  macBuf[19] = 0x00;
  macBuf[20] = 0x00;
  macBuf[21] = 0x00;
  macBuf[22] = 0x00;
  macBuf[23] = 0x00;
  macBuf[24] = 0x00;
  
  // SecurityLevel
  macBuf[25] = 0x00;
  
  // KeyIdMode
  macBuf[26] = 0x00;
  
  // KeyIndex
  macBuf[27] = 0x00;
  
  uartSreq(STATE_SREQ);
  
  
  // wait MAC_ASSOCIATE_CNF
  sreqFlag = STATE_ASSOCIATE_CNF;
  while(sreqFlag != STATE_IDLE);
  
  // ASSOCIATE 성공 시 INIT_FINISH 단계로
  if(stateStatus == 0x00)
    stateFlag = STATE_INIT_FINISH;
}

#define MAC_DATA_REQ 0x2205
#define MAC_DATA_REQ_PAYLOAD_LEN 0x1C
unsigned char macDataReq(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  //printf("[CC2530] 전송 시도\r\n");
  macBuf[0] = MAC_DATA_REQ_PAYLOAD_LEN + data_length;
  macBuf[1] = MSB(MAC_DATA_REQ);
  macBuf[2] = LSB(MAC_DATA_REQ);
  
  // DestAddressMode
  macBuf[3] = 0x02;
  
  // DestAddress
  memcpy(macBuf + 4, dest_addr, 8);
  
  // DestPanID
  if(LSB(STM32_UUID[0]) == 0x2C) {      // MCU 변경시 변경 필요 함.
    macBuf[12] = 0x29;
    macBuf[13] = 0x29;
  }
  else {
    macBuf[12] = 0x2C;
    macBuf[13] = 0x2C;
  }
  
  /*
  macBuf[12] = 0xFF;
  macBuf[13] = 0xFF;
  */
  
  
  // SrcAddressMode
  macBuf[14] = 0x02;
  
  // Handle
  macBuf[15] = 0x00;
  
  // TxOption
  macBuf[16] = 0x10;
  
  // LogicalChannel
  macBuf[17] = 0x0B;
  
  // Power
  macBuf[18] = 0xFF;
  
  // KeySource
  macBuf[19] = 0x00;
  macBuf[20] = 0x00;
  macBuf[21] = 0x00;
  macBuf[22] = 0x00;
  macBuf[23] = 0x00;
  macBuf[24] = 0x00;
  macBuf[25] = 0x00;
  macBuf[26] = 0x00;
  
  // SecurityLevel
  macBuf[27] = 0x00;
  
  // KeyIdMode
  macBuf[28] = 0x00;
  
  // KeyIndex
  macBuf[29] = 0x00;
  
  // MSDULength
  macBuf[30] = data_length;  
  
  // MSDU
  //memcpy(macBuf+31, data, data_length);
  
  memcpy(macBuf+31, data, INTEG_FRAME_HEADER_LEN);
  memcpy(macBuf+31 + INTEG_FRAME_HEADER_LEN, ((INTEG_FRAME*) data)->data, data_length - INTEG_FRAME_HEADER_LEN);

  
  uartSreq(STATE_SREQ);
  
  // wait MAC_DATA_CNF
  sreqFlag = STATE_DATA_CNF;
  while(sreqFlag != STATE_IDLE);
  
  // DATA 전송 성공 시 1 반환
  if(stateStatus == 0x00)
    return 1;
  else 
    return 0;
}


#define MAC_START_REQ 0x2203
#define MAC_START_REQ_PAYLOAD_LEN 0x23;
void macStartReq()
{
  macBuf[0] = MAC_START_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_START_REQ);
  macBuf[2] = LSB(MAC_START_REQ);
  
  // startTime PAN Coordinator 인 경우 무시
  macBuf[3] = 0x00;
  macBuf[4] = 0x00;
  macBuf[5] = 0x00;
  macBuf[6] = 0x00;
  
  // Pan ID : 
  macBuf[7] = LSB(STM32_UUID[0]);
  macBuf[8] = LSB(STM32_UUID[0]);
  
  // LogiccalChannel
  macBuf[9] = 0x0B;
  
  // Channel Page
  macBuf[10] = 0x00;
  
  // BeaconOrder
  //macBuf[11] = 0x05;
  macBuf[11] = 0x09;
  
  // SuperFrameOrder
  macBuf[12] = 0x09;
  
  // PanCoordinatoor
  macBuf[13] = 0x01;
  
  // BatteryLifeExt
  macBuf[14] = 0x00;
  
  // CoordRealignment
  macBuf[15] = 0x00;
  
  // RealignKeySource
  macBuf[16] = 0x00;
  macBuf[17] = 0x00;
  macBuf[18] = 0x00;
  macBuf[19] = 0x00;
  macBuf[20] = 0x00;
  macBuf[21] = 0x00;
  macBuf[22] = 0x00;
  macBuf[23] = 0x00;
  
  // RealignSecurityLevel : No security
  macBuf[24] = 0x00;
  
  // RealignKeyIdMode : NOT Used
  macBuf[25] = 0x00;
  
  // RealignKeyIndex
  macBuf[26] = 0x00;
  
  // BeaconKeySource
  macBuf[27] = 0x00;
  macBuf[28] = 0x00;
  macBuf[29] = 0x00;
  macBuf[30] = 0x00;
  macBuf[31] = 0x00;
  macBuf[32] = 0x00;
  macBuf[33] = 0x00;
  macBuf[34] = 0x00;
  
  // BeaconSecurityLevel : No security
  macBuf[35] = 0x00;
  
  // BeaconKeyIdMode
  macBuf[36] = 0x00;
  
  // BeaconKeyIndex
  macBuf[37] = 0x00;
  
  uartSreq(STATE_SREQ);
  
  // wait MAC_START_CNF
  sreqFlag = STATE_START_CNF;
  while(sreqFlag != STATE_IDLE);
  
}

#define MAC_SCAN_REQ 0x220C
#define MAC_SCAN_REQ_PAYLOAD_LEN 0x13;
void macScanReq(unsigned char scanType)
{
  macBuf[0] = MAC_SCAN_REQ_PAYLOAD_LEN;
  macBuf[1] = MSB(MAC_SCAN_REQ);
  macBuf[2] = LSB(MAC_SCAN_REQ);
  
  // scanChannels : 기본 11
  /*
  macBuf[3] = 0x00;
  macBuf[4] = 0x00;
  macBuf[5] = 0x08;
  macBuf[6] = 0x00;
  */
  
  macBuf[3] = 0x07;
  macBuf[4] = 0xFF;
  macBuf[5] = 0xF8;
  macBuf[6] = 0x00;
  
  // scanType
  macBuf[7] = scanType;
  
  // ScanDuration
  //macBuf[8] = 0x03;
  macBuf[8] = 0x05;
  
  
  // ChannelPage
  macBuf[9] = 0x00;
  
  //MaxResult
  macBuf[10] = 0x05;
  
  // KeySource
  macBuf[11] = 0x00;
  macBuf[12] = 0x00;
  macBuf[13] = 0x00;
  macBuf[14] = 0x00;
  macBuf[15] = 0x00;
  macBuf[16] = 0x00;
  macBuf[17] = 0x00;
  macBuf[18] = 0x00;
  
  // SecurityLevel : NoSecurity
  macBuf[19] = 0x00;
  
  // KeyIdMode : NoUsed
  macBuf[20] = 0x00;
  
  // KeyIndex : NoUsed
  macBuf[21] = 0x00;
  
  uartSreq(STATE_SREQ);
  
  // wait MAC_SCAN_CNF
  sreqFlag = STATE_SCAN_CNF;
  while(sreqFlag != STATE_IDLE);
  
  // SCAN 성공 시 ASSOCIATE 단계로
  if(stateStatus == 0x00)
    stateFlag = STATE_SYNC;
}


#define MT_UTIL_GET_PRIMARY_IEEE 0x27EF
#define MT_UTIL_GET_PRIMARY_IEEE_PAYLOAD_LEN 0x01;
void mtUtilGetPrimaryIEEE()
{
  macBuf[0] = MT_UTIL_GET_PRIMARY_IEEE_PAYLOAD_LEN;
  macBuf[1] = MSB(MT_UTIL_GET_PRIMARY_IEEE);
  macBuf[2] = LSB(MT_UTIL_GET_PRIMARY_IEEE);
  
  macBuf[3] = 0x00;
  
  uartSreq(STATE_SREQ);
  memcpy(cc2530_mac_addr, macBuf + 1, 8);
  cc2530_mac_addr[0] = LSB(STM32_UUID[0]);
}

unsigned char* cc2530_get_mac_addr(unsigned char addr_type) {
  
  if (addr_type == BROADCAST_ADDR) {
    return cc2530_broadcast_addr;
  }
  else {
    return cc2530_mac_addr;
  }
}

void printMacBuf()
{
  for (int i=0; i<10; i++)
    //for (int i=0; i<10; i++)
    printf("%02X ", macBuf[i]);
  printf("\r\n");
}