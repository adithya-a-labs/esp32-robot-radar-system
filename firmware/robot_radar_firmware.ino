#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

/* ===================== WIFI ===================== */
const char* ssid = "ROBOT_RADAR";
const char* password = "12345678";
WebServer server(80);

/* ===================== UI HTML ===================== */
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Robot Radar</title>
<style>
body {
  margin: 0;
  background: black;
  color: #00ff66;
  font-family: monospace;
  display: flex;
  height: 100vh;
}
#radarArea {
  flex: 4;
  display: flex;
  justify-content: center;
  align-items: center;
}
#panel {
  flex: 1;
  padding: 20px;
  border-left: 2px solid #00ff66;
}
canvas { background: black; }
.label { margin: 10px 0; font-size: 18px; }
button {
  margin-top: 20px;
  width: 100%;
  padding: 14px;
  background: black;
  color: #00ff66;
  border: 2px solid #00ff66;
  font-size: 18px;
}
button:hover { background: #00ff66; color: black; }
</style>
</head>

<body>
<div id="radarArea">
  <!-- BIGGER CANVAS -->
  <canvas id="radar" width="640" height="640"></canvas>
</div>

<div id="panel">
  <div class="label">MODE: <span id="mode">---</span></div>
  <div class="label">ANGLE: <span id="angle">---</span>°</div>
  <div class="label">DISTANCE: <span id="distance">---</span> cm</div>
  <div class="label">TARGET: <span id="locked">---</span></div>
  <button onclick="toggleMode()">TOGGLE MODE</button>
</div>

<script>
const canvas = document.getElementById("radar");
const ctx = canvas.getContext("2d");

const cx = canvas.width / 2;
const cy = canvas.height / 2;

// BIGGER RADAR
const radius = canvas.width / 2 - 90;

let angle = 0;
let distance = -1;
let mode = "AUTO";
let locked = false;

let pulses = [];
let echoes = [];
let lastPulseTime = 0;

const LED_COUNT = 7;

function fetchData() {
  fetch("/data")
    .then(r => r.json())
    .then(d => {
      angle = d.angle;
      distance = d.distance;
      mode = d.mode;
      locked = d.locked;

      document.getElementById("mode").innerText = mode;
      document.getElementById("angle").innerText = angle;
      document.getElementById("distance").innerText = distance;
      document.getElementById("locked").innerText = locked ? "YES" : "NO";

      if (locked && distance > 0 && distance < 100) {
        echoes.push({ a: angle, d: distance, alpha: 1.0 });
      }
    });
}

function updatePulses() {
  const now = Date.now();
  if (mode === "AUTO" && !locked && now - lastPulseTime > 600) {
    pulses.push({ r: 0, alpha: 0.35 });
    lastPulseTime = now;
  }
  pulses.forEach(p => { p.r += 3; p.alpha -= 0.004; });
  pulses = pulses.filter(p => p.alpha > 0 && p.r < radius);
}

/* -------- BIGGER LED BAR -------- */
function drawLEDBar() {
  const barX = 60;
  const barY = canvas.height - 45;
  const barWidth = canvas.width - 120;
  const barHeight = 20;   // THICKER

  let lit = 0;
  if (distance > 0) {
    lit = Math.round((100 - distance) / (100 / LED_COUNT));
    lit = Math.min(LED_COUNT, Math.max(0, lit));
  }

  for (let i = 0; i < LED_COUNT; i++) {
    let color = "#003300";
    if (i < lit) {
      if (i < LED_COUNT * 0.4) color = "#00ff66";
      else if (i < LED_COUNT * 0.7) color = "#ffaa00";
      else color = "#ff3333";
    }
    ctx.fillStyle = color;
    ctx.fillRect(
      barX + i * (barWidth / LED_COUNT),
      barY,
      barWidth / LED_COUNT - 8,
      barHeight
    );
  }
}

function drawRadar() {
  ctx.clearRect(0,0,canvas.width,canvas.height);
  const flashRed = locked && Math.floor(Date.now()/300)%2===0;

  ctx.strokeStyle = flashRed ? "#ff3333" : "#00ff66";
  ctx.lineWidth = 2;

  ctx.beginPath();
  ctx.arc(cx, cy, radius, 0, Math.PI*2);
  ctx.stroke();

  for(let i=1;i<=4;i++){
    ctx.beginPath();
    ctx.arc(cx, cy, radius*i/4, 0, Math.PI*2);
    ctx.stroke();
  }

  pulses.forEach(p=>{
    ctx.strokeStyle=`rgba(0,255,102,${p.alpha})`;
    ctx.beginPath();
    ctx.arc(cx,cy,p.r,0,Math.PI*2);
    ctx.stroke();
  });

  const rad=(angle-90)*Math.PI/180;
  ctx.strokeStyle="#00ff66";
  ctx.beginPath();
  ctx.moveTo(cx,cy);
  ctx.lineTo(cx+radius*Math.cos(rad),cy+radius*Math.sin(rad));
  ctx.stroke();

  echoes.forEach(e=>{
    const er=(e.d/100)*radius;
    const ea=(e.a-90)*Math.PI/180;
    ctx.fillStyle=`rgba(0,255,102,${e.alpha})`;
    ctx.beginPath();
    ctx.arc(cx+er*Math.cos(ea),cy+er*Math.sin(ea),5,0,Math.PI*2);
    ctx.fill();
    e.alpha-=0.01;
  });
  echoes=echoes.filter(e=>e.alpha>0);

  if(distance>0&&distance<100){
    const r=(distance/100)*radius;
    ctx.fillStyle=flashRed?"#ff3333":"#00ff66";
    ctx.beginPath();
    ctx.arc(cx+r*Math.cos(rad),cy+r*Math.sin(rad),8,0,Math.PI*2);
    ctx.fill();
  }

  drawLEDBar();
}

function toggleMode(){
  fetch("/control?mode="+(mode==="AUTO"?"MANUAL":"AUTO"));
}

setInterval(()=>{
  fetchData();
  updatePulses();
  drawRadar();
},100);
</script>
</body>
</html>
)rawliteral";

