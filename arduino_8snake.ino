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
void on_button(int pin, int debounce, void (*callback)(uint8_t), const uint8_t context)
{
  if (!digitalRead(pin)) {
    if (!db[pin%4]) {
      db[pin%4] = debounce;
      callback(context);
    }
  } else if (db[pin%4]) db[pin%4]--;
}

#define index(x, y) ((y)*8 + (x))
#define x(i) ((i) % 8)
#define y(i) ((i) / 8)
int8_t head = index(2,2);
// index is a pixel in the screen y*8+x
// The value given by snake[index]-1 is the index of the next snake pixel
// Sentinal value 0 means no snake part at that pixel
int8_t snake[8*8];
uint8_t current_direction = 0;

#define SOUTH (1 << 0)
#define EAST  (1 << 1)
#define NORTH (1 << 2)
#define WEST  (1 << 3)

// Returns true if you can go in the requested direction
bool check(int8_t next)
{
  return snake[next] == -1;
}

void lose()
{
  return;
}

int8_t next_head(uint8_t direction)
{
  switch (direction) {
  case NORTH: return index(x(head), (8 + y(head) - 1) % 8); break;
  case SOUTH: return index(x(head), (8 + y(head) + 1) % 8); break;
  case EAST:  return index((8 + x(head) + 1) % 8, y(head)); break;
  case WEST:  return index((8 + x(head) - 1) % 8, y(head)); break;
  default:    return -1; break;
  }
}

void grow(int8_t next)
{
  snake[next] = head;
  head = next;
  Serial.println(head);
}

// Button handlers
void move(uint8_t direction)
{
  int8_t next = next_head(direction);
  if (!check(next)) {
    lose();
  } else {
    grow(next);
    current_direction = direction;
  }
  update_screen();
}

void turn(uint8_t direction)
{
  current_direction = direction;
}

void update_screen()
{
  matrix.clear();
  draw_snake();
  matrix.writeDisplay();
}

void draw_snake()
{
  for (int i = head; ; i = snake[i]) {
    Serial.print(x(i));Serial.print(" ");
    Serial.print(y(i));Serial.print(" ");
    Serial.print(snake[i]);Serial.println(" ");
    matrix.drawPixel(x(i), y(i), LED_YELLOW);
    if (snake[i] == -1) break;
  }
  Serial.println(" ");
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

  for (int i = 0; i < 8*8; i++) {
    snake[i] = -1;
  }
  update_screen();
}

void loop()
{
  if (millis() % 500 == 0) {
    move(current_direction);
  }
  // Polling loop
  on_button(UB, 25, turn, NORTH);
  on_button(DB, 25, turn, SOUTH);
  on_button(LB, 25, turn, WEST);
  on_button(RB, 25, turn, EAST);
}
