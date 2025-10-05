#include <genesis.h>
#include <../res/resources.h>

/*The edges of the play field*/
const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

int ball_pos_x = 100;
int ball_pos_y = 100;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

Sprite *ball;

void moveBall() {
  // Check horizontal bounds
  if (ball_pos_x < LEFT_EDGE) {
    ball_pos_x = LEFT_EDGE;
    ball_vel_x = -ball_vel_x;
  } else if (ball_pos_x + ball_width > RIGHT_EDGE) {
    ball_pos_x = RIGHT_EDGE - ball_width;
    ball_vel_x = -ball_vel_x;
  }

  // Check vertical bounds
  if (ball_pos_y < TOP_EDGE) {
    ball_pos_y = TOP_EDGE;
    ball_vel_y = -ball_vel_y;
  } else if (ball_pos_y + ball_height > BOTTOM_EDGE) {
    ball_pos_y = BOTTOM_EDGE - ball_height;
    ball_vel_y = -ball_vel_y;
  }

  // Position the ball
  ball_pos_x += ball_vel_x;
  ball_pos_y += ball_vel_y;

  SPR_setPosition(ball, ball_pos_x, ball_pos_y);
}

int main() {
  VDP_loadTileSet(bgtile.tileset, 1, DMA);
  PAL_setPalette(PAL1, bgtile.palette->data, FALSE);
  VDP_fillTileMapRect(BG_B,TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 0, 40, 30);
  /* VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, 1), 39,
   * 0); */
  SPR_init();
  ball = SPR_addSprite(&imgball, ball_pos_x, ball_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
  while (1) {
    moveBall();
    SPR_update();
    SYS_doVBlankProcess();
  }
  return(0);  
}  
