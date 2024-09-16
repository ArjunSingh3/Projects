#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "buttons.h"
#include "device.h"
#include "display.h"
#include "log.h"
#include "fonts/fonts.h"
#include <dirent.h>
#include <string.h>
#include "camera.h"
#include "client.h"
#include "pthread.h"
/* DEFAULT COLORS */
#define BACKGROUND_COLOR WHITE
#define FONT_COLOR BLACK
#define IMG_SIZE 49206
#define FALSE 0
#define TRUE 1

void developersView();

#define FILE_PATH "/home/Sebastian_arjun/doorbell-ArjunSingh3"
#define FILE_PIC_PATH "/home/Sebastian_arjun/doorbell-ArjunSingh3/viewer/%s\0"
#define FILE_PIC_PATH_2 "/home/Sebastian_arjun/doorbell-ArjunSingh3/viewer/"

char fileSet[20][255]; 
static bool isSending =FALSE; 
static bool isSent = FALSE;
static bool duplicate_sent = FALSE;
static bool duplicate_sending = FALSE;
int select_iterator = 1;
int num_pics = 0;
bool drawn = false;

void *sent_timeout(void *time){
	sleep(*(int*)time);
	isSent = false;
	drawn=false;
}

void *send_image(void *arg){
	isSending = TRUE;
	delay_ms(10000);
	
	Config configuration;
	configuration.port = "2225";
	configuration.host = "netlab.et.byu.edu";
	configuration.hw_id = "BF8BDEAFC";

	uint8_t * client_buf = (uint8_t *)malloc(sizeof(uint8_t)*IMG_SIZE);
	FILE *fp;
	char pic_location[150];
	sprintf(pic_location,FILE_PIC_PATH,fileSet[(*(int *)arg)]);
	printf("filename: %s\n",pic_location);
	fp = fopen(pic_location,"r");
	if(fp == NULL){
		printf("error! location not found!!");
		exit(0);
	}

	fread(client_buf, sizeof(uint8_t),IMG_SIZE,fp);
	printf("image Data in main: \n%s\n",client_buf);
	//configuration.payload = client_buf;

	//for(int i = 0; i < IMG_SIZE; i++)

	//	printf("%x", configuration.payload[i]);

	int socket_File_Discriptor_number = client_connect(configuration);
	printf("Hello there made a connection, now sending");
	configuration.payload = client_buf;
	printf("%d\n",sizeof(client_buf));
	client_send_image(socket_File_Discriptor_number,client_buf,configuration.hw_id,IMG_SIZE);
	client_close(socket_File_Discriptor_number);
	free(client_buf);
	isSending = FALSE;
	isSent = TRUE;
}

void intHandler(int dummy){
	log_info("Ctl-c caught! Stopping...");

	display_exit();
	exit(0);
}
/*
 *This function takes in parameters of a 2d array and its length
 * they reload the file menu on the display whenever called
 * */
void draw_list(char array[][255], int length){
	
	for(int l=0; l<=length; l-=-1){	
		display_draw_string(10,2+(8*l),array[l], &Font8, WHITE, RED);	
	}
}

void readDirectory(){
    	printf("Opening the directory");
	DIR *dp = opendir(FILE_PIC_PATH_2);
	printf("setting up the reading of the files");

	//char fileSet[20][255];	
        bool is_file = true;
	struct dirent *dir;
	int i = 1;
//	its infinite loop to iterate over the viewer directory and read all the file names from it
	while(is_file){
		//printf("in the while loop");
		dir = readdir(dp);
		if(dir == NULL){
			//printf("End Of Folder!!!!\n");
			is_file = false;
			break;
		}
		char file_name[255];
		sprintf(file_name,"%s\0",dir->d_name);

 		if(!strcmp(strrchr(file_name,'\0')-4,".bmp") || !strcmp(strrchr(file_name,'\0')-4,".log")){
			sprintf(fileSet[i],"%s\0",file_name);
			i++;
		}

	}

    	closedir(dp);
}
// Makes sure to deinitialize everything before program close

