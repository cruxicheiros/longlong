const byte LONG_LENGTH = 32;
const unsigned long LONG_FULL = 0xFFFFFFFF;
const unsigned long LONG_EMPTY = 0x00000000;
const unsigned long LEFTMOST = 0x80000000;


struct LongLong
{
  unsigned long right, left;
};

// Adapted from https://forum.arduino.cc/index.php?topic=46320.msg335376#msg335376
void printAllBitsInLong(unsigned long l) {

  unsigned long mask = LEFTMOST;  
  for (mask = LEFTMOST; mask; mask >>= 1) {
    if (mask & l) {
      Serial.print(1);
    } else {
      Serial.print(0);
    }
  }
}

void printLongLong(struct LongLong *ptr) {
  printAllBitsInLong(ptr->left);
  Serial.print(" ");
  printAllBitsInLong(ptr->right);
  Serial.println();
}

void leftShiftLongLong(struct LongLong *ptr, byte amount) {
  unsigned long offset = LONG_LENGTH - amount;
  unsigned long bitmask = LONG_FULL << offset;
  
  unsigned long carried = (ptr->right & bitmask) >> offset;
  
  ptr->left <<= amount;
  ptr->left += carried;
  ptr->right <<= amount;
}

void rightShiftLongLong(struct LongLong *ptr, byte amount) {
  unsigned long offset = LONG_LENGTH - amount;
  unsigned long carried = (ptr->left & amount) << offset;
  
  ptr->right >>= amount;
  ptr->right += carried;
  ptr->left >>= amount;
}

void setup() {
  Serial.begin(9600);
  // Code
  struct LongLong i = {.right = 0xFFFF0000, .left = 0x33334444}; 
  printLongLong(&i);
  rightShiftLongLong(&i, 3);
  printLongLong(&i);
}

void loop() {
  
}
