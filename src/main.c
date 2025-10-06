#include <genesis.h>
#include <string.h>
#include <../res/resources.h>

int flashing = FALSE;
int frames = 0;
int ball_color = 0;

bool game_on = FALSE;
char msg_start[22] = "PRESS START TO BEGIN!\0";
char msg_reset[37] = "GAME OVER! PRESS START TO PLAY AGAIN.";

/*Score variables*/
int score = 0;
char label_score[6] = "SCORE\0";
char str_score[4] = "0";

/*The edges of the play field*/
const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

/* Ball variables */
int ball_pos_x = 100;
int ball_pos_y = 100;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

/* Player variables */
int player_pos_x = 144;
const int player_pos_y = 200;
int player_vel_x = 0;
const int player_width = 32;
const int player_height = 8;

Sprite *ball;
Sprite *player;

/*Draws text in the center of the screen*/
void showText(char s[]) { VDP_drawText(s, 20 - strlen(s) / 2, 15); }

int sign(int x) { return (x > 0) - (x < 0); }

void updateScoreDisplay() {
  sprintf(str_score, "%d", score);
  VDP_clearText(1, 2, 3);
  VDP_drawText(str_score, 1, 2);
}

void endGame() {
  showText(msg_reset);
  game_on = FALSE;
}

void startGame() {
  score = 0;
  updateScoreDisplay();

  ball_pos_x = 0;
  ball_pos_y = 0;

  ball_vel_x = 1;
  ball_vel_y = 1;

  /*Clear the text from the screen*/
  VDP_clearTextArea(0, 10, 40, 10);

  game_on = TRUE;
}

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
    endGame();
  }

  /*Check for collisions with the player paddle*/
  if (ball_pos_x < player_pos_x + player_width &&
      ball_pos_x + ball_width > player_pos_x) {
    if (ball_pos_y < player_pos_y + player_height &&
        ball_pos_y + ball_height >= player_pos_y) {
      // On collision, invert the velocity
      ball_pos_y = player_pos_y - ball_height - 1;
      ball_vel_y = -ball_vel_y;

      // Increase the score and update the HUD
      score++;
      updateScoreDisplay();

      flashing = TRUE;

      // Make ball faster on every 10th hit
      if (score % 10 == 0) {
        ball_vel_x += sign(ball_vel_x);
        ball_vel_y += sign(ball_vel_y);
      }
    }
  }

  // Position the ball
  ball_pos_x += ball_vel_x;
  ball_pos_y += ball_vel_y;

  SPR_setPosition(ball, ball_pos_x, ball_pos_y);
}

void myJoyHandler(u16 joy, u16 changed, u16 state) {
  if (joy == JOY_1) {
    /*Set player velocity if left or right are pressed;
     *set velocity to 0 if no direction is pressed */
    if (state & BUTTON_RIGHT) {
      player_vel_x = 3;
    } else if (state & BUTTON_LEFT) {
      player_vel_x = -3;
    } else {
      if ((changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT)) {
        player_vel_x = 0;
      }
    }
    if (state & BUTTON_START) {
      if (!game_on) {
        startGame();
      }
    }
  }
}

void positionPlayer() {
  /*Add the player's velocity to its position*/
  player_pos_x += player_vel_x;

  /*Keep the player within the bounds of the screen*/
  if (player_pos_x < LEFT_EDGE)
    player_pos_x = LEFT_EDGE;
  if (player_pos_x + player_width > RIGHT_EDGE)
    player_pos_x = RIGHT_EDGE - player_width;

  /*Let the Sprite engine position the sprite*/
  SPR_setPosition(player, player_pos_x, player_pos_y);
}

int main() {
  JOY_init();
  JOY_setEventHandler(&myJoyHandler);
  VDP_loadTileSet(bgtile.tileset, 1, DMA);

  /*Draw the texts*/
  VDP_setTextPlane(BG_A);
  VDP_drawText(label_score, 1, 1);
  updateScoreDisplay();

  showText(msg_start);

  PAL_setPalette(PAL1, bgtile.palette->data, FALSE);
  VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 0, 40,
                      30);
  /* VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, 1),
   * 39, 0); */
  SPR_init();
  ball = SPR_addSprite(&imgball, ball_pos_x, ball_pos_y,
                       TILE_ATTR(PAL1, 0, FALSE, FALSE));
  player = SPR_addSprite(&paddle, player_pos_x, player_pos_y,
                         TILE_ATTR(PAL1, 0, FALSE, FALSE));
  ball_color = PAL_getColor(22);
  while (1) {
    if (game_on == TRUE) {
      moveBall();
      positionPlayer();

      // Handle the flashing of the ball
      if (flashing == TRUE) {
        // Cool flashing code goes here!
        frames++;

        if (frames % 4 == 0) {
          PAL_setColor(22, ball_color);
        } else if (frames % 2 == 0) {
          PAL_setColor(22, RGB24_TO_VDPCOLOR(0xffffff));
        }

        // Stop flashing
        if (frames > 30) {
          flashing = FALSE;
          frames = 0;
          PAL_setColor(22, ball_color);
        }
      }
    }

    SPR_update();
    SYS_doVBlankProcess();
  }
  return (0);
}
