#include "fbgraphics.h"
#include <math.h>

struct circle{
	int x;
	int y;
	int radius;
	uint32_t color;
};

void draw_circle(struct circle circ, struct fb_info info){
	for(int i = circ.x-circ.radius; i < circ.x+circ.radius; i++){
		for(int j = circ.y-circ.radius; j < circ.y+circ.radius; j++){
			if(sqrt(pow(i-circ.x, 2) + pow(j-circ.y, 2)) <= circ.radius){
				put_pixel(i, j, info, circ.color);
			}
		}
	}
}

int main(int argc, char **argv){
	struct fb_info info;
	fb_open(0, &info);
	struct circle circ1 = {200, 200, 50, pixel_color(0x00, 0x00, 0xFF, 0x00, info)};
	struct circle circ2 = {100, 500, 50, pixel_color(0xFF, 0x00, 0x00, 0x00, info)};
	int circ_vel1[2] = {1, 1};
	int circ_vel2[2] = {-2, 1};
	while(1){
		black_screen(info);
		if(circ1.x+circ1.radius >= info.var_info.xres || circ1.x-circ1.radius <= 0){
			circ_vel1[0] *= -1;
		}
		if(circ2.x+circ2.radius >= info.var_info.xres || circ2.x-circ2.radius <= 0){
			circ_vel2[0] *= -1;
		}
		if(circ1.y+circ1.radius >= info.var_info.yres || circ1.y-circ1.radius <= 0){
			circ_vel1[1] *= -1;
		}
		if(circ2.y+circ2.radius >= info.var_info.yres || circ2.y-circ2.radius <= 0){
			circ_vel2[1] *= -1;
		}

		circ1.x += circ_vel1[0];
		circ1.y += circ_vel1[1];
		circ2.x += circ_vel2[0];
		circ2.y += circ_vel2[1];

		if(!((circ1.y > info.var_info.yres || circ1.x > info.var_info.xres) || (circ1.y < 0 || circ1.x < 0))){
			draw_circle(circ1, info);
		}
		if(!((circ2.y > info.var_info.yres || circ2.x > info.var_info.xres) || (circ2.y < 0 || circ2.x < 0))){
			draw_circle(circ2, info);
		}

		usleep(100);
		front(info);
		
	}
	cleanup(info);
	return 0;
}
