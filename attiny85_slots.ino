#include "attiny85_slots.h"  //constants and bitmaps

#include "ssd1306.h"  //oled mini-lib
SSD1306 oled;

#if defined __AVR_ATtiny85__
    #define BUTTON 1
    #define SPEAKER 4
    #define RNG_SOURCE 3 //do not connect this to anything. leave it floating for entropy. did not work on pin 4.
    #define ANIMATION_DELAY 200
#else
    #define BUTTON 4
    #define SPEAKER 3
    #define RNG_SOURCE 14
    #define ANIMATION_DELAY 200
#endif

uint8_t values[] = {
  0, 1, 2,  //new values rolling in
  5, 5, 5,  //top row
  3, 4, 0,  //middle row
  0, 0, 0,  //bottom row
  0, 0, 0   //falling off the screen
};

uint8_t postion[] = { //used for roll animation
  0, 0, 0
};

uint16_t score = 100;
uint8_t price = 5;

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  oled.begin();

  rng_seed();

  //TODO: detect button press, and jump to setup menu

  draw_state();
}

void loop() {
  if(score < price){
    game_over();
  }

  while(digitalRead(BUTTON) == HIGH){} //wait for button press
  //oled.clear(); //visual feedback / debounce
  while(digitalRead(BUTTON) == LOW){} //wait for release

  score -= price;

  for (uint8_t i = 0; i < random(4,12); i++){
    roll_wheel(0);  roll_wheel(1);  roll_wheel(2); //roll all 3
    while(postion[0] > 0 || postion[1] > 0 || postion[2] > 0){
      draw_state();
    }
    draw_state();
  }

  for (uint8_t i = 0; i < random(1,5); i++){
    roll_wheel(1);  roll_wheel(2); //just the two
    while(postion[0] > 0 || postion[1] > 0 || postion[2] > 0){
      draw_state();
    }
    draw_state();
  }

  for (uint8_t i = 0; i < random(1,5); i++){
    roll_wheel(2); //just the last one.
    while(postion[0] > 0 || postion[1] > 0 || postion[2] > 0){
      draw_state();
    }
    draw_state();
  }

  //check values, display winnings.
  check_winnings();

  delay(50);
}

void draw_state(){
  oled.fill(0, 7, 32, 47, 0x00); //vertical bars.
  oled.fill(0, 7, 80, 95, 0x00);

  for (uint8_t i = 0; i < 3; i++){
    draw_column(i);
    if (postion[i] > 0){ postion[i]--; }
  }
}

void draw_column(uint8_t column){
  for (uint8_t i = 0; i < 3; i++){
    int8_t page = (i*3) -postion[column];
    if (page < 0 || page > 7){ continue; } //bounds checking

    uint8_t px_col = column * 48;

    oled.area(page, page+1, px_col, px_col+31);
    oled.writeP(SSD1306_DATA, int(bitmap + (values[(i*3) + column] *64)), 64);

    if (page < 1 || page > 5) {continue;}
    oled.fill(page -1, page -1, px_col, px_col +31, 0x00);
    oled.fill(page +2, page +2, px_col, px_col +31, 0x00); //black bar between icons
  }
}

void roll_wheel(uint8_t wheel){
  //shift values 'down'
  for (uint8_t i = wheel +12; i > 2; i -= 3){
    values[i] = values[i-3];
  }
  //now randomize
  //values[wheel] = random(sizeof(multiplier) / sizeof(multiplier[0]));
  values[wheel] = weighted_random();

  postion[wheel] = 2;
}

uint8_t weighted_random(){
  //return 5; //DEBUG

  uint16_t weight_total = 0;
  for (uint8_t i = 0; i < (sizeof(weight) / sizeof(weight[0])); i++){
    weight_total += weight[i];
  }

  uint16_t rng = random(weight_total);

  for (uint8_t i = 0; i < (sizeof(weight) / sizeof(weight[0])); i++){
    if (rng < weight[i]){return i;}
    rng -= weight[i];
  }
  //should never reach here
  return 0;
}


