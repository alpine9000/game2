#include "game.h"

//#define DEBUG

#define USE(x) do { x = x; } while(0);
#define SPRITE_MAX_SPRITES 6

typedef enum {
  ALIVE = 1,
  EXPLODING = 2,
  DEAD = 0
} actor_state_t;


typedef enum {
  SCREEN_DEMO,
  SCREEN_START,
  SCREEN_PLAYER_TURN_MESSAGE,
  SCREEN_GAME,
  SCREEN_GAMEOVER
} screen_t;

typedef struct {
  int x;
  int width;
  int height;
  int count;
  int y[SPRITE_MAX_SPRITES];
} sprite_t;


typedef struct {
  int x;
  int y;
  int sprite;
  int spriteIndex;
  actor_state_t _state;
  void* data;
  int lastX;
  int lastY;
  actor_state_t lastSpriteIndex;
} actor_t;

typedef struct {
  int row;
  int column;
  int direction;
} invader_data_t;


typedef struct {
  char* text;
  int x;
  int y;
  unsigned color;
} screen_text_t;

typedef struct {
  int statusBar;
  int screenChange;
} dirty_t;

#define INVADER_SCREEN_WIDTH 224
#define INVADER_SCREEN_HEIGHT 256
#define SCALE  1
#define SCALED_W (INVADER_SCREEN_WIDTH*SCALE)
#define SCALED_H (INVADER_SCREEN_HEIGHT*SCALE)

#define MISSILE_SPEED 4
#define BOMB_SPEED 1
#define BOMB_DROP_FRAMES 1
#define DEFENDER_EXPLOSION_FRAMES 50
#define DEFENDER_EXPLOSION_FRAMERATE 10
#define DEMO_FRAME_TIME 1
#define PLAYER_TURN_MESSAGE_TIME 1
#define SCORE_FLICKER_TIME 1


#define SCOREBOARD_HEIGHT 32
#define MISSILE_WIDTH 1
#define MISSILE_HEIGHT 4
#define INVADER_WIDTH 12
#define INVADER_HEIGHT 8
#define BOMB_WIDTH 3
#define BOMB_HEIGHT 7
#define DEFENDER_WIDTH 15
#define DEFENDER_HEIGHT 8
#define BASE_WIDTH 22
#define BASE_HEIGHT 16
#define BASE_TOP 192
#define MYSTERY_INVADER_WIDTH 16
#define MYSTERY_INVADER_HEIGHT 8
#define SPRITEMAP_WIDTH 128
#define SPRITEMAP_HEIGHT 64
#define GREEN_TOP (BASE_TOP-INVADER_HEIGHT)

#define SCORE_X   25
#define SCORE_Y   25
#define HISCORE_X 89
#define STATUS_LINE_Y 239
#define GAMEOVER_X 70
#define GAMEOVER_Y 52
#define INVADER_TOP 70
#define INVADER_LEFT 20
#define INVADER_RIGHT_MARGIN 20
#define INVADER_SPACING 16
#define DEFENDER_Y 216
#define CREDIT_LABEL_X 137

#define NUM_DEMO_INVADERS 4
#define NUM_DEFENDERS_X 7
#define NUM_DEFENDERS_Y 241
#define NUM_BASES 4
#define NUM_INVADER_ROWS 5
#define NUM_INVADER_COLUMNS 11
#define NUM_INVADERS (NUM_INVADER_ROWS*NUM_INVADER_COLUMNS)
#define MAX_BOMBS 4
#define NUM_BOMB_RANDOMS 10
#define NUM_BOMB_RANDOM_COLUMNS 11
#define NUM_DEFENDER_EXPLOSION_SPRITES 3

#define COLOR_GREEN 0xFF00FF00
#define RGBA_COLOR_BACKGROUND 0x000000FF


static dirty_t dirty = {
  1, 0
};

static sprite_t spriteConfig[] = { 
  {0, INVADER_WIDTH, INVADER_HEIGHT, 5},
  {16, INVADER_WIDTH, INVADER_HEIGHT, 5},
  {32, INVADER_WIDTH, INVADER_HEIGHT, 5},
  {48, BOMB_WIDTH, BOMB_HEIGHT, 3},
  {64, DEFENDER_WIDTH,DEFENDER_HEIGHT, 4},
  {80, MISSILE_WIDTH, MISSILE_HEIGHT, 2},
  {96, MYSTERY_INVADER_WIDTH, MYSTERY_INVADER_HEIGHT, 1},
  {112, BASE_WIDTH, BASE_HEIGHT, 4},
  {0, 0, 0}
};

typedef enum {
  SPRITE_INVADER1 = 0,
  SPRITE_INVADER2 = 1,
  SPRITE_INVADER3 = 2,
  SPRITE_MISSILE = 3,
  SPRITE_DEFENDER = 4,
  SPRITE_DEFENDER_MISSILE = 5,
  SPRITE_MYSTERY_INVADER = 6,
  SPRITE_BASE = 7,
} sprite_index_t;


