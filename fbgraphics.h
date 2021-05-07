#ifndef __FBGRAPHICS_H__
#define __FBGRAPHICS_H__

#define BITS_PER_PIXEL 32

#include <stdio.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

struct fb_info{
	int fb_fd;
	uint8_t *frontbuffer;
	uint8_t *backbuffer;
	struct fb_var_screeninfo var_info;
	struct fb_fix_screeninfo fix_info;
};

void fb_open(int fb_num, struct fb_info *info){
	//open framebuffer
	char fb[12];
	int fb_fd;
	snprintf(fb, 9, "/dev/fb%d", fb_num);	
	fb_fd = open(fb, O_RDWR);

	//get and set fb_info
	info->fb_fd = fb_fd;
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &info->var_info);
	ioctl(fb_fd, FBIOGET_FSCREENINFO, &info->fix_info);
	info->var_info.grayscale = 0;
	info->var_info.bits_per_pixel = BITS_PER_PIXEL;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &info->var_info);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &info->var_info);

	//memory map || creation of front buffer and back buffer
	long screensize = info->var_info.yres_virtual * info->fix_info.line_length;
	uint8_t *fbp = (uint8_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
	uint8_t *bbp = (uint8_t*)malloc(screensize);
	info->frontbuffer = fbp;
	info->backbuffer = bbp;
}

uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a, struct fb_info info){
	return (r<<info.var_info.red.offset) | (g<<info.var_info.green.offset) | (b<<info.var_info.blue.offset) | (a<<info.var_info.transp.offset); 
}

void put_pixel(int x, int y, struct fb_info info, uint32_t color){
	long location = (x+info.var_info.xoffset) * (info.var_info.bits_per_pixel/8) + (y+info.var_info.yoffset) * info.fix_info.line_length;
	*((uint32_t*)(info.backbuffer + location)) = color;
}

uint32_t get_pixel(int x, int y, struct fb_info info){
	uint32_t location = (x+info.var_info.xoffset) * (info.var_info.bits_per_pixel/8) + (y+info.var_info.yoffset) * info.fix_info.line_length;
	return *((uint32_t*)(info.frontbuffer + location));
}

void black_screen(struct fb_info info){
	memset(info.backbuffer, 0x00, info.var_info.yres*info.fix_info.line_length);
}

void front(struct fb_info info){
	memcpy(info.frontbuffer, info.backbuffer, info.var_info.yres*info.fix_info.line_length);
}

void cleanup(struct fb_info info){
	close(info.fb_fd);
	munmap(info.frontbuffer, info.var_info.yres_virtual * info.fix_info.line_length);
	free(info.backbuffer);
}
#endif
