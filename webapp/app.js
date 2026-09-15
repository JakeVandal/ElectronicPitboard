const connectBtn = document.getElementById('connectBtn');
const sendMessageBtn = document.getElementById('sendMessageBtn');
const actionButtons = document.querySelectorAll('.action-btn');
const quickButtons = document.querySelectorAll('.quick-btn');
const messageInput = document.getElementById('messageInput');

const modeValue = document.getElementById('modeValue');
const linkValue = document.getElementById('linkValue');
const wifiValue = document.getElementById('wifiValue');
const lapValue = document.getElementById('lapValue');
const positionValue = document.getElementById('positionValue');
const speedValue = document.getElementById('speedValue');
const rpmValue = document.getElementById('rpmValue');

let device = null;
let characteristic = null;

async function connectBle() {
  if (!navigator.bluetooth) {
    alert('Web Bluetooth is not supported in this browser.');
    return;
  }

  try {
    device = await navigator.bluetooth.requestDevice({
      filters: [{ services: ['0000FFB0-0000-1000-8000-00805F9B34FB'] }],
    });

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService('0000FFB0-0000-1000-8000-00805F9B34FB');
    characteristic = await service.getCharacteristic('0000FFB1-0000-1000-8000-00805F9B34FB');
    linkValue.textContent = 'Connected';
    modeValue.textContent = 'Manual';
    wifiValue.textContent = 'BLE';
  } catch (error) {
    console.error(error);
    linkValue.textContent = 'Failed';
  }
}

async function sendMessage() {
  if (!characteristic) {
    alert('BLE device not connected');
    return;
  }

  const payload = new TextEncoder().encode(messageInput.value.slice(0, 18));
  await characteristic.writeValue(payload);
}

function setMode(mode) {
  const normalized = mode.charAt(0).toUpperCase() + mode.slice(1);
  modeValue.textContent = normalized;
  if (mode === 'live') {
    wifiValue.textContent = 'Online';
  }
  if (mode === 'manual') {
    wifiValue.textContent = 'BLE';
  }
}

function applyQuickAction(action) {
  messageInput.value = action;
  sendMessage();
}

connectBtn.addEventListener('click', connectBle);
sendMessageBtn.addEventListener('click', sendMessage);

actionButtons.forEach((button) => {
  button.addEventListener('click', () => setMode(button.dataset.mode));
});

quickButtons.forEach((button) => {
  button.addEventListener('click', () => applyQuickAction(button.dataset.action));
});

lapValue.textContent = '8';
positionValue.textContent = '3';
speedValue.textContent = '42.7 mph';
rpmValue.textContent = '13200';
