#ifndef _PCI_H
#define _PCI_H

#define PCI_BUS_SHIFT               0x8
#define PCI_BUS_SIZE                0x8
#define PCI_BUS_MAX                 0xFF
#define PCI_BUS_MASK                0xFF00
#define PCI_DEVICE_SHIFT            0x3
#define PCI_DEVICE_SIZE             0x5
#define PCI_DEVICE_MAX              0x1F
#define PCI_DEVICE_MASK             0x00F8
#define PCI_FUNCTION_SHIFT          0x0
#define PCI_FUNCTION_SIZE           0x3
#define PCI_FUNCTION_MAX            0x7
#define PCI_FUNCTION_MASK           0x0007

#define MK_PCI(bus,dev,func) (uint16_t)((bus&PCI_BUS_MAX)<<PCI_BUS_SHIFT | (dev&PCI_DEVICE_MAX)<<PCI_DEVICE_SHIFT | (func&PCI_FUNCTION_MAX) )
/* PCI address */
#define MK_PCICFGADDR(bus,dev,func) (uint32_t)(0x80000000L | (uint32_t)MK_PCI(bus,dev,func)<<8)
/* PCI Configuration Register */
#define PCI_CONFIG_ADDRESS          0xCF8
#define PCI_CONFIG_DATA             0xCFC

struct cardDetails {
	uint8_t u8BusNum;
	uint8_t u8DevNum;
	uint8_t u8FuncNum;
	uint32_t ioAddress;
};

void getPciDev(uint16_t *u16VenId, uint16_t *u16DevId, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum);
void scanPciBus(struct cardDetails cards[]);


void getPowerMgmtCtrl(uint16_t *pmctl, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum);
void setPowerMgmtCtrl(uint16_t pmvalue, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum);


#endif // !_PCI_H
