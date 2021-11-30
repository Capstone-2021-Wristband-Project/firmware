// Utilities to interact with non-ESP32 components of the wristband.


// Given a scale 1-10.
// Power given to vibration motors should be adjusted to be 10-100% of the possible max power they can get.
// Called in ble.c whenever an integer is given from mobile app.
void adjustVibration(int scale);
