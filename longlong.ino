const byte LONG_LENGTH = 32;
const unsigned long LONG_FULL = 0xFFFFFFFF;
const unsigned long LONG_EMPTY = 0x00000000;
const unsigned long LEFTMOST = 0x80000000;
const byte LT = 0;
const byte GT = 1;
const byte EQ = 2;

struct LongLong
{
  unsigned long right, left;
};

// Adapted from https://forum.arduino.cc/index.php?topic=46320.msgo35376#msg335376
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

struct LongLong leftShiftLongLong(struct LongLong a, long amount) {
  struct LongLong ret;
  
  unsigned long offset = LONG_LENGTH - amount;
  unsigned long bitmask = LONG_FULL << offset;
  unsigned long carried = (a.right & bitmask) >> offset;
  
  ret.left = a.left << amount;
  ret.left += carried;
  ret.right = a.right << amount;

  return ret;
}

struct LongLong rightShiftLongLong(struct LongLong a, long amount) {
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

bool boolLongLong(struct LongLong a) {
  return a.right > LONG_EMPTY || a.left > LONG_EMPTY;
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

// Logic based on https://stackoverflow.com/a/4068918
struct LongLong addLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong carry = andLongLong(a, b);
  struct LongLong ret = xorLongLong(a, b);

  while (boolLongLong(carry) == true) {
    struct LongLong shifted_carry = leftShiftLongLong(carry, 1);
    carry = andLongLong(ret, shifted_carry);
    ret = xorLongLong(ret, shifted_carry);
  }

  return ret;
}

// Case GT adapted from https://stackoverflow.com/a/10097167
bool cmpLongLong(struct LongLong a, struct LongLong b, char op) {
  bool ret;
  
  switch(op) {
    case EQ:
        ret = a.right == b.right && a.left == b.left;
        break;

    case GT:
      {
        // Bits in a that are less than the bits in the same positions in b
        struct LongLong ltb = andLongLong(notLongLong(a), b);
        
        // Bits in a that are greater than the bits in the same positions in b
        struct LongLong gtb = andLongLong(a, notLongLong(b));
  
        struct LongLong place_val;
        for (place_val = {.right = LONG_EMPTY, .left = LEFTMOST};  boolLongLong(place_val); place_val = rightShiftLongLong(place_val, 1)) {
          long pv = place_val.right + (2 * place_val.left);
          ltb = orLongLong(ltb, rightShiftLongLong(ltb, pv));
        }
  
        struct LongLong isGt = andLongLong(gtb, notLongLong(ltb));
  
        ret = boolLongLong(isGt);
        break;
      }

   case LT:
        bool isGt = cmpLongLong(a, b, GT);

        if (isGt) {
          ret = false;
        } else {
          if (cmpLongLong(a, b, EQ)) {
            ret = false;
          } else {
            ret = true;
          }
        }
  
        break;
  }

  return ret;
}

// Logic based on https://ideone.com/CA7jW8
struct LongLong subLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong borrow;

  while (boolLongLong(b) == true) {
    borrow = andLongLong(notLongLong(a), b);
    a = xorLongLong(a, b);
    b = leftShiftLongLong(borrow, 1);
  }

  return a;
}

struct LongLong multLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong ret = {.right = LONG_EMPTY, .left = LONG_EMPTY};
  struct LongLong one = {.right = 1, .left = 0};
  
  while (boolLongLong(b) == true) {
    b = subLongLong(b, one);
    ret = addLongLong(ret, a);
  }

  return ret;
}

struct LongLong divLongLong(struct LongLong a, struct LongLong b) {
  struct LongLong ret = {.right = LONG_EMPTY, .left = LONG_EMPTY};
  struct LongLong one = {.right = 1, .left = 0};

  while (cmpLongLong(a, b, GT)) {
    a = subLongLong(a, b);
    ret = addLongLong(ret, one);
  }

  if (cmpLongLong(a, b, EQ)) {
    ret = addLongLong(ret, one);
  }

  return ret;
}


void setup() {
  Serial.begin(9600);
  // Demo
  struct LongLong i = {.right = 0xFFF00F00, .left = 0xFF003210}; 
  struct LongLong j = {.right = 0xFF0EE222, .left = 0x80612DA0};

  Serial.println("Right Shift");
  printLongLong(i);
  printLongLong(rightShiftLongLong(i, 2));
  
  Serial.println("Left Shift");
  printLongLong(i); 
  printLongLong(leftShiftLongLong(i,2));

  Serial.println("And");
  printLongLong(i);
  printLongLong(j);
  printLongLong(andLongLong(i, j));

  Serial.println("Not");
  printLongLong(i);
  printLongLong(notLongLong(i));

  Serial.println("Or");
  printLongLong(i);
  printLongLong(j);
  printLongLong(orLongLong(i, j));

  Serial.println("Xor");
  printLongLong(i);
  printLongLong(j);
  printLongLong(xorLongLong(i, j));

  Serial.println("Add");
  printLongLong(i);
  printLongLong(j);
  printLongLong(addLongLong(i, j));
  
  Serial.println("Subtract");
  printLongLong(i);
  printLongLong(j);
  printLongLong(subLongLong(i, j));

  Serial.println("Multiply");
  struct LongLong k = {.right = LEFTMOST, .left = 0};
  struct LongLong l = {.right = 8, .left = 0};
  printLongLong(k);
  printLongLong(l);
  printLongLong(multLongLong(k, l));

  Serial.println("Divide");
  k = {.right = 8, .left = 0};
  l = {.right = 2, .left = 0};
  printLongLong(k);
  printLongLong(l);
  printLongLong(divLongLong(k, l));

  Serial.println("Comparisons");
  Serial.println("EQ");
  printLongLong(i);
  printLongLong(j);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, j, EQ));
  printLongLong(i);
  printLongLong(i);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, i, EQ));
  
  Serial.println("GT");
  printLongLong(i);
  printLongLong(j);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, j, GT));
  printLongLong(j);
  printLongLong(i);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(j, i, GT));
  printLongLong(i);
  printLongLong(i);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, i, GT));

  Serial.println("LT");
  printLongLong(i);
  printLongLong(j);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, j, LT));
  printLongLong(j);
  printLongLong(i);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(j, i, LT));
  printLongLong(i);
  printLongLong(i);
  Serial.print("Bool: ");
  Serial.println(cmpLongLong(i, i, LT));
}

void loop() {
  
}
