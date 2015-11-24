/*
 * Simple tank game to show off the OLEDShield joystick and piezo functionality
 *
 * Player navigates the green tank with the joystick, shoots by
 * pressing button. Try to shoot the brown tank without crashing into
 * it.
 *
 * If you don't have the OLEDShield with joystick, you'll need analog inputs A2 (X) and A3 (Y),
 * and a digital input 6 for the button (active low) and optional digital output 9 for the piezo.
 * You can swap any of these pins in the sketch, see below.
 *
 * The tank sprites are made
 * from Stephen Challener's "Scifi Creature Tileset", licensed CC-BY
 * http://opengameart.org/content/scifi-creature-tileset-mini-32x32-scifi-creature-icons
 *
 * Thanks Stephen!
 *
 */
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>

#include "frames.h"

/* Pins for the OLED display */
const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

/* Pins for game input & piezo (these are the default OLEDShield pins) */
const byte analog_x = 2;
const byte analog_y = 3;
const byte pin_button = 6;
const byte pin_piezo = 9;

OLED oled(pin_cs, pin_dc, pin_reset);

void setup()
{
  oled.begin();
  randomSeed(analogRead(0));
  
  pinMode(pin_button, INPUT);
  pinMode(pin_piezo, OUTPUT);
  
}

/* We define a class to display and move each tank, either player controlled (green)
   or computer controlled (brown.)
*/
class Tank
{
public:
  Tank(OLED_Colour colour, const uint8_t **left_frames, const uint8_t **right_frames, int x, int y, byte facing);
  void drawTank();
  void moveTankBy(int dx, int dy);
  void fire();
  bool checkCollisions(Tank &otherTank);
  inline bool isDead();

  int x, start_x;
  int y, start_y;
  bool moved;
  byte facing; // 1=right, 0=left
  int bullet_x;
  int bullet_y;
  int bullet_dx ; // if 0, no bullet currently fired
  OLED_Colour colour;
  /* The tank can be in one of these 4 states, each with their own display sprite */
  enum { NORMAL, FIRING1, FIRING2, DEAD, RESPAWNING } state;
  /* Timestamp (in millis() milliseconds) of next automatic state change, for automatic animation or respawn events */
  unsigned long next_transition;
  /* Each tank has 4 frame BMPs for its display, this pointer is to the array of BMPs */
  const uint8_t **left_frames;
  const uint8_t **right_frames;
};

// Tank sprite dimensions
const int TANK_WIDTH=32;
const int TANK_HEIGHT_ALIVE=16;
const int TANK_HEIGHT_DEAD=23;

/* Green tank is the player */

/* The four frame BMPs correspond ot the first four tank states (NORMAL, FIRING1, FIRING2, DEAD) */
const uint8_t *greentank_leftframes[] = {
  greentank_left,
  greentank_firing1_left,
  greentank_firing2_left,
  greentank_dead_left,
};

const uint8_t *greentank_rightframes[] = {
  greentank_right,
  greentank_firing1_right,
  greentank_firing2_right,
  greentank_dead_right,
};

Tank green_tank(GREEN, greentank_leftframes, greentank_rightframes, 0, 32, 1);

/* Brown tank is the computer */

const uint8_t *browntank_leftframes[] = {
  browntank_left,
  browntank_firing1_left,
  browntank_firing2_left,
  browntank_dead_left,
};

const uint8_t *browntank_rightframes[] = {
  browntank_right,
  browntank_firing1_right,
  browntank_firing2_right,
  browntank_dead_right,
};

Tank brown_tank(BROWN, browntank_leftframes, browntank_rightframes, 90, 32, 0);

Tank::Tank(OLED_Colour colour, const uint8_t **left_frames, const uint8_t **right_frames, int x, int y, byte facing)
{
    this->colour = colour;
    this->left_frames = left_frames;
    this->right_frames = right_frames;
    this->x = x;
    this->y = y;
    this->start_x = x;
    this->start_y = y;
    this->facing = facing;
}

void Tank::moveTankBy(int dx, int dy)
{
  int last_x = x, last_y = y;
  x = constrain(x+dx, 0, 127-TANK_WIDTH);
  y = constrain(y+dy, 0, 127-TANK_HEIGHT_ALIVE);
  moved = (last_x != x) || (last_y != y);

  // Update the direction we're facing
  if(dx < 0) {
    facing = 0;
  } else if (dx > 0) {
    facing = 1;
  }
}

void Tank::fire()
{
  // Can only fire a new bullet if no bullet is already being fired
  if(bullet_dx == 0 && state == NORMAL) {
    state = FIRING1;
    noTone(pin_piezo);
    tone(pin_piezo, 900);
    delay(10);
    tone(pin_piezo, 90, 20);
    next_transition = millis() + 150;
  }
}

