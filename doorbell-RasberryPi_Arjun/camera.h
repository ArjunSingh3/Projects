#include <stdint.h>

#define IMG_SIZE 49206

void camera_capture_data(uint8_t * buf, size_t bufsize);
void camera_save_to_file(uint8_t * buf, size_t bufsize, char * filename);