static actor_t invaders[NUM_INVADER_COLUMNS*NUM_INVADER_ROWS];
static invader_data_t invaderData[NUM_INVADER_COLUMNS*NUM_INVADER_ROWS];

static actor_t bombs[MAX_BOMBS] = {0};

static actor_t missile = {0, 0, SPRITE_DEFENDER_MISSILE, 0, DEAD, 0, -1, -1, 0};

static actor_t defender = {0, DEFENDER_Y, SPRITE_DEFENDER, 0, ALIVE, 0, -1, -1, 0};

static actor_t demoInvaders[NUM_DEMO_INVADERS] = {
  {64, 137, SPRITE_MYSTERY_INVADER, 0, ALIVE, 0, -1, -1, 0},
  {66, 152, SPRITE_INVADER3, 1, ALIVE, 0, -1, -1, 0},
  {66, 168, SPRITE_INVADER1, 0, ALIVE, 0, -1, -1, 0},
  {66, 184, SPRITE_INVADER2, 1, ALIVE, 0, -1, -1, 0}
};

static actor_t spareDefenders[] = {
  {26, 240, SPRITE_DEFENDER, 0, ALIVE, 0, -1, -1, 0},
  {42, 240, SPRITE_DEFENDER, 0, ALIVE, 0, -1, -1, 0}
};

static actor_t bases[NUM_BASES] = {
  {30, BASE_TOP, SPRITE_BASE, 0, ALIVE, 0, -1, -1, 0},
  {75, BASE_TOP, SPRITE_BASE, 1, ALIVE, 0, -1, -1, 0},
  {120, BASE_TOP, SPRITE_BASE, 2, ALIVE, 0, -1, -1, 0},
  {165, BASE_TOP, SPRITE_BASE, 3, ALIVE, 0, -1, -1, 0}
};

typedef struct {
  int x;
  int y;
} coord_t;

static coord_t missileBaseExplosion[] = 
  {
    {-2, -3}, {0, -3}, {2, -3}, 
    {-1, -2}, {-3, -2}, {0, -2}, {1, -2}, {3, -2},
    {-1, -1}, {-2, -1}, {0, -1}, {1, -1}, {2, -1},
    {-1, 0}, {-3, 0}, {1, 0}, {3, 0}, {0, 0}
  };

static coord_t bombBaseExplosion[] = 
  {
    {-2, 3}, {0, 3}, {2, 3}, 
    {-1, 2}, {-3, 2}, {0, 2}, {1, 2}, {3, 2},
    {-1, 1}, {-2, 1}, {0, 1}, {1, 1}, {2, 1},
    {-1, 0}, {-3, 0}, {1, 0}, {3, 0}, {0, 0}
  };

static coord_t* baseExplosion[] = {
  missileBaseExplosion,
  bombBaseExplosion
};


static int bombRandomMap[NUM_BOMB_RANDOMS] = { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0};
static int bombRandomColumns[NUM_BOMB_RANDOM_COLUMNS] = { 0, 2, 6, 1, 9, 3, 5, 7, 8, 4, 10};
static int killScores[NUM_INVADER_ROWS] = { 30, 20, 20, 10, 10};
static int invaderIndex = 0;
static int invaderSpeed = 5;
static int score = 0;
static int credits = 0;
static int creditsMode = 1;
static int hiscore = 0;
static unsigned frame = 0;
static int numDefenders = 0;
static volatile uint8 *frameBuffer, *spriteFrameBuffer;
static int renderTime = 0;
static int lastTime = 0;
static int demoIndex = 0;
static int playerTurnMessageTime = 0;
static screen_t currentScreen = SCREEN_DEMO;
static unsigned time = 0;


static char *
scoretoa(int i, int bufsize, char* buf)
{
  buf[bufsize-1] = 0;
  char *p = buf + bufsize-1;
  do {
    *--p = '0' + (i % 10);
    i /= 10;
  } while (i != 0);
  do {
    *--p = '0';
  } while (p != buf);

  return buf;
}


static void 
setCurrentScreen(int screen)
{
  currentScreen = screen;
  dirty.screenChange = 1;
}


static void 
initInvader(int x, int y, int row, int column, unsigned sprite)
{
  actor_t *i = &  invaders[invaderIndex];
  i->x = x;
  i->y = y;
  i->lastX = -1;
  i->lastY = -1;
  i->sprite = sprite;
  i->spriteIndex = 0;
  i->lastSpriteIndex = 0;
  i->_state = ALIVE;
  invader_data_t* data = &invaderData[invaderIndex];
  data->row = row;
  data->column = column;
  data->direction = 1;
  i->data = data;
  invaderIndex++;
}


static void 
initInvaders()
{
  unsigned char sprites[] = {2, 0, 0, 1, 1};

  for (int y = NUM_INVADER_ROWS-1; y >= 0; y--) {
    for (int x = 0; x < NUM_INVADER_COLUMNS; x++) {
      initInvader(INVADER_LEFT+(x * INVADER_SPACING), INVADER_TOP+(y * INVADER_SPACING), y, x, sprites[y]);
    } 
  }
}