void check_winnings(){
  uint16_t round_winnings = 0;
  int8_t wildmatch = -1;

  for (uint8_t i = 0; i < sizeof(winlines); i += 3){
    for (uint8_t j = 0; j < 3; j++){ // find one of them that isn't wild to use as base score
      if (wild[values[winlines[i+j]]] == false){
        wildmatch = values[winlines[i+j]];
      }
    }

    if (values[winlines[i]] == values[winlines[i+1]] || wild[values[winlines[i]]] || wild[values[winlines[i+1]]]){ //first two will either match or one (or both) will be wild
      if (values[winlines[i]] == values[winlines[i+2]] || wild[values[winlines[i]]] || wild[values[winlines[i+2]]]){ //same compare for first/third
        if (values[winlines[i+1]] == values[winlines[i+2]] || wild[values[winlines[i+1]]] || wild[values[winlines[i+2]]]){ //same compare for second/third
          blink_winline(i);

          if (wildmatch == -1){ //if all 3 compares succeeded, but wildmatch is still -1, then they are all wild.
            round_winnings += price * multiplier[values[winlines[i]]];
            blink_winline(i, true); //blink again, inverted this time.

          }else{ //if wildmatch is set to an actual value, it's used instead
            round_winnings += price * multiplier[wildmatch];
          }
        }
      }
    }
    wildmatch = -1; //reset this so the next check is valid.
  }

  score += round_winnings;

  delay(ANIMATION_DELAY);
  display_winnings(round_winnings);
}

void blink_winline(uint8_t line){blink_winline(line,false);} //default
void blink_winline(uint8_t line, bool inverted){
  for (uint8_t j = 0; j < 3; j++){
    for (uint8_t i = 0; i < 3; i++){
      uint8_t page = (winlines[line + i] / 3) * 3;
      uint8_t cols = (winlines[line + i] % 3) * 48;

      oled.fill(page, page +1, cols, cols +31, inverted?0xFF:0x00);
    }
    delay(ANIMATION_DELAY);
    draw_state();
    delay(ANIMATION_DELAY);
  }
}

void display_winnings(uint16_t round_winings){
  oled.fill(2, 2,  24, 104, 0xF0); //top
  oled.fill(5, 5,  24, 104, 0x0F); //bottom
  oled.fill(2, 5,  20,  23, 0x00); //left space
  oled.fill(3, 4,  24,  27, 0xFF); //left bar
  oled.fill(3, 4, 101, 104, 0xFF); //right bar
  oled.fill(2, 5, 105, 108, 0x00); //right space
  oled.fill(3, 4,  28, 100, 0x00); //clear middle


  oled.area(3, 3, 30, 60);
  oled.writeP(SSD1306_DATA, int(msg_win), sizeof(msg_win));
  drawnum(3, 64, round_winings);

  oled.area(4, 4, 30, 60);
  oled.writeP(SSD1306_DATA, int(msg_total), sizeof(msg_total));
  drawnum(4, 64, score);
}


void drawnum(uint8_t page, uint8_t column, int16_t number){

  if (number < 0){ //draw minus for negative numbers
    oled.fill(page, page, column, column +4, 0x10);
    column += 5;
    number *= -1;
  }

  uint8_t digits = 0;
  int16_t tempnum = number;

  while (tempnum){
    digits++;
    tempnum /= 10;
  }

  oled.area(page, page, column, column + (digits *5));

  tempnum = number;

  for(int8_t i = digits; i > 0; i--){
    uint8_t single = tempnum / powint(10, i-1);
    tempnum = tempnum % powint(10, i-1);

    oled.writeP(SSD1306_DATA, int(numfont_4x6 + (single *4)), 4);
    oled.write(SSD1306_DATA, {0x00}, 1);
  }

}

int powint(int x, int y) //need this because 'pow()' function has rounding errors.
{
 int val=x;
 for(int z=0;z<=y;z++)
 {
   if(z==0)
     val=1;
   else
     val=val*x;
 }
 return val;
}


void game_over(){
  oled.inverted(true);
  while(1){} //TODO: just locks up for now. need to do something better.
}


void rng_seed(){
  uint32_t seedval = 0;
  for (uint8_t i = 0; i < 32; i++){
    seedval += ((analogRead(RNG_SOURCE) & 1) << i); //grab the lsb of the adc repeatedly since it'll tend to flicker whether it's tied high, low, or left floating.
  }

  randomSeed(seedval);
}
