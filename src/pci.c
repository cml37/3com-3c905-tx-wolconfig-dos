#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include "pci.h"
#include "vendor.h"

void getPciDev(uint16_t *venId, uint16_t *devId, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum)
{
    uint32_t addr;
    uint32_t data;

    addr = MK_PCICFGADDR(u8BusNum, u8DevNum, u8FuncNum);

    outpd(PCI_CONFIG_ADDRESS, addr);
    data = inpd(PCI_CONFIG_DATA);

    *venId = (data & VENDOR_ID_MASK);
    *devId = (data & DEVICE_ID_MASK) >> 16;
}

void getIoAddr(uint32_t *ioaddr, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum)
{
    uint32_t addr;
    uint32_t data;

    addr = MK_PCICFGADDR(u8BusNum, u8DevNum, u8FuncNum);

    outpd(PCI_CONFIG_ADDRESS, addr+0x10);
    data = inpd(PCI_CONFIG_DATA);

    *ioaddr = data;
}

void getPowerMgmtCtrl(uint16_t *pmctlvalue, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum)
{
    uint32_t addr;
    uint16_t data;

    addr = MK_PCICFGADDR(u8BusNum, u8DevNum, u8FuncNum);

    outpd(PCI_CONFIG_ADDRESS, addr+0xe0);
    data = inpw(PCI_CONFIG_DATA);

    *pmctlvalue = data;
}

void setPowerMgmtCtrl(uint16_t pmctlvalue, uint8_t u8BusNum, uint8_t u8DevNum, uint8_t u8FuncNum)
{
    uint32_t addr;

    addr = MK_PCICFGADDR(u8BusNum, u8DevNum, u8FuncNum);

    outpd(PCI_CONFIG_ADDRESS, addr+0xe0);
    outpw(PCI_CONFIG_DATA, pmctlvalue);

}


void scanPciBus(struct cardDetails cards[])
{
    uint16_t bus = 0;
    uint16_t dev = 0;
    uint16_t func = 0;
    uint16_t venId = 0;
    uint16_t devId = 0;

    uint32_t ioaddr = 0;

    int counter = 0;
    int i = 0;
    int found = 0;

    for (bus = 0; bus <= PCI_BUS_MAX; ++bus)
    {
        for (dev = 0; dev <= PCI_DEVICE_MAX; ++dev)
        {
            for (func = 0; func <= PCI_FUNCTION_MAX; ++func)
            {
                getPciDev(&venId, &devId, bus, dev, func);
                if (venId == _3COM_VENDOR && devId == _3C905_DEV) {
                  //printf("%s:%03d:%03d:%03d - vendor %04x device %04x\n", __FUNCTION__, bus, dev, func, venId, devId);
                  getIoAddr(&ioaddr, bus, dev, func);

                  // TODO: Am just masking out bottom bit of I/O Address register, should check for memory mapped vs. not
                  found = 0;
                  for (i = 0; i < counter; i++) {
                      if (cards[i].ioAddress == (ioaddr & 0xFFFE)) {
                          found = 1;
                          break;
                      }
                  }
                  // Add card to the list if not found
                  if (!found) {
                      cards[counter].u8BusNum = bus;
                      cards[counter].u8DevNum = dev;
                      cards[counter].u8FuncNum = func;
                      cards[counter++].ioAddress = (ioaddr & 0xFFFE);
                  }
               }
            }
        }
    }
}