static void 
dropBomb(int index, int x, int y)
{
  actor_t *i = &bombs[index];
  i->x = x;
  i->y = y;
  i->sprite = SPRITE_MISSILE;
  i->spriteIndex = 0;
  i->_state = ALIVE;
}


static void 
drawSpriteRGBA(uint16 x, uint16 y, unsigned *ptr)
{
  unsigned data = (unsigned)(*ptr);
  unsigned char alpha = (data & 0xFF);
  data = (data >> 8) | alpha << 24;
  gfx_drawPixel(spriteFrameBuffer, x, y, data);
}


static void
putSpritePixelRGBA(int sprite, int index, int x, int y, unsigned pixel)
{
#if 0
  unsigned *ptr = (unsigned*) &sprite_rgba;
  sprite_t* sp = &spriteConfig[sprite];
  int sy = sp->height*index;  
  int sx = 0;
  
  if (x < 0 || y < 0) {
    return;
  }
    
  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  unsigned* dp = ptr + ((sy+y)*SPRITEMAP_WIDTH) + (sx+x);
  *dp = pixel;
#endif
}


static unsigned
getSpritePixelRGBA(int sprite, int index, int x, int y)
{
#if 0
  sprite_t* sp = &spriteConfig[sprite];

  if (!(x >= 0 && x < sp->width && y >= 0 && y < sp->height)) {
    return 0;
  }

  unsigned *ptr = (unsigned*) &sprite_rgba;
  int sy = sp->height*index;  
  int sx = 0;

  for (int i = 0; i < sprite; i++) {
    sx += spriteConfig[i].width;
  }
  
  unsigned* dp = ptr + ((sy+y)*SPRITEMAP_WIDTH) + (sx+x);
  return *dp;
#endif
  return 0;
}


static void
forceRenderActor(actor_t* actor)
{
  sprite_t *s = &spriteConfig[actor->sprite];  
  gfx_fillRect(frameBuffer, actor->x, actor->y, s->width, s->height, 0);
  gfx_bitBlt(frameBuffer, s->x, s->y[actor->spriteIndex], actor->x, actor->y, s->width, s->height, spriteFrameBuffer);
}


static void
clearActor(actor_t* actor)
{
  sprite_t *s = &spriteConfig[actor->sprite];  
  gfx_fillRect(frameBuffer, actor->x, actor->y, s->width, s->height, 0);
}


static void
renderActor(actor_t* actor)
{
  sprite_t *s = &spriteConfig[actor->sprite];  

  if (actor->_state != DEAD) {
    if (actor->x != actor->lastX || actor->y != actor->lastY || actor->lastSpriteIndex != actor->spriteIndex) {
      if (actor->lastX != -1) {
	gfx_fillRect(frameBuffer, actor->lastX, actor->lastY, s->width, s->height, 0);
      }
      gfx_bitBlt(frameBuffer, s->x, s->y[actor->spriteIndex], actor->x, actor->y, s->width, s->height, spriteFrameBuffer);
      actor->lastX = actor->x;
      actor->lastY = actor->y;
      actor->lastSpriteIndex = actor->spriteIndex;
    } 
  }
}


static void
killActor(actor_t* actor)
{
  sprite_t *s = &spriteConfig[actor->sprite];  
  actor->_state = DEAD;
  if (actor->lastX != -1) {
    gfx_fillRect(frameBuffer, actor->lastX, actor->lastY, s->width, s->height, 0);
  }
}


static void 
initRender()
{
  for (int sprite = 0; spriteConfig[sprite].count != 0; sprite++) {
    for (int index = 0; index < spriteConfig[sprite].count; index++) {
      spriteConfig[sprite].y[index] = spriteConfig[sprite].height * index;
    }
  }

  gfx_fillRect(frameBuffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  gfx_fillRect(frameBuffer, 0, 0, INVADER_SCREEN_WIDTH, INVADER_SCREEN_HEIGHT, 0);
  gfx_drawStringRetro(frameBuffer, 9, 9, "SCORE<1> HI-SCORE SCORE<2>", 1, 3);  
}



static void 
downShiftInvaders(int row)
{
  //invaderSpeed = invaderSpeed - 50;

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    if (((invader_data_t*)inv->data)->row == row) {
      inv->y += (INVADER_HEIGHT);
      ((invader_data_t*)inv->data)->direction = -((invader_data_t*)inv->data)->direction;
      inv->x += ((invader_data_t*)inv->data)->direction;
    }
  }
}


static void
moveMissile()
{
  if (missile._state == ALIVE) {
    missile.y-=MISSILE_SPEED;
    if ((missile.y+MISSILE_HEIGHT) < BASE_TOP) {
      missile.spriteIndex = 1;
    }
  }

  if (missile.y <= (SCOREBOARD_HEIGHT)) {
    killActor(&missile);
  }
}


