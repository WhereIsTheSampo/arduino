const int POT_PIN = A0;
const int RED_PIN = 2;
const int YELLOW_PIN = 3;
const int GREEN_PIN = 4;

const int RED_YELLOW = 450;
const int YELLOW_GREEN = 550;



void setup() 
{
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
}

void loop() 
{
  int potValue = analogRead(POT_PIN);

  Serial.println(potValue);

  if (1023 > potValue && potValue >= YELLOW_GREEN)
  {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    Serial.println("GREEN");
  }
  else if (YELLOW_GREEN > potValue && potValue >= RED_YELLOW)
  {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    Serial.println("YELLOW");
  }
  else if (RED_YELLOW > potValue && potValue >= 0)
  {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    Serial.println("RED");
  }
  else
  {
    Serial.println("ERROR");
  }
   
  delay(200);
  
}
