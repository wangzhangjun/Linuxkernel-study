#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define SECTOR_SIZE 512
#define MBR_SIZE SECTOR_SIZE
#define MBR_DISK_SIGNATURE_OFFSET 440 
#define MBR_DISK_SIGNATURE_SIZE 4 
#define PARTITION_TABLE_OFFSET 446 
#define PARTITION_ENTRY_SIZE 16 // sizeof(PartEntry)
#define PARTITION_TABLE_SIZE 64 // sizeof(PartTable)
#define MBR_SIGNATURE_OFFSET 510 
#define MBR_SIGNATURE_SIZE 2 
#define MBR_SIGNATURE 0xAA55 
#define BR_SIZE SECTOR_SIZE 
#define BR_SIGNATURE_OFFSET 510 
#define BR_SIGNATURE_SIZE 2 
#define BR_SIGNATURE 0xAA55

typedef struct {
	unsigned char boot_type; // 0x00 - Inactive; 0x80 - Active (Bootable) 
	unsigned char start_head;
	unsigned char start_sec:6;   
        unsigned char start_cyl_hi:2;
        unsigned char start_cyl;
        unsigned char part_type; 
        unsigned char end_head;  
        unsigned char end_sec:6;
        unsigned char end_cyl_hi:2; 
        unsigned char end_cyl; 
        unsigned long abs_start_sec;   
        unsigned long sec_in_part; 
} PartEntry; 

typedef struct {
        unsigned char boot_code[MBR_DISK_SIGNATURE_OFFSET]; // to offset 440
        unsigned long disk_signature; // to offset 444
        unsigned short pad;   // to offset 446
        unsigned char pt[PARTITION_TABLE_SIZE]; //offset 446 + 4*16 = 510
        unsigned short signature; // to offset 512
} MBR;


void print_computed(unsigned long sector) {
        unsigned long heads, cyls, tracks, sectors;
        sectors = sector % 63 + 1 /* As indexed from 1 */;   
        tracks = sector / 63; 
        cyls = tracks / 255 + 1 /* As indexed from 1 */;
        heads = tracks % 255;  
        printf("(%3d/%5d/%1d)", heads, cyls, sectors); 
} 

int main(int argc, char *argv[]) { 
        char *dev_file = "/dev/sda";  
        int fd, i, rd_val;   
        MBR m;   
        PartEntry *p = (PartEntry *)(m.pt);   
        if (argc == 2) {
        	dev_file = argv[1];     
	}
	if ((fd = open(dev_file, O_RDONLY)) == -1) {    
        	fprintf(stderr, "Failed opening %s: ", dev_file); 
                perror("");    
                return 1;   
        } 
        if ((rd_val = read(fd, &m, sizeof(m))) != sizeof(m)) {
        	fprintf(stderr, "Failed reading %s: ", dev_file);
	        perror("");   
                close(fd); 
                return 2; 
        }
        close(fd);   
        printf("\nDOS type Partition Table of %s:\n", dev_file);
        printf("  B Start (H/C/S)   End (H/C/S) Type  StartSec    TotSec\n");   
        for (i = 0; i < 4; i++) {
        	printf("%d:%d (%3d/%4d/%2d) (%3d/%4d/%2d)  %02X %10d %9d\n",  
	        	i + 1, !!(p[i].boot_type & 0x80),   
                        p[i].start_head,
                        1 + ((p[i].start_cyl_hi << 8) | p[i].start_cyl),     
                        p[i].start_sec,
                        p[i].end_head,
                        1 + ((p[i].end_cyl_hi << 8) | p[i].end_cyl),    
                        p[i].end_sec, 
                        p[i].part_type,   
                        p[i].abs_start_sec, p[i].sec_in_part);     
        } 
	printf("\nRe-computed Partition Table of %s:\n", dev_file); 
        printf("  B Start (H/C/S)   End (H/C/S) Type  StartSec    TotSec\n");    
        for (i = 0; i < 4; i++) {   
  	      printf("%d:%d ", i + 1, !!(p[i].boot_type & 0x80));
              print_computed(p[i].abs_start_sec); 
              printf(" ");  
              print_computed(p[i].abs_start_sec + p[i].sec_in_part - 1); 
              printf(" %02X %10d %9d\n", p[i].part_type,   
              	p[i].abs_start_sec, p[i].sec_in_part);    
 	}
        printf("\n"); 
        return 0; 
} 