static actor_t* 
findBottomInvaderForColumn(int column)
{
  for (int i = 0; i < NUM_INVADERS; i++) {
    if (invaders[i]._state == ALIVE  && ((invader_data_t*)invaders[i].data)->column  == column) {
      return &invaders[i];
      break;
    }
  }
  
  return 0;
}


static actor_t* 
findBottomInvader()
{
  return findBottomInvaderForColumn(bombRandomColumns[frame %  NUM_BOMB_RANDOM_COLUMNS]);
}


static void
moveBombs()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    actor_t* b = &bombs[i];
    if (b->_state == ALIVE) {
      b->y+=BOMB_SPEED;
      if (b->y >= (STATUS_LINE_Y-BOMB_HEIGHT+2))
	killActor(b);
    }    
  }
}

static void
dropBombs()
{
  if (frame % BOMB_DROP_FRAMES == 0) {
    if (bombRandomMap[frame % NUM_BOMB_RANDOMS]) {
      for (int i = 0; i < MAX_BOMBS; i++) {
	if (bombs[i]._state == DEAD) {
	  actor_t *invader;
	  if ((invader = findBottomInvader()) != 0) {
	    dropBomb(i, invader->x + INVADER_WIDTH/2, invader->y + INVADER_HEIGHT);
	    break;
	  }
	}
      }
    }
  }
}

static void
playBeat(int time)
{
  static int beat = 0;
  static int last = 0;
  static int beatSpeed = 50;

  if (time-last > beatSpeed) {
    audio_playBeat(beat++);
    if (beat == 4) {
      beat = 0;
    }
    last = time;
  }
}

static void
moveInvaders(int time) 
{
  static int last = 0;
  static int index = 0;

  if (time-last > invaderSpeed) {
    last = time;

    for (int i = 0; i < 4; i++) {
      actor_t *inv = &invaders[index];
      if (inv->_state == ALIVE) {
	inv->x += ((invader_data_t*)inv->data)->direction;
	if (inv->x > INVADER_SCREEN_WIDTH-INVADER_RIGHT_MARGIN) {
	  downShiftInvaders(((invader_data_t*)inv->data)->row);
	}
	if (inv->x < 10) {
	  downShiftInvaders(((invader_data_t*)inv->data)->row);
	}
	switch (inv->spriteIndex) {
	case 0:
	  inv->spriteIndex = 1;
	  break;
	case 1:
	  inv->spriteIndex = 0;
	  break;
	case 3:
	  inv->spriteIndex = 4;
	  break;
	case 4:
	  inv->spriteIndex = 3;
	  break;
	}
      }
      if (++index == invaderIndex) {
	index = 0;
      }
    }    

    for (int i = 0; i < invaderIndex; i++) {
      actor_t *inv = &invaders[i];
      if (inv->_state == EXPLODING) {
	killActor(inv);
      }
    }

  } else {
    dropBombs();
  }


}

static void
renderGameOver()
{
  if (dirty.screenChange) {
    gfx_fillRect(frameBuffer, GAMEOVER_X, GAMEOVER_Y, 9*(gfx_retroFontWidth+3), gfx_retroFontHeight, 0);
    gfx_drawStringRetro(frameBuffer, GAMEOVER_X, GAMEOVER_Y, "GAME OVER", 1, 3);  
    dirty.screenChange = 0;
  }

  if (hw_joystickButton) {
    demoIndex = 0;
    setCurrentScreen(SCREEN_DEMO);
  }
}

static void
renderBases()
{
  for (unsigned i = 0; i < NUM_BASES; i++) {
    forceRenderActor(&bases[i]);
  }
}


static void
renderStatusBar(int drawLine, int drawLives)
{
  if (drawLine) {
    gfx_drawLine(frameBuffer, 0, STATUS_LINE_Y, INVADER_SCREEN_WIDTH, STATUS_LINE_Y, COLOR_GREEN);
  }
  
  gfx_fillRect(frameBuffer, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, gfx_retroFontWidth, gfx_retroFontHeight, 0);
  if (drawLives) {
    char buffer[2] = {'0' + numDefenders, 0};
    gfx_drawStringRetro(frameBuffer, NUM_DEFENDERS_X, NUM_DEFENDERS_Y, buffer , 1, 0);    
  }
  
  int i;
  for (i = 0; i < numDefenders-1; i++) {
    forceRenderActor(&spareDefenders[i]);
  }

  for (; i < 2; i++) {
    clearActor(&spareDefenders[i]);
  }
  
  gfx_drawStringRetro(frameBuffer, CREDIT_LABEL_X, NUM_DEFENDERS_Y, "CREDIT ", 1, 3);    
  static char creditbuffer[3];
  static int lastCredits = -1;
  int display = creditsMode ? credits : renderTime;
  if (display < 99) {
    if (display != lastCredits) {
      scoretoa(display, 3, creditbuffer);
      lastCredits = display;
    }
    gfx_drawStringRetro(frameBuffer, CREDIT_LABEL_X+((gfx_retroFontWidth+3)*7), NUM_DEFENDERS_Y, creditbuffer, 1, 3);    
  } else {
    gfx_drawStringRetro(frameBuffer, CREDIT_LABEL_X+((gfx_retroFontWidth+3)*7), NUM_DEFENDERS_Y, "XX", 1, 3);    
  }
}