int main(void) {

    signal(SIGINT, intHandler);

    // Initialize the hardware here
    log_info("Starting...");
    /* place lines here*/
    delay_ms(1000);

    // Add file scanning logic here
	// Initializes all the buttons and the display
	printf("initializing displays and buttons");
    display_init();
    buttons_init();
    display_clear(WHITE);



  // DIR *dp = opendir("./viewer");

  //      char fileSet[20][255];	
  //      bool is_file = true;
  //      struct dirent *dir;
  //      int i = 1;
////	its infinite loop to iterate over the viewer directory and read all the file names from it
  //      while(is_file){
  //      	dir = readdir(dp);
  //      	if(dir == NULL){
  //      		printf("End Of Folder!!!!\n");
  //      		is_file = false;
  //      		break;
  //      	}
  //      	char file_name[255];
  //      	sprintf(file_name,"%s\0",dir->d_name);

  //      	if(!strcmp(strrchr(file_name,'\0')-4,".bmp") || !strcmp(strrchr(file_name,'\0')-4,".log")){
  //      		sprintf(fileSet[i],"%s\0",file_name);
  //      		i++;
  //      	}

  //      }

  //  	closedir(dp);
	// Reads the new set of files in the directory needs to be done each time a picture is taken, such that if a new name is added, it can be detected
    	//printf("Calling the readDirectory function");
    	//readDirectory();
	drawn = false;

 
    // True loop keeps looping until quit the display
    while (true) {
        delay_ms(100);
	
		if(!drawn){
			drawn = true;
			display_clear(BLUE);
			display_draw_string(25,25,"WELCOME",&Font12,BLUE,RED);
		}
	
	    if(button_center() ==0){
			
			drawn = false;
			uint8_t * my_new_buf = (uint8_t *)malloc(sizeof(uint8_t)*IMG_SIZE);
			//num_pics = select_iterator;
			if(my_new_buf==NULL){
				printf("ERROR: Memory Not Allocated !!");
				exit(0);
			}

			printf("Successfully: Memory has been Allocated\n");

			camera_capture_data(my_new_buf,IMG_SIZE);
			char file_name[40];
			printf("%d\n", num_pics);
		//num_pics++;
			printf("%d\n", num_pics);
			num_pics = (num_pics % 5)+1;

			printf("%d\n", num_pics);
			//num_pics++;
			sprintf(file_name,"doorbell-%d.bmp\0",num_pics);
			//num_pics++;
		//	sprintf(file_name,"doorbell-%d.bmp\0",num_pics);
			printf("%s\n",file_name);
			camera_save_to_file(my_new_buf,IMG_SIZE,file_name);
				
			free(my_new_buf);
			sprintf(fileSet[num_pics],"%s\0",file_name);
			char file_location[40];
			sprintf(file_location,FILE_PIC_PATH,file_name);
			//display_draw_image(file_location);
			delay(1000);
			//display_clear(WHITE);
		//	readDirectory();
			//draw_list(fileSet, 5);
			//display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);
			//display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);
			
			
			int * i = (int *) malloc(sizeof(int));
			*i = num_pics;
			//send_image((void *)i);
			pthread_t send_image_thread;
			pthread_create(&send_image_thread,NULL,send_image,(void*)i);
		}else if(button_right() ==0){
			printf("upButton_Pushed\n");
			delay(200);
			if(button_right()==1){
				printf("upButtonLeft\n");
				delay(200);
				if(button_right() ==0){
					printf("upButton_Pushed\n");
					delay(200);
					if(button_right()==1){
						printf("upButtonLeft\n");
						delay(200);
						if(button_left()==0){
							printf("downButton_Pushed\n");
							developersView();
							drawn = false;
						}
					}
				}
			}
		}

		
		if(isSending && !duplicate_sending){
			duplicate_sending = TRUE;
			duplicate_sent = FALSE;
			display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_WIDTH,BLUE,true,1);
			display_draw_string(10,120,"Sending...", &Font8, BLUE, WHITE);
		}
		else if(isSent && !duplicate_sent){
			duplicate_sent = TRUE;
			duplicate_sending = FALSE;
			
			//display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_HEIGHT,GREEN,true,1);
			//display_draw_string(0,120,"Sent", &Font8, GREEN, WHITE);
			display_clear(GREEN);
			display_draw_string(25,25,"Ding! Dong!",&Font12,GREEN,RED);
			int *sleep_time = (int *)malloc(sizeof(int));
			*sleep_time = 5;		
			pthread_t sleep_thread;
			pthread_create(&sleep_thread,NULL,sent_timeout,(void *)sleep_time);
		}
		else if(!isSent && !isSending){
			duplicate_sent = FALSE;
			duplicate_sending = FALSE;
			if(!drawn){
				drawn = true;
				display_clear(BLUE);
				display_draw_string(25,25,"WELCOME",&Font12,BLUE,RED);
			}
			//display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_HEIGHT,BLUE,true,1);
		}

	}

    return 0;
}


