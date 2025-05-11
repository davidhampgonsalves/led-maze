export async function ensureDevicePermissions() {
  if (typeof DeviceOrientationEvent.requestPermission !== "function") return;

  await DeviceOrientationEvent.requestPermission().then((state) => {
    if (state === "granted") return;

    console.error("You must grant permission for sensor data to play");
  });
};