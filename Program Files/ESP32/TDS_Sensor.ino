#define TdsSensorPin 1
#define VREF 5.0      
#define SCOUNT  30           
int analogBuffer[SCOUNT];   
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

short qn=0, qm=0;
float qx[1000];


void setup_TDS()
{
  pinMode(TdsSensorPin, INPUT);
  for(int m=0;m<1000;m++){ qx[m]=0; }
}

String get_TDS(void){
  String ss="";
  int m, n;
  if(qn!=1000){ n=0; }else{ n = qm; }
  for(m=0;m<(qn-1);m++){
    ss += String(qx[n++])+',';
    if(n>999){ n=0; }
  }
  ss += String(qx[n++]);
  return ss;
}

void loop_TDS()
{
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);   
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 8192.0; 
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); 
    float compensationVolatge = averageVoltage / compensationCoefficient; 
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; 
  }
  if (tdsValue!=0){
    Serial.print("TDS Value:");
    Serial.print(tdsValue);
    Serial.println("ppm");
  }
  
  qx[qm++] = tdsValue;
  if(qm>999){ qm=0; }
  if(qn<1000){ qn++; }
}
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
