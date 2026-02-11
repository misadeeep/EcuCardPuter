#include <M5Cardputer.h>
#include <NimBLEDevice.h>
#include <SD.h>
#include <SPI.h>

// SD Card Pins for M5Cardputer (StampS3)
#define SD_SPI_SCK  40
#define SD_SPI_MISO 39
#define SD_SPI_MOSI 14
#define SD_SPI_CS   12

NimBLEScan* pBLEScan;
bool scanning = false;

// BLE Callbacks
class MyAdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        M5Cardputer.Display.printf("Found: %s \n", advertisedDevice->toString().c_str());
        // Logic to connect to ELM327 (usually named OBDII, V-LINK, etc.) would go here
        if (advertisedDevice->getName() == "OBDII") {
             M5Cardputer.Display.println(">> OBDII FOUND!");
             // Connect logic placeholder
        }
    }
};

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.fillScreen(BLACK);
    M5Cardputer.Display.println("M5Cardputer ELM327 & SD");
    M5Cardputer.Display.println("Press 'S' to Scan BLE");
    M5Cardputer.Display.println("Press 'W' to Write SD");

    // Init SD
    SPI.begin(SD_SPI_SCK, SD_SPI_MISO, SD_SPI_MOSI, SD_SPI_CS);
    if (!SD.begin(SD_SPI_CS, SPI, 25000000)) {
        M5Cardputer.Display.setTextColor(RED);
        M5Cardputer.Display.println("SD Init Failed!");
        M5Cardputer.Display.setTextColor(WHITE);
    } else {
        M5Cardputer.Display.setTextColor(GREEN);
        M5Cardputer.Display.println("SD Init OK");
        M5Cardputer.Display.setTextColor(WHITE);
    }

    // Init BLE (NimBLE)
    NimBLEDevice::init("M5Cardputer-Scanner");
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void loop() {
    M5Cardputer.update();
    
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            for (auto i : status.word) {
                // 'S' key for Scan
                if (i == 's' || i == 'S') {
                    if (!scanning) {
                        M5Cardputer.Display.println("Starting BLE Scan...");
                        scanning = true;
                        pBLEScan->start(5, false); // Scan for 5 seconds
                        scanning = false;
                        M5Cardputer.Display.println("Scan Done.");
                    }
                }
                // 'W' key for Write Test
                if (i == 'w' || i == 'W') {
                    File file = SD.open("/log.txt", FILE_APPEND);
                    if (file) {
                        file.println("Log Entry: Button Pressed");
                        file.close();
                        M5Cardputer.Display.println("Written to /log.txt");
                    } else {
                        M5Cardputer.Display.println("File Write Error");
                    }
                }
            }
        }
    }
    delay(10);
}