static void 
renderMissile()
{
  renderActor(&missile);
}


static void 
renderScores(int hideScore)
{
  static char scoreBuffer[6];
  static char hiScoreBuffer[6];
  static int lastScore = -1;
  static int lastHiScore = -1;
  static int lastHide = -1;
  int dirtyScore = 0;
  int dirtyHi = 0;

  if (lastHide != hideScore) {
    dirtyScore = 1;
    lastHide = hideScore;
  }

  if (lastScore != score) {
    scoretoa(score, 6, scoreBuffer);
    lastScore = score;
    dirtyScore = 1;
  }

  if (lastHiScore != hiscore) {
     scoretoa(hiscore, 6, hiScoreBuffer);
    lastHiScore = hiscore;
    dirtyHi = 1;
  }
  
  if (dirtyScore) {
    gfx_fillRect(frameBuffer, SCORE_X, SCORE_Y, (gfx_retroFontWidth+3)*4, gfx_retroFontHeight+2, 0);
    if (hideScore != 1) {
      gfx_drawStringRetro(frameBuffer, SCORE_X, SCORE_Y, scoreBuffer+1, 1, 3);    
    }
  }

  if (dirtyHi) {
    gfx_fillRect(frameBuffer, HISCORE_X, SCORE_Y, (gfx_retroFontWidth+3)*4, gfx_retroFontHeight+2, 0);
    gfx_drawStringRetro(frameBuffer, HISCORE_X, SCORE_Y, hiScoreBuffer+1, 1, 3);
  }
}


static void
renderDefender()
{
  renderActor(&defender);
}


static void
renderInvaders()
{
  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = & invaders[i];    
    renderActor(inv);
  }
}


static void
renderBombs()
{
 for (int i = 0; i < MAX_BOMBS; i++) {
    actor_t *bomb = & bombs[i];    
    renderActor(bomb);
  }
}


static void 
renderGameScreen()
{
  //custom->color[0] = 0xf00;

  renderScores(0);

  renderDefender();
  
  renderInvaders();
  
  renderBombs();
  
  renderMissile();

  //custom->color[0] = 0x000;
}


static void
renderStartScreen()
{
  if (dirty.screenChange) {
    gfx_fillRect(frameBuffer, 0, SCOREBOARD_HEIGHT, INVADER_SCREEN_WIDTH, INVADER_SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0);

    hw_waitBlitter();    

    renderScores(0);    
    renderStatusBar(0, 0);
    
    screen_text_t text[] = {
      {"PUSH", (INVADER_SCREEN_WIDTH/2) - (2*(gfx_retroFontWidth+3)), 120-(3*gfx_retroFontHeight), 1},
      {"ONLY 1PLAYER  BUTTON", (INVADER_SCREEN_WIDTH/2) - (10*(gfx_retroFontWidth+3)), 120,  1}
    };

    for (unsigned i = 0; i < sizeof(text)/sizeof(screen_text_t); i++) {
      gfx_drawStringRetro(frameBuffer, text[i].x, text[i].y, text[i].text, 1, 3);  
    }

    dirty.screenChange = 0;
  }

}

static void
renderPlayer1TurnMessageScreen(int time)
{
  static int flickerTime = 0;
  static int showScore = 0;


  if (dirty.screenChange) {
    gfx_fillRect(frameBuffer, 0, SCOREBOARD_HEIGHT, INVADER_SCREEN_WIDTH, INVADER_SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0);

    hw_waitBlitter();

    renderStatusBar(0, 0);
    screen_text_t text[] = {
      {"PLAY PLAYER<1>", (INVADER_SCREEN_WIDTH/2) - (7*(gfx_retroFontWidth+3)), 120-(2*gfx_retroFontHeight),  0xFFFFFFFF}
    };
    
    for (unsigned i = 0; i < sizeof(text)/sizeof(screen_text_t); i++) {
      gfx_drawStringRetro(frameBuffer, text[i].x, text[i].y, text[i].text, 1, 3);  
    } 

    dirty.screenChange = 0;
  }

  if (time - flickerTime > SCORE_FLICKER_TIME) {
    showScore = !showScore;
    flickerTime = time;
  }

  renderScores(showScore);
}


