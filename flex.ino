int flexsensor = A3; 
 
void setup() 
{ 
  Serial.begin(9600);
  pinMode(flexsensor, INPUT);
} 
 
void loop() 
{ 
  int Value;
  Value = analogRead(flexsensor);
  Serial.print("joint: ");
  Serial.println(Value);
  
  delay(200);
} 