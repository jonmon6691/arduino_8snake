/*************** 8snake ***************
 *          It's just snake
 *          Jon Wallace 2020
 */

#include "Adafruit_LEDBackpack.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

// Controller buttons' pin numbers
// Has to be sequential because the debounce code is lazy, ¯\_(ツ)_/¯
#define RB (14)
#define DB (15)
#define UB (16)
#define LB (17)

// super simple rising edge and debounce detector
// Warning: only works if all 4 button pin numbers are sequential
uint8_t db[4] = {0};
void on_button(int pin, int debounce, void (*callback)())
{
  if (!digitalRead(pin)) {
    if (!db[pin%4]) {
      db[pin%4] = debounce;
      callback();
    }
  } else if (db[pin%4]) db[pin%4]--;
}

// state objects
struct {
  int x;
  int y;
  int color;
} you;

int current_direction = 0;

#define SOUTH (1 << 0)
#define EAST  (1 << 1)
#define NORTH (1 << 2)
#define WEST  (1 << 3)

// Returns true if you can go in the requested direction
bool check(uint8_t direction)
{
  switch (direction)
  {
  case NORTH:
    return true;
    break;
  
  case SOUTH:
    return true;
    break;

  case EAST:
    return true;
    break;

  case WEST:
    return true;
    break;

  default:
    return false;
  }
}

void lose()
{
  return;
}

// Button handlers
void move_up()
{
  if (!check(NORTH)) lose();
  update_screen();
}

void move_down()
{
  if (!check(SOUTH)) lose();
  update_screen();
}

void move_left()
{
  if (!check(WEST)) lose();
  update_screen();
}

void move_right()
{
  if (!check(EAST)) lose();
  update_screen();
}

void update_screen()
{
  matrix.clear();
  draw_snake();
  matrix.writeDisplay();
}

void draw_snake()
{
  ;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("8snake!");
  
  pinMode(RB, INPUT_PULLUP);
  pinMode(DB, INPUT_PULLUP);
  pinMode(UB, INPUT_PULLUP);
  pinMode(LB, INPUT_PULLUP);
  
  // Screen setup
  matrix.begin(0x70);
  matrix.setBrightness(6);

  // My screen is soldered in the "1" direction
  matrix.setRotation(1);

  // Init
  you.x = you.y = 1; // Start in the upper left corner
  you.color = LED_GREEN;

  update_screen();
}

void loop()
{
  // Polling loop
  on_button(UB, 5, move_up);
  on_button(DB, 5, move_down);
  on_button(LB, 5, move_left);
  on_button(RB, 5, move_right);
}