void Tank::drawTank()
{
  // Check for a state transition if we're displaying an animated sequence or dying/respawning
  if(next_transition && millis() > next_transition) {
    switch(state) {
    case FIRING1:
      state = FIRING2;
      // bullet actually appears in this state
      bullet_x = facing ? x+TANK_WIDTH: x-1;
      bullet_y = y+12;
      bullet_dx = facing ? 3 : -3;
      next_transition = millis() + 150;
      break;
    case FIRING2:
      state = NORMAL;
      next_transition = 0;
      break;
    case DEAD:
      state = RESPAWNING; // vanish from our death position, then we respawn on the start position
      next_transition = millis(); // happens in the very next frame
      break;
    case RESPAWNING:
      x = start_x;
      y = start_y;
      state = NORMAL;
      next_transition = 0;
      break;
    default:
      next_transition = 0;
    }
  }

  if(moved) {
    int height = isDead() ? TANK_HEIGHT_DEAD : TANK_HEIGHT_ALIVE;
    oled.drawBox(max(0,x-5),max(0,y-5),min(127,x+TANK_WIDTH+4),min(127,y+height+4),6,BLACK);
    moved = false;
  }
  if(state != RESPAWNING) {
    const uint8_t **frames = facing ? right_frames : left_frames;
    oled.displayBMP(frames[state], x, y);
  } else {
    oled.drawFilledBox(x,y,x+TANK_WIDTH,y+TANK_HEIGHT_DEAD,BLACK);
  }

  // Update and draw bullet if there is one
  if(bullet_dx != 0) {
    oled.drawBox(bullet_x, bullet_y, bullet_x+1, bullet_y+1, 1, BLACK);
    bullet_x += bullet_dx;
    if(bullet_x < 0 || bullet_x > 127) { // Bullet off screen
      bullet_dx = 0;
      return;
    }
    oled.drawBox(bullet_x, bullet_y, bullet_x+1, bullet_y+1, 1, colour);
  }
}

bool Tank::checkCollisions(Tank &otherTank)
{
  // Check if the tanks crashed into each other
  int height = isDead() ? TANK_HEIGHT_DEAD : TANK_HEIGHT_ALIVE;
  bool dead = otherTank.y > y-height && otherTank.y < y+height && otherTank.x > x-(TANK_WIDTH) && otherTank.x < x+TANK_WIDTH;
  // Check if the other tank shot this one
  if(otherTank.bullet_dx && otherTank.bullet_y > y-height && otherTank.bullet_y < y+height && otherTank.bullet_x > x-TANK_WIDTH && otherTank.bullet_x < x+TANK_WIDTH) {
    dead = true;
    oled.drawBox(otherTank.bullet_x, otherTank.bullet_y, otherTank.bullet_x+1, otherTank.bullet_y+1, 1, BLACK);
    otherTank.bullet_dx = 0;
  }
  if(dead && !isDead()) {
    state = DEAD;
    next_transition = millis() + 2000;
  }
  return dead;
}

inline bool Tank::isDead()
{
  return state == DEAD || state == RESPAWNING;
}


void loop()
{
  // Check for any interactions (a collision, or one tank shot the other)
  bool brown_was_dead = brown_tank.isDead();
  bool green_was_dead = green_tank.isDead();
  brown_tank.checkCollisions(green_tank);
  green_tank.checkCollisions(brown_tank);
  if((brown_tank.isDead() && !brown_was_dead) || (green_tank.isDead() && !green_was_dead)) {
    // Tank newly dead, play a sound effect
    if(brown_tank.isDead() && green_tank.isDead()) {
      noTone(pin_piezo);
      tone(pin_piezo, 220, 20);
    } else if(brown_tank.isDead()) {
      noTone(pin_piezo);
      tone(pin_piezo, 660, 20);
    } else {
      noTone(pin_piezo);
      tone(pin_piezo, 220, 10);
    }
  }

  if(!green_tank.isDead()) {
    // Use joystick to move green (player) tank
    int dx = map(analogRead(analog_x), 0, 1024, -5, 5);
    if(dx<0) dx++; // favour 0 if joystick is around the centre pointa
    if(dx>0) dx--;
    int dy = map(analogRead(analog_y), 0, 1024, 5, -5);
    if(dy<0) dy++; // favour 0 if joystick is around the centre pointa
    if(dy>0) dy--;
    green_tank.moveTankBy(dx, dy);

    // Use button to fire from green (player) tank
    if(digitalRead(pin_button) == LOW)
      green_tank.fire();
  }
  green_tank.drawTank();

  if(!brown_tank.isDead()) {
    // Randomly move brown (computer) tank
    static int brown_dx = 0;
    static int brown_dy = 0;
    if(random(20) == 0) {
      // Change horizontal direction
      brown_dx = random(9) - 4; // -4 to +4
    }
    if(random(20) == 0) {
      // Change vertical direction
      brown_dy = random(9) -4; // -4 to +4
    }
    brown_tank.moveTankBy(brown_dx,brown_dy);

    // Check if brown tank should fire (ie is it roughly facing the green tank)
    if(abs(brown_tank.y - green_tank.y) < 48 && (bool)brown_tank.facing == (green_tank.x > brown_tank.x) && random(3) == 0) {
      brown_tank.fire();
    }
  }
  brown_tank.drawTank();
}

