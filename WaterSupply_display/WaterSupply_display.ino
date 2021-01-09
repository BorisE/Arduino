#include <LCD_1602_RUS.h>

LCD_1602_RUS lcd(0x3F, 20, 4);


void pump(int onflag);
void pump_relay(int onflag);
void MainLine(int onflag=1);

void setup()
{
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);

  MainLine(1);
  pump(1);
  
  lcd.setCursor(0,2);
  lcd.print("Бак: ");
  lcd.print("closed");
  
}

void loop()
{
  delay(2000);
  pump(0);
  pump_relay(0);
  pressure(2.12);
  flow(0);
  delay(2000);
  
  pump(1);
  pump_relay(1);
  pressure(0.21);
  flow(29);
}

void MainLine(int onflag){
  lcd.setCursor(0,0);
  lcd.print("Линия: ");
  if (onflag==1) {
    lcd.print("opened");
  }else{
    lcd.print("closed");
  }
}

void pump(int onflag=1) {
  lcd.setCursor(0,1);
  lcd.print("Насос: ");
  if (onflag==1) {
    lcd.print("opened");
  }else{
    lcd.print("closed");
  }
}

void pump_relay(int onflag=1) {
  lcd.setCursor(16,1);
  if (onflag==1) {
    lcd.print("220V");
  }else{
    lcd.print("  0V");
  }
}

void pressure(float press) {
  lcd.setCursor(14,0);
  lcd.print(press,1);
  lcd.print("bar");
}

void flow(float flow) {
  lcd.setCursor(15,2);
  if (flow<10) lcd.print(" ");
  lcd.print(flow,0);
  lcd.print("l/m");
}