static void
renderDemoScreen(int time)
{
  static int lastDemoTime = 0;
  static int lastDemoIndex = 0;

  if (dirty.screenChange) {
    lastDemoIndex = 0;
    gfx_fillRect(frameBuffer, 0, SCOREBOARD_HEIGHT, INVADER_SCREEN_WIDTH, INVADER_SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0);
    dirty.screenChange = 0;
  }

  renderScores(0);

  renderStatusBar(0, 0);

  static screen_text_t text[] = {
    {"P", 97, 65, 1},
    {"L", 97+(1*(gfx_retroFontWidth+3)), 65, 1},
    {"A", 97+(2*(gfx_retroFontWidth+3)), 65, 1},
    {"Y", 97+(3*(gfx_retroFontWidth+3)), 65, 1},

    {"S", 57+(0*(gfx_retroFontWidth+3)), 89, 1},
    {"P", 57+(1*(gfx_retroFontWidth+3)), 89, 1},
    {"A", 57+(2*(gfx_retroFontWidth+3)), 89, 1},
    {"C", 57+(3*(gfx_retroFontWidth+3)), 89, 1},
    {"E", 57+(4*(gfx_retroFontWidth+3)), 89, 1},
    {" ", 57+(5*(gfx_retroFontWidth+3)), 89, 1},
    {" ", 57+(6*(gfx_retroFontWidth+3)), 89, 1},

    {"I", 114+(0*(gfx_retroFontWidth+3)), 89, 1},
    {"N", 114+(1*(gfx_retroFontWidth+3)), 89, 1},
    {"V", 114+(2*(gfx_retroFontWidth+3)), 89, 1},
    {"A", 114+(3*(gfx_retroFontWidth+3)), 89, 1},
    {"D", 114+(4*(gfx_retroFontWidth+3)), 89, 1},
    {"E", 114+(5*(gfx_retroFontWidth+3)), 89, 1},
    {"R", 114+(6*(gfx_retroFontWidth+3)), 89, 1},
    {"S", 114+(7*(gfx_retroFontWidth+3)), 89, 1},

    {"*SCORE ADVANCE TABLE*", 33, 121, 1},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 137, 1},
    {"?", 81+(1*(gfx_retroFontWidth+3)), 137, 1},
    {" ", 81+(2*(gfx_retroFontWidth+3)), 137, 1},
    {"M", 81+(3*(gfx_retroFontWidth+3)), 137, 1},
    {"Y", 81+(4*(gfx_retroFontWidth+3)), 137, 1},
    {"S", 81+(5*(gfx_retroFontWidth+3)), 137, 1},
    {"T", 81+(6*(gfx_retroFontWidth+3)), 137, 1},
    {"E", 81+(7*(gfx_retroFontWidth+3)), 137, 1},
    {"R", 81+(8*(gfx_retroFontWidth+3)), 137, 1},
    {"Y", 81+(9*(gfx_retroFontWidth+3)), 137, 1},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 153, 1},
    {"3", 81+(1*(gfx_retroFontWidth+3)), 153, 1},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 153, 1},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 153, 1},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 153, 1},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 153, 1},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 153, 1},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 153, 1},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 153, 1},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 153, 1},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 169, 1},
    {"2", 81+(1*(gfx_retroFontWidth+3)), 169, 1},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 169, 1},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 169, 1},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 169, 1},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 169, 1},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 169, 1},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 169, 1},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 169, 1},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 169, 1},

    {"=", 81+(0*(gfx_retroFontWidth+3)), 185, 1},
    {"1", 81+(1*(gfx_retroFontWidth+3)), 185, 1},
    {"0", 81+(2*(gfx_retroFontWidth+3)), 185, 1},
    {" ", 81+(3*(gfx_retroFontWidth+3)), 185, 1},
    {"P", 81+(4*(gfx_retroFontWidth+3)), 185, 1},
    {"O", 81+(5*(gfx_retroFontWidth+3)), 185, 1},
    {"I", 81+(6*(gfx_retroFontWidth+3)), 185, 1},
    {"N", 81+(7*(gfx_retroFontWidth+3)), 185, 1},
    {"T", 81+(8*(gfx_retroFontWidth+3)), 185, 1},
    {"S", 81+(9*(gfx_retroFontWidth+3)), 185, 1},

  };

  for (int i = lastDemoIndex; i < demoIndex; i++) {
    gfx_drawStringRetro(frameBuffer, text[i].x, text[i].y, text[i].text, 1, 3);  
    if (text[i].text[0] == '*') {
      for (int a = 0; a < NUM_DEMO_INVADERS; a++) {
	forceRenderActor(&demoInvaders[a]);
      }
    }
  }

  if (lastDemoTime == 0) {
    lastDemoTime = time + (5*DEMO_FRAME_TIME);
  }

  if (time - lastDemoTime >  DEMO_FRAME_TIME && demoIndex < (int)(sizeof(text)/sizeof(screen_text_t))) {
    lastDemoIndex = demoIndex;
    demoIndex++;
    lastDemoTime = time;
  }
}



