#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <limits.h>

//CẤU HÌNH
#define LED_GPIO 48 // LED báo mode 2
#define I2C_SDA 11
#define I2C_SCL 12

const byte rows = 4;
const byte cols = 4;

char keys_mode1[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'R','0','#','D'}
};

char keys_mode2[rows][cols] = {
  {'1','2','3','J'},
  {'4','5','6','Q'},
  {'7','8','9','M'},
  {'R','0','#','D'}
};

byte rowPins[rows] = {5, 6, 7, 8};
byte colPins[cols] = {21, 10, 17, 18};

Keypad keypad1 = Keypad(makeKeymap(keys_mode1), rowPins, colPins, rows, cols);
Keypad keypad2 = Keypad(makeKeymap(keys_mode2), rowPins, colPins, rows, cols);

Keypad *keypad = &keypad1;

// LCD
LiquidCrystal_I2C lcd(0x21, 16, 2);

// Global states
int mode = 1;
String expr = "";

// Queue chứa ký tự nhấn 
QueueHandle_t keyQueue;

// MAP KEY
char mapKey(char k) {
  if (mode == 1) {
    if (k == 'A') return '+';
    if (k == 'B') return '-';
    if (k == 'C') return '*';
  }
  if (mode == 2) {
    if (k == 'J') return '/';
    if (k == 'Q') return 'q';
    if (k == 'M') return 'm';
  }
  return k;
}


int precedence(char op) { // xác định độ ưu tiên toán tử
  if (op == '+' || op == '-') return 1;
  if (op == '*' || op == '/' || op == 'm') return 2;
  return 0;
}

double applyOp(char op, double b, double a) { // thực hiện phép toán
  switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return (b == 0) ? INT32_MIN : a / b;
    case 'm': return (b == 0) ? INT32_MIN : fmod(a, b);
  }
  return 0;
}

double evaluateExpression(String expr) { // đánh giá biểu thức
  const char* s = expr.c_str();
  int i = 0;

  double values[30];
  char ops[30];
  int vTop = -1, oTop = -1;

  while (s[i]) { // duyệt chuỗi
    if (s[i]==' ') { i++; continue; }

    if ((s[i]=='+' || s[i]=='-') &&
         (i==0 || (!isdigit(s[i-1]) && s[i-1]!=')'))) {

      int sign = 1;
      while(s[i]=='+' || s[i]=='-') {
        if(s[i]=='-') sign*=-1;
        i++;
      }

      if(!isdigit(s[i])) return INT32_MIN+3; // lỗi cú pháp
      
      long val = 0;
      while(isdigit(s[i])) {
        val = val*10 + (s[i]-'0');
        i++;
      }

      values[++vTop] = val * sign;
      continue;
    }

    if(isdigit(s[i])) {
      long val = 0;
      while(isdigit(s[i])) {
        val = val*10 + (s[i]-'0');
        i++;
      }
      values[++vTop] = val;
      continue;
    }

    if(s[i]=='+'||s[i]=='-'||s[i]=='*'||s[i]=='/'||s[i]=='m') {
      while(oTop>=0 && precedence(ops[oTop]) >= precedence(s[i])) {
        if(vTop<1) return INT32_MIN+3;
        double b = values[vTop--];
        double a = values[vTop--];
        char op = ops[oTop--];

        if((op=='/'||op=='m') && b==0) return INT32_MIN;

        values[++vTop] = applyOp(op,b,a);
      }
      ops[++oTop] = s[i];
      i++;
      continue;
    }

    return INT32_MIN+3;
  }

  while(oTop>=0) {
if(vTop<1) return INT32_MIN+3;
    double b = values[vTop--];
    double a = values[vTop--];
    char op = ops[oTop--];

    if((op=='/'||op=='m') && b==0) return INT32_MIN;

    values[++vTop] = applyOp(op,b,a);
  }

  if(vTop!=0) return INT32_MIN+3;
  return values[vTop];
}

String toBinary(long x) {
  if (x == 0) return "0";
  String out = "";
  while (x > 0) {
    out = String(x % 2) + out;
    x /= 2;
  }
  return out;
}


//TASK ĐỌC KEYPAD

void TaskKeypad(void *pv) {
  while (1) {
    char key = keypad->getKey();
    if (keypad->isPressed(key) && key) {
      xQueueSend(keyQueue, &key, 0);
    }
    vTaskDelay(pdMS_TO_TICKS(20)); // đọc keypad mỗi 20ms
  }
}


// XỬ LÝ LOGIC
void TaskLogic(void *pv) {
  char key;

  while (1) {
    if (xQueueReceive(keyQueue, &key, portMAX_DELAY)) {

      // NÚT ĐỔI MODE
      if (key == 'D') {
        mode = (mode == 1) ? 2 : 1;
        keypad = (mode == 1) ? &keypad1 : &keypad2;

        digitalWrite(LED_GPIO, mode == 2);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode:");
        lcd.setCursor(0, 1);
        lcd.print(mode);

        vTaskDelay(pdMS_TO_TICKS(1200));
        lcd.clear();
        lcd.print(expr);
        continue;
      }

      char k = mapKey(key);

      // CLEAR
      if (k == 'R') {
        expr = "";
        lcd.clear();
        lcd.print("CLEARED");
        vTaskDelay(pdMS_TO_TICKS(800));
        lcd.clear();
        continue;
      }

      // INPUT
      if (isdigit(k) || k=='+'||k=='-'||k=='*'||k=='/'||k=='m') {

     

        expr += k;
        lcd.clear();
        lcd.print(expr);
        continue;
      }

      // Nhị phân
      if (k == 'q') {
        long val = expr.toInt();
        String bin = toBinary(val);

        lcd.clear();
        lcd.print("BIN:");
        lcd.setCursor(0,1);
        lcd.print(bin);

        expr="";
        continue;
      }

      // Tính toán
      if (k == '#') {
        double result = evaluateExpression(expr);

        lcd.setCursor(0, 1);

        if(result == INT32_MIN) {
          lcd.clear();
          lcd.print("Math Error");
        } else if(result == INT32_MIN+3) {
          lcd.clear();
          lcd.print("Syntax Error");
        } else {
          lcd.print("= ");
          lcd.print(result);
        }

        expr = "";
        continue;
      }
    }
  }
}


// SETUP

void setup() {
  Serial.begin(115200);

  pinMode(LED_GPIO, OUTPUT);

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.begin();
  lcd.backlight();

  lcd.print("May tinh cua em!");
  lcd.setCursor(0,1);
  lcd.print("Mode 1");
  vTaskDelay(pdMS_TO_TICKS(1500));
  lcd.clear();

  // Tạo queue
  keyQueue = xQueueCreate(20, sizeof(char));

  // Tạo task
  xTaskCreate(TaskKeypad, "Keypad Task", 4096, NULL, 2, NULL); //RTOS_PRIORITY_MAX
xTaskCreate(TaskLogic, "Logic Task", 8192, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(1); 
}