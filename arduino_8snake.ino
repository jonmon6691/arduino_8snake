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
void on_button(int pin, int debounce, void (*callback)(int8_t), const int8_t context)
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
int8_t current_direction = 0;
unsigned long game_speed_ms = 500;

struct {
  uint8_t x;
  uint8_t y;
  uint8_t color;
} food;

uint8_t snake_color;

#define NORTH (1)
#define EAST  (2)
#define WEST  (-2)
#define SOUTH (-1)

#define GREEN_FOOD (8*8 + LED_GREEN)
#define RED_FOOD (8*8 + LED_RED)
#define YELLOW_FOOD (8*8 + LED_YELLOW)
#define SNAKE_BODY (1)
#define EMPTY (-1)

// Returns true if you can go in the requested direction
int8_t check(int8_t next)
{
  int8_t value = snake[next];
  if (value >= 0 && value < 8*8) return SNAKE_BODY;
  else return value;
}

void lose()
{
  for (int i = 0; i < 1000/100; i++) {
    snake_color = (i % 3) + 1;
    update_screen();
    delay(100);
  }
  setup();
}

int8_t next_head(int8_t direction)
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
}

void shrink()
{
  int8_t last = head;
  for (int8_t i = head; snake[i] != -1; last = i, i = snake[i]);
  snake[last] = -1;
}

void move(int8_t next)
{
  switch (check(next))
  {
  case SNAKE_BODY: lose(); break;
  case RED_FOOD: game_speed_ms = game_speed_ms * 80 / 100; // fall thru
  case GREEN_FOOD: grow(next); new_food(next); break;
  case EMPTY: shrink(); grow(next); break;
  default: break;
  }
  update_screen();
}

// Button handler
void turn(int8_t direction)
{
  if (current_direction + direction != 0)
    current_direction = direction;
}

void update_screen()
{
  matrix.clear();
  matrix.drawPixel(food.x, food.y, food.color);
  draw_snake();
  matrix.writeDisplay();
}

void draw_snake()
{
  for (int i = head; ; i = snake[i]) {
    matrix.drawPixel(x(i), y(i), snake_color);
    if (snake[i] == -1) break;
  }
}

void new_food(int8_t next)
{
  int8_t proposal;
  do {
    food.x = random(8);
    food.y = random(8);
    proposal = index(food.x, food.y);
  } while (check(proposal) != EMPTY && proposal != next);
  food.color = random(100) > 10 ? LED_GREEN : LED_RED;
  snake[proposal] = 8*8 + food.color;
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

  randomSeed(analogRead(A7));

  for (int i = 0; i < 8*8; i++) snake[i] = EMPTY;
  new_food(head);
  snake_color = LED_YELLOW;
  current_direction = random(2) ? SOUTH : EAST;
  update_screen();
}

void loop()
{
  if (millis() % game_speed_ms == 0) {
    move(next_head(current_direction));
  }
  // Polling loop
  on_button(UB, 25, turn, NORTH);
  on_button(DB, 25, turn, SOUTH);
  on_button(LB, 25, turn, WEST);
  on_button(RB, 25, turn, EAST);
}