static void
explodeBase(int baseIndex, int explosionIndex, int x, int y)
{
  int baseX = bases[baseIndex].x;
  int baseY = bases[baseIndex].y;
  int i;

  for (i = 0; baseExplosion[explosionIndex][i].x != 0 || baseExplosion[explosionIndex][i].y != 0; i++) {
    putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), RGBA_COLOR_BACKGROUND);
  }
  putSpritePixelRGBA(SPRITE_BASE, baseIndex, (x+baseExplosion[explosionIndex][i].x-baseX), (y+baseExplosion[explosionIndex][i].y-baseY), RGBA_COLOR_BACKGROUND);
}


static inline int
actorCollision(actor_t* a, int aw, int ah, actor_t* b, int bw, int bh)
{
  return (a->x < b->x + bw &&
	  a->x + aw > b->x &&
	  a->y < b->y + bh &&
	  ah + a->y > b->y);
}

static void
invaderBaseCollision()
{
  int dirtyBases[NUM_BASES] = {0};
  for (int c = 0; c < NUM_INVADER_COLUMNS; c++) {
    actor_t *inv = findBottomInvaderForColumn(c);
    for (int b = 0; b < NUM_BASES; b++) {      
      if (inv && inv->_state == ALIVE && actorCollision(inv, INVADER_WIDTH, INVADER_HEIGHT, &bases[b], BASE_WIDTH, BASE_HEIGHT)) {
	for (unsigned x = inv->x; x < (unsigned)inv->x+INVADER_WIDTH; x++) {      
	  for (unsigned y = inv->y-1; y <= (unsigned)inv->y+INVADER_HEIGHT; y++) {
	    int _x = x-bases[b].x;
	    int _y = y-bases[b].y;
	    unsigned pixel = getSpritePixelRGBA(bases[b].sprite, bases[b].spriteIndex, _x, _y);
	    if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
	      putSpritePixelRGBA(bases[b].sprite, bases[b].spriteIndex, _x, _y, RGBA_COLOR_BACKGROUND);
	      dirtyBases[b] = 1;
	    }
	  }
	}
      }
    }
  }

  for (int i = 0; i < NUM_BASES; i++) {
    if (dirtyBases[i]) {
      //     transferSprite(bases[i].sprite, bases[i].spriteIndex);
    }
  }

}

static int
missileBaseCollision()
{
#if 0
  #define numOffsets 3
  int xOffsets[numOffsets] = {0, 1, -1};

  for (int y = MISSILE_SPEED; y >= 0; y--) {
    for (int x = 0; x < numOffsets; x++) {
      for (int i = 0; i < NUM_BASES; i++) {
	unsigned pixel = getSpritePixelRGBA(bases[i].sprite, bases[i].spriteIndex, missile.x-bases[i].x+xOffsets[x], missile.y-bases[i].y+y);
	if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
	  killActor(&missile);
	  explodeBase(i, 0, missile.x+xOffsets[x], missile.y+y);
	  return 1;
	}
      }
    }
  }
#endif
  return 0;
}


static int
bombBaseCollision(int bombIndex)
{
  #define numOffsets 3
  int xOffsets[numOffsets] = {0, 1, -1};
  actor_t* b = &bombs[bombIndex];
  int bombY = b->y + BOMB_HEIGHT;

  for (int y = BOMB_SPEED; y >= 0; y--) {
    for (int x = 0; x < numOffsets; x++) {
      for (int i = 0; i < NUM_BASES; i++) {
	unsigned pixel = getSpritePixelRGBA(bases[i].sprite, bases[i].spriteIndex, b->x-bases[i].x+xOffsets[x], bombY-bases[i].y+y);
	if (pixel != RGBA_COLOR_BACKGROUND && pixel != 0) {
	  killActor(b);
	  explodeBase(i, 1, b->x+xOffsets[x], bombY+y-1);
	  return 1;
	}
      }
    }
  }

  return 0;
}


static void
bombBasesCollision()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    if (bombs[i]._state == ALIVE) {
      bombBaseCollision(i);
    }
  }
}


static void
bombCollision()
{
  for (int i = 0; i < MAX_BOMBS; i++) {
    if (bombs[i]._state == ALIVE) {
      if (defender.x < bombs[i].x + BOMB_WIDTH &&
	  defender.x + DEFENDER_WIDTH > bombs[i].x &&
	  defender.y < bombs[i].y + BOMB_HEIGHT &&
	  DEFENDER_HEIGHT + defender.y > bombs[i].y) {
	defender._state = EXPLODING;
	defender.data = 0;
	defender.spriteIndex = 1;
	killActor(&bombs[i]);
	audio_playExplosion();
	break;
      }
    }
  }
}


