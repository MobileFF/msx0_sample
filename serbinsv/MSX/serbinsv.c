#include <stdlib.h>
#include <stdio.h>
#include "mymsx.h"

void main() {
    msx_println("=== SERrial BINnary file SaVe ===");

    char* filename = msx_input_with_prompt("FILENAME=");
    char drive = 0; // default drive
    unsigned short int record_size = 1;

    // FCB、DMA作成
    char* fcb = msx_create_fcb(drive,filename);
    char* dma = msx_create_dma_buffer(record_size);

    // FILE CREATE
    int result1 = msx_create_file(fcb);
    if (result1==0xFF) {
        msx_print("CREATE FILE FAILED:");
        msx_println(filename);
        exit(1);
    }

    // FILE OPEN
    int result2 = msx_open_file(fcb);
    if (result2==0xFF) {
        msx_print("OPEN FILE FAILED:");
        msx_println(filename);
        exit(1);
    }

   	// レコードサイズを設定
	fcb[15] = (char)(record_size >> 8);
	fcb[14] = (char)(record_size & 0x00FF);

	// ランダムアクセスレコードを０で初期化
	for(int i=33;i<=36;i++) {
		fcb[i] = 0;
	}

    // SET DMA
    msx_set_dma_address(dma);

    // RAMDOM WRITE
    unsigned int count = 0;
    char* input = "00";
    do {

        do {
            input[0] = msx_chget();
        } while(input[0]==0x0D || input[0]==0x0A || input[0]==0x20);

        if (input[0]=='.') {
            break;
        }

        do {
            input[1] = msx_chget();
        } while(input[1]==0x0D || input[1]==0x0A || input[1]==0x20);

        if (input[1]=='.') {
            break;
        }

        dma[0] = (char)strtol(input,NULL,16);
        int result3 = msx_file_write_rnd(fcb);
        if(result3==0xFF) {
            msx_println("RANDOM WRITE FAILED");
            exit(1);
        }
        count++;
        if(count%1000==0) {
            msx_chput('.');
        }
    } while(1);
    msx_chput_CRLF();

    // FILE CLOSE
    int result4 = msx_file_close(fcb);
    if(result4==0xFF) {
        msx_println("FILE CLOSE FAILED");
        exit(1);
    }
    char sizeStr[8] = "0000000";
    itoa(count,sizeStr,10);
    msx_print(sizeStr);
    msx_println(" bytes received and saved.");
}
