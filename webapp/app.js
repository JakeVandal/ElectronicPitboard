const connectBtn = document.getElementById('connectBtn');
const sendMessageBtn = document.getElementById('sendMessageBtn');
const actionButtons = document.querySelectorAll('.action-btn');
const quickButtons = document.querySelectorAll('.quick-btn');
const messageInput = document.getElementById('messageInput');
const browserWarning = document.getElementById('browserWarning');
const warningTitle = document.getElementById('warningTitle');
const warningDetails = document.getElementById('warningDetails');
const activeRiderSelect = document.getElementById('activeRiderSelect');
const manualBikeInput = document.getElementById('manualBikeInput');
const applyRiderBtn = document.getElementById('applyRiderBtn');

const modeValue = document.getElementById('modeValue');
const linkValue = document.getElementById('linkValue');
const wifiValue = document.getElementById('wifiValue');
const lapValue = document.getElementById('lapValue');
const positionValue = document.getElementById('positionValue');
const speedValue = document.getElementById('speedValue');
const rpmValue = document.getElementById('rpmValue');

const serviceUuid = '0000FFB0-0000-1000-8000-00805F9B34FB';
const batteryUuid = '0000FFB5-0000-1000-8000-00805F9B34FB';

let device = null;
let characteristic = null;

function detectBrowserEnvironment() {
  const userAgent = navigator.userAgent || '';
  const platform = navigator.platform || '';
  const isIOS = /iPad|iPhone|iPod/.test(userAgent) || (platform === 'MacIntel' && navigator.maxTouchPoints > 1);
  const isBrave = !!window.navigator.brave || /Brave/.test(userAgent);
  const supportsBluetooth = 'bluetooth' in navigator;

  return { isIOS, isBrave, supportsBluetooth };
}

function showBrowserWarning(title, details) {
  warningTitle.textContent = title;
  warningDetails.innerHTML = details;
  browserWarning.classList.remove('hidden');
}

function hideBrowserWarning() {
  browserWarning.classList.add('hidden');
}

function updateBrowserWarnings() {
  const { isIOS, isBrave, supportsBluetooth } = detectBrowserEnvironment();

  if (!supportsBluetooth) {
    showBrowserWarning(
      'Web Bluetooth unavailable',
      'This browser does not expose <code>navigator.bluetooth</code>. Use Chrome/Edge on desktop or Bluefy/WebBLE on iOS.'
    );
    return;
  }

  if (isIOS) {
    showBrowserWarning(
      'iOS compatibility notice',
      'Safari on iOS does not support Web Bluetooth. Open this page in Bluefy or WebBLE, then reconnect.'
    );
    return;
  }

  if (isBrave) {
    showBrowserWarning(
      'Brave browser setup',
      'Go to <strong>brave://flags/#enable-web-bluetooth</strong>, enable it, disable Brave Shields for this URL, then relaunch Brave.'
    );
    return;
  }

  hideBrowserWarning();
}

function populateRoster(roster) {
  activeRiderSelect.innerHTML = '<option value="">Choose rider</option>';

  roster.forEach((rider) => {
    const option = document.createElement('option');
    option.value = String(rider.bikeNumber);
    option.textContent = `#${rider.bikeNumber} • ${rider.riderName}`;
    activeRiderSelect.appendChild(option);
  });
}

function setTrackedRider(bikeNumber) {
  const selected = Number(bikeNumber);
  if (!Number.isFinite(selected) || selected <= 0) {
    return;
  }

  manualBikeInput.value = String(selected);
  if (activeRiderSelect.value !== String(selected)) {
    activeRiderSelect.value = String(selected);
  }
}

async function connectBle() {
  const { supportsBluetooth, isIOS, isBrave } = detectBrowserEnvironment();
  if (!supportsBluetooth) {
    showBrowserWarning('Web Bluetooth unavailable', 'This browser does not expose <code>navigator.bluetooth</code>. Use Chrome/Edge on desktop or Bluefy/WebBLE on iOS.');
    return;
  }

  if (isIOS) {
    showBrowserWarning('iOS compatibility notice', 'Safari on iOS does not support Web Bluetooth. Open this page in Bluefy or WebBLE, then reconnect.');
    return;
  }

  if (isBrave) {
    showBrowserWarning('Brave browser setup', 'Go to <strong>brave://flags/#enable-web-bluetooth</strong>, enable it, disable Brave Shields for this URL, then relaunch Brave.');
  }

  try {
    device = await navigator.bluetooth.requestDevice({
      filters: [{ services: [serviceUuid] }],
    });

    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(serviceUuid);
    characteristic = await service.getCharacteristic('0000FFB1-0000-1000-8000-00805F9B34FB');
    linkValue.textContent = 'Connected';
    modeValue.textContent = 'Manual';
    wifiValue.textContent = 'BLE';
  } catch (error) {
    console.error(error);
    linkValue.textContent = 'Failed';

    const name = error && (error.name || '');
    const message = error && (error.message || '');
    if (name === 'SecurityError' || name === 'NotAllowedError' || /blocked|denied|permission/i.test(message)) {
      showBrowserWarning(
        'Permission blocked',
        'Bluetooth access was denied or blocked. Enable Web Bluetooth in Brave, disable Shields, and retry.'
      );
    } else {
      showBrowserWarning(
        'Connection failed',
        'The device was not available or permission was denied. Try again or use a supported browser.'
      );
    }
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

const mockRoster = [
  { bikeNumber: 21, riderName: 'Alex Masuk' },
  { bikeNumber: 8, riderName: 'Rory McLaren' },
  { bikeNumber: 44, riderName: 'Jamie Ross' },
  { bikeNumber: 77, riderName: 'Mia Carson' },
];

applyRiderBtn.addEventListener('click', () => {
  const bikeNumber = manualBikeInput.value.trim();
  if (!bikeNumber) {
    return;
  }
  setTrackedRider(bikeNumber);
});

activeRiderSelect.addEventListener('change', (event) => {
  const value = event.target.value;
  if (value) {
    setTrackedRider(value);
  }
});

connectBtn.addEventListener('click', connectBle);
sendMessageBtn.addEventListener('click', sendMessage);

actionButtons.forEach((button) => {
  button.addEventListener('click', () => setMode(button.dataset.mode));
});

quickButtons.forEach((button) => {
  button.addEventListener('click', () => applyQuickAction(button.dataset.action));
});

updateBrowserWarnings();
populateRoster(mockRoster);
setTrackedRider(21);

lapValue.textContent = '8';
positionValue.textContent = '3';
speedValue.textContent = '42.7 mph';
rpmValue.textContent = '13200';
