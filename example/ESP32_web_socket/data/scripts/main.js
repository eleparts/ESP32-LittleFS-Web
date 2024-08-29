var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onload);

function onload(event) {
  initWebSocket();
}

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log('Connection opened');
}

function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}

// 데이터 수신
function onMessage(event) {
  console.log('Receive Message: '+event.data);
  document.getElementById("state").innerHTML = event.data;
}

//----  LED 제어 버튼  ----// 
function LED_ON() {
  console.log("Send: ON");
  websocket.send('ON\0');
}

function LED_OFF() {
  console.log("Send: OFF");
  websocket.send('OFF\0');
}