void developersView(){

	printf("Calling the readDirectory function");
    	readDirectory();
		display_clear(WHITE);
	draw_list(fileSet, 5);

    // Everythin inside this loop will repeat until 'Ctrl-C' is pressed in the terminal.
    // Add drawing logic here
    select_iterator =1;
    int previous = 1;
    bool change = false;
    bool changed_current = true;

	num_pics = 0;
	while(true){
		delay(100);
	// Cehcks to update the new selected and the previous selected file
		if(changed_current){
			display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);	
			display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);	
			changed_current = false;
		
			display_draw_rectangle(0,2+(8*previous),127,((previous*8)+8+2),WHITE,true,1);
			display_draw_string(10,2+(8*previous),fileSet[previous], &Font8, WHITE, RED);	
		
			
		}

		// Checks which Buttons is pUshed

		if(button_left() == 0){
			changed_current = true;
			previous = select_iterator;
			change = true;
			if((select_iterator+1)>5){
				select_iterator = 1;
			}
			else
				select_iterator++;
			//display_draw_string(10,10+(12*j),fileSet[j], &Font12, BLUE, RED);	
		}
		else if(button_right() == 0){
			
			changed_current = true;
			previous = select_iterator;
			change = true;
			if((select_iterator-1)<1){
				select_iterator = 5;
			}
			else
				select_iterator--;
		}
		else if(button_down() == 0){
			
			//
			//display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),ORANGE,true,1);	
			//display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, ORANGE, BLUE);
			//delay(200);	
			//char final_file[80];
			//sprintf(final_file,FILE_PIC_PATH,fileSet[select_iterator]);
			//// cehcks if the file chosen is either a .bmp fle or a .log file
 			//if(!strcmp(strrchr(fileSet[select_iterator],'\0')-4,".bmp")){
			//	printf("picture name: %s\n",final_file);
			//	display_draw_image(final_file);
			//	delay(2000);	
			//	display_clear(WHITE);
			//	draw_list(fileSet, 5);
			//	display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);	
			//	display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);
			//}
			//else if(!strcmp(strrchr(fileSet[select_iterator],'\0')-4,".log")){
			//	char word[25];
			//	display_clear(ORANGE);
			//	int x =5;
			//	int y =5;
			//	FILE *fp;
			//	fp = fopen(final_file,"r+");
			//	while(true){
			//		int ret = fscanf(fp,"%s\0", word);
			//		if(ret == 1){
			//			if((y+10)>=125 && (x+(strlen(word)*6))>=120){
			//				display_draw_string(x,y,"....",&Font8,ORANGE,BLUE);
			//				break;
			//			}
			//			if((x+(strlen(word)*6))>=127){
			//				x = 5;
			//				y+= 10;
			//			}
			//			display_draw_string(x,y,word,&Font8,ORANGE,BLUE);
			//			x += strlen(word)*7;
			//		}
			//		else if(ret == EOF)
			//			break;
			//	}
			//	delay(2000);
			//	display_clear(WHITE);
			//	draw_list(fileSet, i);
			//	display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);	
			//	display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);
			//}
		}
		else if (button_center() ==0){
			
			display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),ORANGE,true,1);	
			display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, ORANGE, BLUE);
			delay(200);	
			char final_file[80];
			sprintf(final_file,FILE_PIC_PATH,fileSet[select_iterator]);
			// cehcks if the file chosen is either a .bmp fle or a .log file
 			if(!strcmp(strrchr(fileSet[select_iterator],'\0')-4,".bmp")){
				printf("picture name: %s\n",final_file);
				display_draw_image(final_file);
				delay(2000);	
				display_clear(WHITE);
				draw_list(fileSet, 5);
				display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);	
				display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);
			}
			/*uint8_t * my_new_buf = (uint8_t *)malloc(sizeof(uint8_t)*IMG_SIZE);
			//num_pics = select_iterator;
			if(my_new_buf==NULL){
				printf("ERROR: Memory Not Allocated !!");
				exit(0);
			}

			printf("Successfully: Memory has been Allocated\n");

			camera_capture_data(my_new_buf,IMG_SIZE);
			char file_name[40];
			printf("%d\n", num_pics);
		//num_pics++;
			printf("%d\n", num_pics);
			num_pics = (num_pics % 5)+1;

			printf("%d\n", num_pics);
			//num_pics++;
			sprintf(file_name,"doorbell-%d.bmp\0",num_pics);
			//num_pics++;
		//	sprintf(file_name,"doorbell-%d.bmp\0",num_pics);
			printf("%s\n",file_name);
			camera_save_to_file(my_new_buf,IMG_SIZE,file_name);
				
			free(my_new_buf);
			sprintf(fileSet[num_pics],"%s\0",file_name);
			char file_location[40];
			sprintf(file_location,FILE_PIC_PATH,file_name);
			display_draw_image(file_location);
			delay(1000);
			display_clear(WHITE);
		//	readDirectory();
			draw_list(fileSet, 5);
			display_draw_rectangle(0,2+(8*select_iterator),127,((select_iterator*8)+8+2),BLUE,true,1);
			display_draw_string(10,2+(8*select_iterator),fileSet[select_iterator], &Font8, BLUE, RED);
			//num_pics++;
*/
		}
		else if(button_up() == 0){
			printf("upButton_Pushed\n");
			delay(200);
			if(button_up()==1){
				printf("upButtonLeft\n");
				delay(200);
				if(button_down() ==0){
					printf("upButton_Pushed\n");
					delay(200);
					if(button_down()==1){
						printf("upButtonLeft\n");
						delay(200);
						if(button_up()==0){
							delay(200);
							printf("downButton_Pushed\n");
							if(button_up()==1){
								delay(200);
								if(button_down()==0){
									break;
								}
							}
							//developersView();
							//drawn = false;
							
						}
					}
				}
			}
		
			//int * i = (int *) malloc(sizeof(int));
			//*i = select_iterator;
			//send_image((void *)i);

			/*
			Config configuration;
			configuration.port = "2225";
			configuration.host = "netlab.et.byu.edu";
			configuration.hw_id = "BF8BDEAFC";
			
			uint8_t * client_buf = (uint8_t *)malloc(sizeof(uint8_t)*IMG_SIZE);
			FILE *fp;
			char pic_location[40];
			sprintf(pic_location,"../viewer/%s\0",fileSet[select_iterator]);
			printf("filename: %s\n",pic_location);
			fp = fopen(pic_location,"r");
			if(fp == NULL){
				printf("error! location not found!!");
				exit(0);
			}
			fread(client_buf, sizeof(uint8_t),IMG_SIZE,fp);
			printf("image Data in main: \n%s\n",client_buf);
			configuration.payload = client_buf;
			//for(int i = 0; i < IMG_SIZE; i++)
			//	printf("%x", configuration.payload[i]);
			int socket_File_Discriptor_number = client_connect(configuration);
			printf("Hello there made a connection, now sending");
			printf("%d\n",sizeof(client_buf));
			client_send_image(socket_File_Discriptor_number,client_buf,configuration.hw_id,IMG_SIZE);
			client_close(socket_File_Discriptor_number);
			free(client_buf);
			*/
	}
		/*else if(button_down() ==0){
			
			int * i = (int *) malloc(sizeof(int));
			*i = select_iterator;
			//send_image((void *)i);
			pthread_t send_image_thread;
			pthread_create(&send_image_thread,NULL,send_image,(void*)i);
		}*/

		/*if(isSending && !duplicate_sending){
			duplicate_sending = TRUE;
			duplicate_sent = FALSE;
			display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_WIDTH,BLUE,true,1);
			display_draw_string(0,120,"Sending...", &Font8, BLUE, WHITE);
		}
		else if(isSent && !duplicate_sent){
			duplicate_sent = TRUE;
			duplicate_sending = FALSE;
			display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_HEIGHT,GREEN,true,1);
			display_draw_string(0,120,"Sent", &Font8, GREEN, WHITE);
			int *sleep_time = (int *)malloc(sizeof(int));
			*sleep_time = 5;		
			pthread_t sleep_thread;
			pthread_create(&sleep_thread,NULL,sent_timeout,(void *)sleep_time);
		}
		else if(!isSent && !isSending){
			duplicate_sent = FALSE;
			duplicate_sending = FALSE;
			display_draw_rectangle(0,118,DISPLAY_WIDTH,DISPLAY_HEIGHT,BLUE,true,1);
		}*/
	//printf("In the forever While Loop\n");
	// // Example of how to interact with a button
        // if (button_up() == 0) {
        // // Do something upon detecting button press

        //     while (button_up() == 0) {
        //         // Do something while the button is pressed
        //         delay_ms(1);
        //     }
        // }

        /* Write code to fufill lab requirements here*/
	}

}