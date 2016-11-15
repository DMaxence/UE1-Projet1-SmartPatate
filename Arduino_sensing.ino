#define SET(x,y) (x |=(1<<y))        //-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))           // |
#define CHK(x,y) (x & (1<<y))               // |
#define TOG(x,y) (x^=(1<<y))                //-+

#define N 160  //How many frequencies

int sizeOfArray = N;
int nothing, touch, touch2, grab;

float results[N];            //-Filtered result buffer

/* ====================================================================================
 * initialize: Calibrating the Arduino to detect the different ways to touch the potato 
 ===================================================================================*/
void initialize (int &nothing, int &touch, int &touch2, int &grab)  {

  Serial.write("Calibrage en cours... Ne touchez pas la patate...\n3...\n");
  delay(1000);
  Serial.write("2...\n");
  delay(1000);
  Serial.write("1...\n");
  delay(1000);
  nothing = readValeur();
  Serial.write("Done !\n\nCalibrage en cours...\nVeuillez toucher la patate avec un doigt...\n3...\n");
  delay(1000);
  Serial.write("2...\n");
  delay(1000);
  Serial.write("1...\n");
  delay(1000);
  touch = readValeur();
  Serial.write("Done !\n\nCalibrage en cours...\nVeuillez toucher la patate avec deux doigts...\n3...\n");
  delay(1000);
  Serial.write("2...\n");
  delay(1000);
  Serial.write("1...\n");
  delay(1000);
  touch2 = readValeur();
  Serial.write("Done !\n\nCalibrage en cours...\nVeuillez prendre la patate a pleine main...\n3...\n");
  delay(1000);
  Serial.write("2...\n");
  delay(1000);
  Serial.write("1...\n");
  delay(1000);
  grab = readValeur();
  Serial.println("Done !\n\nCalibrage termine !");
  delay(3000);
  //Serial.println(nothing);
  //Serial.println(touch);
  //Serial.println(touch2);
  //Serial.println(grab);
}

/* ========================================================================
 * setup : Setting the pinModes for every used pins, and setting frequency
 * also calling initialize defined above
 ==========================================================================*/
void setup()
{
  
  TCCR1A=0b10000010;        //-Set up frequency generator
  TCCR1B=0b00011001;        //-+
  ICR1=110;
  OCR1A=55;

  for (int i = 0; i < N; i++) {
    results[i] = 0;
  }
  
  pinMode(8,INPUT);
  pinMode(9,OUTPUT);        //-Signal generator pin
  pinMode(10,OUTPUT); 
  pinMode(11,OUTPUT); 
  Serial.begin(115200);

  //Initializing values for touching, touching with 2, and grabbing
  initialize(nothing, touch, touch2, grab);
}

/* ==================================================================================
 * loop : main function with Arduino programming. Once the function reaches its end
 *  it is called again. Reads a value from the sensor, then judges how the sensor is
 *  being touched, according to the values calibrated before.
 ====================================================================================*/
void loop()
{
  int v = readValeur();

  digitalWrite(8, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  
  if (v < (nothing+touch/2))  { //nothing
    Serial.println("Nothing");
  } else if( v < (touch+touch2/2))  { //touch
    
    digitalWrite(8, HIGH);
    Serial.println("Touch");
    
  } else if (v < (touch2+grab/2)) { //touch2
    
    digitalWrite(10,HIGH);
    Serial.println("Touch2");
    
  } else  { //grab

    digitalWrite(11, HIGH);
  }

  delay(1000);
}

/* =============================================================================
 * readValeur: Reads a value from the Arduino. Listens to the port for N periods
 *  then returns the highest value
 ===============================================================================*/

float readValeur ()  {

  //array could have been used for better filtering

  unsigned int d;

  float Vmax = 0;
  
  for(d = 0; d < N; d++)
  {
    int v = analogRead(0);    //-Read response signal
    
    CLR(TCCR1B,0);          //-Stop generator
    TCNT1=0;                //-Reload new frequency
    ICR1=d;                 // |
    OCR1A=d/2;              //-+
    SET(TCCR1B,0);          //-Restart generator

    //Smoothing the period using average
    results[d] = results[d]*0.5+(float)(v)*0.5; //Filter results
    //We have waited for 160 periods

    //If we discover a new highest value, remember it
    if (results[d] > Vmax) {
      Vmax = results[d];
    }
  }

  //Then return it
  return Vmax;
}
