#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
TOC (table of contents)
- only needed in GP devices while using MMC RAW mode
- consists of a max of 2 items
  - second TOC item must be filled by FFh
- each TOC item contains information required by the ROM Code to find a valid
  image in RAW mode
- to detect RAW mode, the ROM also needs the magic values:
offset  value
40h     0xC0C0C0C1
44h     0x00000100
- if the sector contains a valid CHSETTINGS item , the ROM reads the GP header
  in the NEXT 512-byte section, uses its size and destination info to download
  the image to the destination address and jumps to the destination address 
  to complete the boot

TOC item fields: 26-38 in AM335x TRM
+---------------------------------+ 00h
|                TOC              | - 1st item contains data as per am335x TRM 26.1.11
|   - Configuration Header (CH)   | - 2nd section filled by FFh
|   - magic values                |
+---------------------------------+ next 512 byte section @ 200h
|             GP header           | 4 bytes for image size
|                                 | 4 bytes for addr where to store image / entry point
+---------------------------------+
|              Image              |
|                .                |
|                .                |
|                .                |
+---------------------------------+

*/
int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: %s <path_to_boot_image> <path_to_output_SPL_file>\n", argv[0]);
  }

  uint32_t BUF_SIZE = 12;
  char buf[BUF_SIZE];
  uint32_t TOC_ITEM_SIZE = 32;
  char toc_empty_sector[TOC_ITEM_SIZE];
  uint32_t SECTOR_SIZE = 512;
  char sector[SECTOR_SIZE];
  size_t bytes_written;
  int err, i;
  FILE *in_fptr, *out_fptr;
  uint8_t *image, *p;
  long in_fptr_size;

  char *boot_img_path = argv[1];

  out_fptr = fopen(argv[2], "w");
  if (out_fptr == 0) {
    printf("ERROR: failed creating new file");
    exit(1);
  }
  
  /* table of contents */
  uint32_t start = 0x00000040; 
  uint32_t size = 0x0000000C;
  uint32_t magic_number_0 = 0xC0C0C0C1;
  uint32_t magic_number_1 = 0x00000100;
  uint32_t img_destination_addr = 0x402F0400; /* dest address: public RAM mem map ranges from 402F0400h to 4030FFFFh (26.1.4.2) */

  bytes_written = fwrite(&start, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }
  bytes_written = fwrite(&size, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  memset(buf, 0, BUF_SIZE);
  bytes_written = fwrite(&buf, sizeof(buf[0]), BUF_SIZE, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  memcpy(buf, "CHSETTINGS\0", BUF_SIZE);
  bytes_written = fwrite(&buf, sizeof(buf[0]), BUF_SIZE, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  memset(toc_empty_sector, 0xFF, TOC_ITEM_SIZE);
  bytes_written = fwrite(&toc_empty_sector, TOC_ITEM_SIZE, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  /* magic numbers */
  bytes_written = fwrite(&magic_number_0, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  bytes_written = fwrite(&magic_number_1, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  /* zero out the rest of the sector */
  memset(sector, 0, SECTOR_SIZE);
  bytes_written = fwrite(&sector, sizeof(sector[0]), SECTOR_SIZE - 0x40 - 0x8, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  in_fptr = fopen(argv[1], "r");
  if (in_fptr == 0) {
    printf("ERROR: failed reading temp file");
    exit(1);
  }

  fseek(in_fptr, 0L, SEEK_END);
  in_fptr_size = ftell(in_fptr);
  if (in_fptr_size == -1) {
    printf("ERROR: failed getting file position\n");
    exit(1);
  }

  /* GP header */
  printf("image size: %ld\n", in_fptr_size);

  fseek(in_fptr, 0L, SEEK_SET);
  image = (uint8_t*)malloc(in_fptr_size);
  fread(image, 1, in_fptr_size, in_fptr);

  bytes_written = fwrite(&in_fptr_size, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  bytes_written = fwrite(&img_destination_addr, 4, 1, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  bytes_written = fwrite(image, sizeof(image[0]), in_fptr_size, out_fptr);
  if (bytes_written == 0) {
    printf("ERROR: failed writing to MLO\n");
    exit(1);
  }

  fclose(out_fptr);
}
