#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"

#define FILE_PIC_PATH "/home/Sebastian_arjun/doorbell-ArjunSingh3/viewer/%s\0"

/*
Takes a picture with the included libcamera-ecen224 binary
uint8_t * buf: a buffer where the image data of the photo taken will be stored
size_t bufsize: integer that holds the size of the buffer. The size of the photo
being taken is found as a #DEFINEd variable in "camera.h"
*/
void camera_capture_data(uint8_t * buf, size_t bufsize)
{
    system("libcamera-still --width=128 --height=128 -ebmp -n -o ecen224-lab-camera.bmp");

    FILE *fp;

    fp = fopen("ecen224-lab-camera.bmp", "rb");

    fread(buf, sizeof(uint8_t), bufsize, fp);

    fclose(fp);

    system("rm -f ecen224-lab-camera.bmp");
}

/*
Takes the photo data retrieved from the previous function and saves it to a file
uint8_t * buf: a buffer where the image data of the photo taken is stored
size_t bufsize: integer that holds the size of the buffer. The size of the photo
being taken is found as a #DEFINEd variable in "camera.h"
char * filename: name of the file that is being saved
*/
void camera_save_to_file(uint8_t * buf, size_t bufsize, char * filename)
{
    	/*
    	YOUR CODE HERE
    	*/
	
	FILE *fp;
	printf("filename: %s\n", filename);
	char file_location[100];
	sprintf(file_location,FILE_PIC_PATH,filename);
	printf("fileName: %s\n",file_location);
	fp = fopen(file_location,"w+");
	// Double check the size in bytes of each element to be writen -> It is 1 right now, but could be different??????????????????????
	fwrite(buf,1,bufsize,fp);
	fclose(fp);
    
}
