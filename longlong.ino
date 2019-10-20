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

void printLongLong(struct LongLong a) {
  printAllBitsInLong(a.left);
  Serial.print(" ");
  printAllBitsInLong(a.right);
  Serial.println();
}

struct LongLong leftShiftLongLong(struct LongLong a, byte amount) {
  struct LongLong ret;
  
  unsigned long offset = LONG_LENGTH - amount;
  unsigned long bitmask = LONG_FULL << offset;
  unsigned long carried = (a.right & bitmask) >> offset;
  
  ret.left = a.left << amount;
  ret.left += carried;
  ret.right = a.right << amount;

  return ret;
}

struct LongLong rightShiftLongLong(struct LongLong a, byte amount) {
  struct LongLong ret;
  
  unsigned long offset = LONG_LENGTH - amount;
  unsigned long bitmask = LONG_FULL >> offset;
  unsigned long carried = (a.left & bitmask) << offset;
  
  ret.right = a.right >> amount;
  ret.right += carried;
  ret.left = a.left >> amount;

  return ret;
}

struct LongLong andLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong ret;
    
  ret.left = a.left & b.left;
  ret.right = a.right & b.right;

  return ret;
}

struct LongLong orLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong ret;
    
  ret.left = a.left | b.left;
  ret.right = a.right | b.right;

  return ret;
}

struct LongLong xorLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong ret;
    
  ret.left = a.left ^ b.left;
  ret.right = a.right ^ b.right;

  return ret;
}

struct LongLong notLongLong(struct LongLong a) {
  struct LongLong ret;
    
  ret.left = ~a.left;
  ret.right = ~a.right;

  return ret;
}


void setup() {
  Serial.begin(9600);
  // Demo
  struct LongLong i = {.right = 0xF0F0F0F0, .left = 0x347DEAB3}; 
  struct LongLong j = {.right = 0xFFFF0000, .left = 0x33334444};

  Serial.println("Right Shift");
  printLongLong(i);
  rightShiftLongLong(i, 2);
  printLongLong(leftShiftLongLong(i,2));
  
  Serial.println("Left Shift");
  printLongLong(i); 
  printLongLong(leftShiftLongLong(i,2));

  Serial.println("And");
  printLongLong(i);
  printLongLong(j);
  printLongLong(andLongLong(i, j));

  Serial.println("Not");
  printLongLong(i);
  printLongLong(j);
  printLongLong(notLongLong(i));

  Serial.println("Or");
  printLongLong(i);
  printLongLong(j);
  printLongLong(orLongLong(i, j));

  Serial.println("Xor");
  printLongLong(i);
  printLongLong(j);
  printLongLong(xorLongLong(i, j));
}

void loop() {
  
}