/* ===================== HARDWARE CODE (UNCHANGED) ===================== */

#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 13
int ledPins[]={16,17,19,21,22,23,25};
#define LED_COUNT (sizeof(ledPins)/sizeof(ledPins[0]))
#define JOY_X_PIN 34
#define JOY_BTN_PIN 14
#define BUZZER_PIN 4

#define MAX_DISTANCE 100
#define LOCK_DISTANCE 25
#define LOOP_INTERVAL 30
#define DEBOUNCE_TIME 250

enum Mode{AUTO,MANUAL};
Mode currentMode=AUTO;

bool targetLocked=false;
long lastDistance=-1;

Servo headServo;
float currentAngle=90,targetAngle=90,sweepStep=0.6;

unsigned long lastLoopTime=0,lastBtnTime=0,lastBeepTime=0;

long getDistanceCM(){
  digitalWrite(TRIG_PIN,LOW);delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);
  long d=pulseIn(ECHO_PIN,HIGH,25000);
  if(d==0)return -1;
  return d*0.034/2;
}

void updateProximityLEDs(long d){
  int n=map(d,MAX_DISTANCE,LOCK_DISTANCE,0,LED_COUNT+1);
  n=constrain(n,0,LED_COUNT);
  for(int i=0;i<LED_COUNT;i++) digitalWrite(ledPins[i],i<n);
}

void updateSound(long d){
  if(currentMode==MANUAL||d<=0||d>80){noTone(BUZZER_PIN);return;}
  if(d<=15){tone(BUZZER_PIN,2000);return;}
  int interval=map(d,80,15,1000,120);
  if(millis()-lastBeepTime>interval){
    tone(BUZZER_PIN,2000,50);
    lastBeepTime=millis();
  }
}

void handleRoot(){ server.send_P(200,"text/html",index_html); }

void handleData(){
  String j="{";
  j+="\"distance\":"; j+=lastDistance; j+=",";
  j+="\"angle\":"; j+=(int)currentAngle; j+=",";
  j+="\"mode\":\""; j+=(currentMode==AUTO?"AUTO":"MANUAL"); j+="\",";
  j+="\"locked\":"; j+=(targetLocked?"true":"false");
  j+="}";
  server.send(200,"application/json",j);
}

void handleControl(){
  if(server.hasArg("mode")){
    if(server.arg("mode")=="AUTO")currentMode=AUTO;
    if(server.arg("mode")=="MANUAL")currentMode=MANUAL;
  }
  server.send(200,"text/plain","OK");
}

void setup(){
  Serial.begin(115200);
  pinMode(TRIG_PIN,OUTPUT); pinMode(ECHO_PIN,INPUT);
  for(int i=0;i<LED_COUNT;i++) pinMode(ledPins[i],OUTPUT);
  pinMode(JOY_BTN_PIN,INPUT_PULLUP); pinMode(BUZZER_PIN,OUTPUT);

  headServo.setPeriodHertz(50);
  headServo.attach(SERVO_PIN,500,2400);

  WiFi.softAP(ssid,password);
  server.on("/",handleRoot);
  server.on("/data",handleData);
  server.on("/control",handleControl);
  server.begin();
}

void loop(){
  server.handleClient();
  if(millis()-lastLoopTime<LOOP_INTERVAL)return;
  lastLoopTime=millis();

  if(digitalRead(JOY_BTN_PIN)==LOW&&millis()-lastBtnTime>DEBOUNCE_TIME){
    currentMode=currentMode==AUTO?MANUAL:AUTO;
    lastBtnTime=millis();
  }

  long d=getDistanceCM();
  lastDistance=d;
  updateProximityLEDs(d<0?MAX_DISTANCE:d);

  if(currentMode==AUTO){
    if(!targetLocked){
      targetAngle+=sweepStep;
      if(targetAngle>=180||targetAngle<=0)sweepStep=-sweepStep;
      if(d>0&&d<=LOCK_DISTANCE)targetLocked=true;
    }else{
      if(d<0||d>LOCK_DISTANCE+10)targetLocked=false;
    }
  }else{
    targetAngle=map(analogRead(JOY_X_PIN),0,4095,0,180);
    targetLocked=false;
  }

  currentAngle+=(targetAngle-currentAngle)*0.15;
  headServo.write(currentAngle);
  updateSound(d);
}
