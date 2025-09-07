#define RxStation 1
#define RxMulticast 2
#define RxBroadcast 4
#define RxProm 8

#define SetRxFilter  0x8000
#define RxEnable     0x2000
#define SelectWindow 0x0800
#define EL3_CMD      0x0e

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdint.h>
#include <string.h>
#include "pci.h"

// Write a word to the I/O Device
void iowrite16(unsigned int cmd, int addr) {
    outpw(addr, cmd);
}

// Read a word from the I/O Device
unsigned int ioread16(int addr) {
    return inpw(addr);
}

// Set the Wake On LAN Status
void setWol(struct cardDetails card) {
    unsigned int powerMgmtEvent = 0;
    unsigned int ioBase = card.ioAddress;
    unsigned short pmctlvalue;

    // Derived from the 3COM Spec at https://people.freebsd.org/~wpaul/3Com/3c90xb.pdf

    // TODO add window 7 IntStatus check that verifies that this is really a valid card

    // Select window 7, which contains the PowerMgmtEvent at offset 0xc.
    // Read it and print it out
    iowrite16(SelectWindow + 7, ioBase + EL3_CMD);
    powerMgmtEvent=ioread16(ioBase + 0xc);
    printf("Old Value of PowerMgmtEvent for card at I/O Address 0x%04x is %d\n", ioBase, powerMgmtEvent);

    // Set bit 1 at offset 0xc.  We are currently in window 7.
    // TODO define this as a constant somewhere.
    iowrite16(2, ioBase + 0xc);

    // Read PowerMgmtEvent back out to confirm!
    powerMgmtEvent=ioread16(ioBase + 0xc);
    printf("New Value of PowerMgmtEvent for card at I/O Address 0x%04x is %d\n", ioBase, powerMgmtEvent);

    // Set up filters to look for magic packet!
    iowrite16(SetRxFilter|RxStation|RxMulticast|RxBroadcast, ioBase + EL3_CMD);
    iowrite16(RxEnable, ioBase + EL3_CMD);

    // Set bit 8 in the PowerMgmtCtrl register (pmeEn) to enable the card to report wakeup events to
    // the power management event signal
    getPowerMgmtCtrl(&pmctlvalue, card.u8BusNum, card.u8DevNum, card.u8FuncNum);
    printf("\nOld value for PowerMgmtCtrl for card at I/O Address 0x%04x is 0x%04x\n", card.ioAddress, pmctlvalue);
    setPowerMgmtCtrl(pmctlvalue | 0x0100, card.u8BusNum, card.u8DevNum, card.u8FuncNum);
    getPowerMgmtCtrl(&pmctlvalue, card.u8BusNum, card.u8DevNum, card.u8FuncNum);
    printf("New value for PowerMgmtCtrl for card at I/O Address 0x%04x is 0x%04x\n", card.ioAddress, pmctlvalue);
}

// Main method
int main(int argc, char **argv) {
    // ASSUMPTION: Never more than 10 NIC cards in a system
    struct cardDetails cards[10];
    int counter = 0;

    memset(cards, 0, sizeof(cards));
    printf("Trying to detect I/O base\n\n");
    scanPciBus(cards);

    for (counter=0; counter < (sizeof(cards)/sizeof(cards[0])); counter++) {
        struct cardDetails card = cards[counter];
        if (card.ioAddress != 0) {
            printf("Found 3C905-TX at 0x%04x\n\n", card.ioAddress);
            setWol(card);
        }
    }
    return 0;
}
