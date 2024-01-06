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

    char* filenameInput = msx_input_with_prompt("FILENAME");
    char filename[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i=0;i<12;i++) {
        if(filenameInput[i]==0) { break; }
        filename[i]=filenameInput[i];
    }
    // char* driveStr = msx_input_with_prompt("DRIVE(DEFAULT=0/A=1/B=2/C=3)");
    // int drive = 0x30-driveStr[0];
    int drive = 0;

    // スロット切り替え
    // msx_println("ENASLT");
    unsigned char slot = bpeek(0xF342);
    msx_enaslt(slot,0x4000); // [01]000000 = PAGE 1
    unsigned char slot2 = bpeek(0xF343);
    msx_enaslt(slot2,0x8000); // [10]000000 = PAGE 2
    // msx_enaslt(slot,0x80);

    // FCB、DMA作成
    // msx_println("FCB/DMA CREATE");
    unsigned short int record_size = 1024;
    char* fcb = msx_assign_memory_to_fcb(0xe000,drive,filename);
    char* dma = msx_assign_memory_to_dma_buffer(0xe000+37);

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
        msx_print("*");
        int result3 = msx_file_read_rnd(fcb);
        if(result3==0xFF) {
            msx_println("RANDOM READ FAILED");
            msx_restore_slot(pri_slot_addr,sec_slot_addr);
            exit(1);
        }
        msx_ldir_dma2mem(dma,record_size,write_address);
    }
    msx_chput_CRLF();

    // msx_println("FILE CLOSE");
    int result4 = msx_file_close(fcb);
    if(result4==0xFF) {
        msx_println("FILE CLOSE FAILED");
        msx_restore_slot(pri_slot_addr,sec_slot_addr);
        exit(1);
    }

    unsigned short int rom_init = wpeek(0x4002);

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