static void 
missileCollision()
{
  if (missile._state == DEAD) {
    return;
  }

  if (missileBaseCollision()) {
    return;
  }

  for (int i = 0; i < invaderIndex; i++) {
    actor_t *inv = &invaders[i];
    if (inv->_state == ALIVE) {
      if (inv->x < missile.x + MISSILE_WIDTH &&
	  inv->x + INVADER_WIDTH > missile.x &&
	  inv->y < missile.y + MISSILE_HEIGHT &&
	  INVADER_HEIGHT + inv->y > missile.y) {
	audio_playInvaderKilled();
	inv->_state = EXPLODING;
	inv->spriteIndex = 2;
	forceRenderActor(inv);
	killActor(&missile);
	score = score + killScores[((invader_data_t*)inv->data)->row];
	if (score > hiscore) {
	  hiscore = score;
	}
	break;
      }
    } 
  }
}


static void
moveDefender()
{
  if (defender._state == ALIVE) {
    if (hw_joystickPos == 0x7) {
      defender.x-=2;
      if (defender.x < 0) {
	defender.x = 0;
      }
    }
    
    if (hw_joystickPos == 0x3) {
      defender.x+=2;
      if (defender.x >= INVADER_SCREEN_WIDTH-INVADER_WIDTH-3) {
	defender.x = INVADER_SCREEN_WIDTH-INVADER_WIDTH-3;
      }
    }
  } else if (defender._state == EXPLODING) {
    int explosionCount = (int)defender.data;
    if (explosionCount++ > DEFENDER_EXPLOSION_FRAMES) {
      defender.data = 0;
      defender.spriteIndex = 0;
      if (--numDefenders > 0) {
	renderStatusBar(0, 1);
	defender._state = ALIVE;
      } else {
	killActor(&defender);
      }
    } else {
      defender.data = (void*)explosionCount;
      if (explosionCount % DEFENDER_EXPLOSION_FRAMERATE == 0) {
	defender.spriteIndex++;
      }
      if (defender.spriteIndex == NUM_DEFENDER_EXPLOSION_SPRITES) {
	defender.spriteIndex = 1;
      }
    }
  }
}


static void
shootMissile()
{
  if (missile._state == DEAD) {
    missile.x = defender.x + (INVADER_WIDTH/2);
    missile.y = defender.y-spriteConfig[missile.sprite].height;
    missile.spriteIndex = 0;
    missile._state = ALIVE;
    audio_playShoot();
  }
}


static void
init()
{
  
  frameBuffer = (uint8*)&bitplanes;
  spriteFrameBuffer = (uint8*)&spriteBitplanes;

  initInvaders();
  initRender();
}

static void
gameLoop(unsigned time)
{
  if (numDefenders > 0) {
    if (hw_joystickButton & 0x1) {
      shootMissile();
    }

    moveDefender();
    moveInvaders(time);
    moveMissile();
    moveBombs();


    //    invaderBaseCollision();
    missileCollision();
    bombCollision();
    //bombBasesCollision();     

    renderGameScreen();
    
    playBeat(time);
    
    if (frame % 20 == 0) {
      renderTime = time-lastTime;      
    }
    lastTime = time;
    frame++;

  } else {
    setCurrentScreen(SCREEN_GAMEOVER);
  }
}

static void
startLoop(int time)
{
  renderStartScreen();
}

static void
initNewGame(void)
{
  invaderIndex = 0;
  initInvaders();
  
  score = 0;
  defender._state = ALIVE;
  defender.x = 0;
  defender.lastX = -1;
  missile.lastY = -1;
  numDefenders = 3;
  setCurrentScreen(SCREEN_GAME);
  gfx_fillRect(frameBuffer, 0, SCOREBOARD_HEIGHT, INVADER_SCREEN_WIDTH, INVADER_SCREEN_HEIGHT-SCOREBOARD_HEIGHT, 0);
  dirty.statusBar = 1;

  renderBases();
  renderStatusBar(1, 1);
}

static void
playerTurnMessageLoop(int time) 
{
  if (time - playerTurnMessageTime > PLAYER_TURN_MESSAGE_TIME) {
    initNewGame();
  } else {
    renderPlayer1TurnMessageScreen(time);
  }
}

static void
demoLoop(int time)
{
  renderDemoScreen(time);
}

void
si_init()
{
  init();
}

void
si_loop()
{
  USE(lastTime);

  audio_vbl();

  time++;

  static uint8 lastJoystick = 0;

  if (lastJoystick != hw_joystickButton && hw_joystickButton & 0x1) {
    switch (currentScreen) {
    case SCREEN_START:
      credits--;
      setCurrentScreen(SCREEN_PLAYER_TURN_MESSAGE);
      playerTurnMessageTime = time;      
      break;
    case SCREEN_DEMO:
      credits++;
      setCurrentScreen(SCREEN_START);
      break;
    }
  }

  lastJoystick = hw_joystickButton;

  switch (currentScreen) {
  case SCREEN_DEMO:
    demoLoop(time);
    break;
  case SCREEN_START:
    startLoop(time);
    break;
  case SCREEN_PLAYER_TURN_MESSAGE:
    playerTurnMessageLoop(time);      
    break;
  case SCREEN_GAME:
    gameLoop(time);
    break;
  case SCREEN_GAMEOVER:
    renderGameOver();
    break;
  }
}
