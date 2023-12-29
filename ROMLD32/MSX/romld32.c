#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mymsx.h"

void main() {
    msx_println("=== ROMLOAD(32K) ===");

	// IN	A,(0A8H) ; I/O A8H = PRIMARY SLOT STATUS
    unsigned char pri_slot_addr = inp(0xA8);

	// LD	A,(0FFFFH) ; (FFFFH) = SECONDARY SLOT STATUS
	// CPL          ; CPL=1の補数(ビット反転)
    unsigned char sec_slot_addr = ~bpeek(0xFFFF) & 0xFFFF;

    msx_print("PRIMARY   SLOT=");
    char priBinString[9]={0,0,0,0,0,0,0,0,0};
    unsigned int priLen = itoa(pri_slot_addr,priBinString,2) - priBinString;

    for(int i=0;i<8-priLen;i++) {
        msx_chput('0');
    }
    msx_println(priBinString);

    msx_print("SECONDARY SLOT=");
    char secBinString[9]={0,0,0,0,0,0,0,0,0};
    unsigned int secLen = itoa(sec_slot_addr,secBinString,2) - secBinString;

    for(int i=0;i<8-secLen;i++) {
        msx_chput('0');
    }
    msx_println(secBinString);

    char* exStr = msx_input_with_prompt("EXTEND(Y=1/N=0)");
    unsigned char ex = exStr[0];

    char* prStr = msx_input_with_prompt("PRIMARY(0-3)");
    unsigned char pr = prStr[0];

    unsigned char sc = 0;
    if (ex=='1') {
        char* scStr = msx_input_with_prompt("SECONDARY(0-3)");
        sc = scStr[0];
    }
    char* filenameInput = msx_input_with_prompt("FILENAME");
    char filename[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i=0;i<12;i++) {
        if(filenameInput[i]==0) { break; }
        filename[i]=filenameInput[i];
    }
    // char* driveStr = msx_input_with_prompt("DRIVE(DEFAULT=0/A=1/B=2/C=3)");
    // int drive = 0x30-driveStr[0];
    int drive = 0;

    char* slotBin = "00000000";
    slotBin[0] = ex;
    if (sc=='1' || sc=='3') {
        slotBin[5] = '1';
    }
    if (sc=='2' || sc=='3') {
        slotBin[4] = '1';
    }
    if (pr=='1' || pr=='3') {
        slotBin[7] = '1';
    }
    if (pr=='2' || pr=='3') {
        slotBin[6] = '1';
    }

    unsigned char slot = 0;
    for(int i=0;i<8;i++) {
        if(slotBin[i]=='1') {
            slot = slot + pow(2,7-i);
        }
    }
    msx_println(slotBin);

    // char* slotStr = "000";
    // itoa(slot,slotStr,10);
    // msx_println(slotStr);

    // for(int i=0;i<16;i++) {
    //     char* a = "00";
    //     itoa((unsigned char)bpeek(0x4000+i),a,16);
    //     msx_print(a);
    //     msx_print(" ");
    // }
    // msx_chput_CRLF();


    // スロット切り替え
    // msx_println("ENASLT");
    msx_enaslt(slot,0x4000); // [01]000000 = PAGE 1
    msx_enaslt(slot,0x8000); // [10]000000 = PAGE 2
    // msx_enaslt(slot,0x80);

    // FCB、DMA作成
    // msx_println("FCB/DMA CREATE");
    unsigned short int record_size = 1024;
    char* fcb = msx_assign_memory_to_fcb(0xe000,drive,filename);
    char* dma = msx_assign_memory_to_dma_buffer(0xe000+37);

    // char* fcbStr="0000";
    // char* dmaStr="0000";
    // itoa(fcb,fcbStr,16);
    // itoa(dma,dmaStr,16);
    // msx_println(fcbStr);
    // msx_println(dmaStr);


    // msx_println("FILE OPEN");
    int result2 = msx_open_file(fcb);
    if (result2==0xFF) {
        msx_print("OPEN FILE FAILED:");
        msx_println(filename);
        msx_restore_slot(pri_slot_addr,sec_slot_addr);
        exit(1);
    }

   	// レコードサイズを設定
	fcb[15] = (char)(record_size >> 8);
	fcb[14] = (char)(record_size & 0x00FF);

	// ランダムアクセスレコードを０で初期化
	for(int i=33;i<=36;i++) {
		fcb[i] = 0;
	}

    // msx_println("SET DMA");
    msx_set_dma_address(dma);

    msx_println("          1111111111222222222233");
    msx_println("01234567890123456789012345678901");
    // msx_println("RAMDOM WRITE");
    for(unsigned short int write_address = 0x4000;write_address<0xC000;write_address+=record_size) {
        // char ra[5] = "0000";
        // itoa(read_address,ra,16);
        // msx_print(ra);
        // msx_print(":");
        msx_print("*");
        int result3 = msx_file_read_rnd(fcb);
        if(result3==0xFF) {
            msx_println("RANDOM READ FAILED");
            msx_restore_slot(pri_slot_addr,sec_slot_addr);
            exit(1);
        }
        // msx_print("#");
        msx_ldir_dma2mem(dma,record_size,write_address);
    }
    msx_chput_CRLF();

    // for(int i=0;i<16;i++) {
    //     char* a = "00";
    //     itoa((unsigned char)bpeek(0x4000+i),a,16);
    //     msx_print(a);
    //     msx_print(" ");
    // }
    // msx_chput_CRLF();

    // msx_println("FILE CLOSE");
    int result4 = msx_file_close(fcb);
    if(result4==0xFF) {
        msx_println("FILE CLOSE FAILED");
        msx_restore_slot(pri_slot_addr,sec_slot_addr);
        exit(1);
    }

    // char* fcbStr="0000";
    // char* dmaStr="0000";
    // itoa(fcb,fcbStr,16);
    // itoa(dma,dmaStr,16);
    // msx_println(fcbStr);
    // msx_println(dmaStr);

    unsigned short int rom_init = wpeek(0x4002);
    // msx_print("INIT:");
    // char* romInitStr = "0000";
    // itoa(rom_init,romInitStr,16);
    // msx_println(romInitStr);

    if (rom_init>0) {
        msx_print("PRESS ANY KEY TO JUMP ROM INIT[");
        char* romInitStr = "0000";
        itoa(rom_init,romInitStr,16);
        msx_print(romInitStr);
        msx_println("](ESC=Exit).");
        char c = msx_chget();
        if(c==0x1B) {
            msx_restore_slot(pri_slot_addr,sec_slot_addr);
            exit(0);
        }
        msx_calslt(slot<<8,rom_init);
    } else {
        msx_println("PRESS ANY KEY TO RESET(ESC=Exit).");
        char c = msx_chget();
        if(c==0x1B) {
            msx_restore_slot(pri_slot_addr,sec_slot_addr);
            exit(0);
        }
        msx_soft_reset();
    }
}
