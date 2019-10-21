/*
 * lcdtest.c
 *
 *  Created on: May 22, 2017
 *      Author: johnsontimoj
 *
 *      rev 1.1 9/13/17 johnsontimoj
 *          added test for contrast
 */

#include <stdio.h>
#include "msp432.h"
#include "msoe_lib_lcd.h"

#include "pu1.h"
#include "msoe1.h"
#include "bat1.h"
#include "test_bmp.h"


int main(void){
	int test = 0;
    LCD_Config();
	while(1){
		printf("Enter the number for the test you want to run.\n");
		printf("1 - small text\n");
		printf("2 - big text\n");
		printf("3 - bit map\n");
		printf("4 - binary\n");
		printf("5 - hex\n");
		printf("6 - unsigned decimal\n");
		printf("7 - signed decimal\n");
		printf("8 - float\n");
        printf("9 - contrast\n");
		scanf("%i", &test);
		printf("\n");

		switch (test){
		case 1:{
			LCD_clear();
			char foo = '0';
			int i;
			for(i=0; i<20; i++){
				LCD_print_char(foo++);
				_delay_cycles(230000);
			} // end for

			LCD_goto_xy(3,3);
			LCD_print_char('r');
			LCD_goto_xy(5,5);
			LCD_print_char('s');
			LCD_row(4);
			LCD_print_char('t');
			LCD_col(3);
			LCD_print_char('u');

			LCD_goto_xy(9,1);
			char boo[8] = "testing";
			LCD_print_str(boo);
			break;
		}
		case 2:{
			LCD_clear();
			LCD_print_bigchar(0, 0,'G');
			LCD_print_bigchar(7, 1,'B');
			char soo[12] = "HELLO WORLD";
			LCD_print_bigstr(6, 0, soo);
			LCD_print_bigstr(2, 2, soo);
			break;
		}
		case 3:
			LCD_clear();
			LCD_print_bmpArray(pu1);

			_delay_cycles(2300000);
			LCD_clear();
			LCD_print_bmpArray(msoe1);

			_delay_cycles(2300000);
			LCD_clear();
			LCD_print_bmpArray(bat1);

			_delay_cycles(2300000);
			LCD_clear();
			LCD_print_bmpArray(test_bmp);
			break;
		case 4:{
			LCD_clear();
			uint8_t bin8 = 0;
            uint16_t bin16 = 0;
            LCD_print_bin8(bin8);
            LCD_goto_xy(0,1);
            LCD_print_bin16(bin16);
            bin8 = 0xA5;
			bin16 = 0xA5A5;
            LCD_goto_xy(0,3);
            LCD_print_bin8(bin8);
			LCD_goto_xy(0,4);
			LCD_print_bin16(bin16);
			break;
		}
		case 5:{
			LCD_clear();
			uint8_t hex8 = 0x0;
            uint16_t hex16 = 0x0;
            uint32_t hex32 = 0x0;
            LCD_print_hex8(hex8);
            LCD_goto_xy(0,1);
            LCD_print_hex16(hex16);
            LCD_goto_xy(0,2);
            LCD_print_hex32(hex32);
            hex8 = 0xA5;
			hex16 = 0xA5A5;
            hex32 = 0x12345A5A;
			LCD_goto_xy(0,3);
			LCD_print_hex8(hex8);
			LCD_goto_xy(0,4);
			LCD_print_hex16(hex16);
            LCD_goto_xy(0,5);
            LCD_print_hex32(hex32);
            break;
		}
		case 6:{
			LCD_clear();
			uint8_t udec3 = 0;
            uint16_t udec5 = 0;
            uint32_t udec10 = 0;
            LCD_goto_xy(0,0);
            LCD_print_udec3(udec3);
            LCD_goto_xy(0,1);
            LCD_print_udec5(udec5);
            LCD_goto_xy(0,2);
            LCD_print_udec10(udec10);
            udec3 = 255;
			udec5 = 65123;
			udec10 = 0xA5A5A5A5;
			LCD_goto_xy(0,3);
			LCD_print_udec3(udec3);
			LCD_goto_xy(0,4);
			LCD_print_udec5(udec5);
			LCD_goto_xy(0,5);
			LCD_print_udec10(udec10);
			break;
		}
		case 7:{
			LCD_clear();
			int8_t dec3 = 0;
            int16_t dec5 = 0;
            int32_t dec10 = 0;
            LCD_goto_xy(0,0);
            LCD_print_dec3(dec3);
            LCD_goto_xy(0,1);
            LCD_print_dec5(dec5);
            LCD_goto_xy(0,2);
            LCD_print_dec10(dec10);
            dec3 = -127;
			dec5 = -32000;
			dec10 = -1929292929;
			LCD_goto_xy(0,3);
			LCD_print_dec3(dec3);
			LCD_goto_xy(0,4);
			LCD_print_dec5(dec5);
			LCD_goto_xy(0,5);
			LCD_print_dec10(dec10);
			break;
		}
		case 8:{
			LCD_clear();
			float test2;
			int j = 0;
			LCD_goto_xy(0,j);
			if(j==5)
				j=0;
			else
				j++;
			printf("enter a value: ");
			scanf("%f", &test2);
			LCD_print_float(test2);
			printf("\n");
			break;
		}
        case 9:{
            uint8_t cont_val;
            printf("enter a contrast value 0-10: ");
            scanf("%c", &cont_val);
            LCD_contrast(cont_val);
            printf("\n");
            break;
        }
		default:
			break;
		} // end switch
	} // end while
} // end main
