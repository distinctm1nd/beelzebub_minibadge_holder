#include <avr/io.h>
#include <util/delay.h>

#define EYE_LED_ARRAY 4      
#define NUM_EYE_LEDS 4         
#define WING_LED_ARRAY 5    
#define NUM_WING_LEDS 20        
#define BUTTON_PIN 1         
#define MINIBADGE_CLK_CTRL_H_PIN 3 
#define MINIBADGE_CLK_CTRL_L_PIN 2 

enum LedMode {
    LORD_OF_THE_FLIES, 
    NIGHT_OF_DARKNESS,   
    THE_SHADOWS,  
    MAYBE_TODAY_BEELZEBUB,    
    FLY_BEELZEBUB,   
    ANCIENT_GODS,
    FORCES_OF_DARKNESS,
    SEVEN_PRINCES, 
    BEELZEBUB_REVEALED,
    NIGHT_OF_DISCORD
};

LedMode currentMode = BEELZEBUB_REVEALED; // Set default mode

void lordOfTheFliesLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Red
            g = 0; 
            r = 200; 
            b = 0; 
            break;
        case 1: // Bright Red
            g = 0; 
            r = 255;
            b = 0; 
            break;
        case 2: // Light Brown
            g = 165; 
            r = 90; 
            b = 0; 
            break;
        case 3: // Dark Brown
            g = 75;  
            r = 40; 
            b = 0; 
            break;
        case 4: // Yellow
            g = 255; 
            r = 255; 
            b = 0; 
            break;
    }
}


void nightOfDarknessLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Red
            g = 0;
            r = 200;                        
            b = 0;             
            break;
        case 1: // Medium Red
            g = 0;
            r = 255;                        
            b = 0;             
            break;
        case 2: // Dark Red
            g = 0;
            r = 150;                        
            b = 0;             
            break;
        case 3: // Deep Red
            g = 0;
            r = 100;                        
            b = 0;             
            break;
        case 4: // Yellow
            g = 255; 
            r = 255;                     
            b = 0;             
            break;
    }
}


void theShadowsLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Purple
            g = 128;
            r = 0;    
            b = 255;          
            break;
        case 1: // Dark Purple
            g = 75;
            r = 0;    
            b = 150;          
            break;
        case 2: // Light Blue
            g = 0;
            r = 0;    
            b = 200;          
            break;
        case 3: // Dark Blue
            g = 0;
            r = 0;    
            b = 100;          
            break;
        case 4: // Red
            g = 0;
            r = 255;    
            b = 0;            
            break;
    }
}


void maybeTodayBeelzebubLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Green
            g = 200;
            r = 0;   
            b = 0;  
            break;
        case 1: // Medium Light Green
            g = 150; 
            r = 0;  
            b = 0;  
            break;
        case 2: // Medium Green
            g = 100; 
            r = 0;   
            b = 0;   
            break;
        case 3: // Dark Green
            g = 50;  
            r = 0;   
            b = 0;   
            break;
        case 4: // Yellow-Green
            g = 255;
            r = 255; 
            b = 0;  
            break;
    }
}


void flyBeelzebubLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Purple
            g = 128; 
            r = 64;  
            b = 128; 
            break;
        case 1: // Dark Purple
            g = 0;   
            r = 0;   
            b = 255; 
            break;
        case 2: // Light Blue
            g = 0;   
            r = 0;  
            b = 200; 
            break;
        case 3: // Dark Blue
            g = 0;   
            r = 0;   
            b = 100; 
            break;
        case 4: // White
            g = 255; 
            r = 255; 
            b = 255; 
            break;
    }
}


void ancientGodsLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Bright Red
            g = 0; 
            r = 255; 
            b = 0; 
            break;
        case 1: // Medium Red
            g = 0; 
            r = 200; 
            b = 0; 
            break;
        case 2: // Dark Red
            g = 0; 
            r = 150; 
            b = 0; 
            break;
        case 3: // Deeper Red
            g = 0; 
            r = 100;
            b = 0; 
            break;
        case 4: // Dark Brown
            g = 10;  
            r = 40;  
            b = 0;  
            break;
    }
}


void forcesOfDarknessLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 4) {
        case 0: // Light Orange
            g = 165; 
            r = 255; 
            b = 0; 
            break;  
        case 1: // Medium Orange
            g = 140; 
            r = 255; 
            b = 0; break;  
        case 2: // Dark Orange
            g = 100; 
            r = 200; 
            b = 0; break;  
        case 3: // White
            g = 255; 
            r = 255; 
            b = 255; 
            break; 
    }
}

void sevenPrincesLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Red
            g = 0; 
            r = 255; 
            b = 0; 
            break;
        case 1: // Yellow
            g = 255; 
            r = 255; 
            b = 0; 
            break;
        case 2: // Orange
            g = 165; 
            r = 255; 
            b = 0; 
            break;
        case 3: // Green
            g = 0; 
            r = 255; 
            b = 0; 
            break;
        case 4: // Purple
            g = 128; 
            r = 0; 
            b = 128; 
            break;
    }
}

void beelzebubRevealedLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Red
            g = 0; 
            r = 255; 
            b = 0; 
            break;
        case 1: // Green
            g = 255; 
            r = 0; 
            b = 0; 
            break;
        case 2: // Blue
            g = 0; 
            r = 0; 
            b = 255; 
            break;
        case 3: // Orange
            g = 165; 
            r = 255; 
            b = 0; 
            break;
        case 4: // Purple
            g = 128; 
            r = 0; 
            b = 128; 
            break;
    }
}

void nightOfDiscordLedMode(uint8_t index, uint8_t& g, uint8_t& r, uint8_t& b) {
    switch (index % 5) {
        case 0: // Light Blue
            g = 0; 
            r = 0; 
            b = 255; 
            break;
        case 1: // Medium Blue
            g = 0; 
            r = 0; 
            b = 200; 
            break;
        case 2: // Dark Blue
            g = 0; 
            r = 0; 
            b = 100; 
            break;
        case 3: // Deep Blue
            g = 0; 
            r = 0; 
            b = 50; 
            break;
        case 4: // White
            g = 255; 
            r = 255; 
            b = 255;
            break;
    }
}


void sendColor(uint8_t g, uint8_t r, uint8_t b, uint8_t ledPin) {
    sendByte(g, ledPin);
    sendByte(r, ledPin);
    sendByte(b, ledPin);
}

void sendByte(uint8_t byte, uint8_t ledPin) {
    for (int i = 0; i < 8; i++) {
        if (byte & (1 << (7 - i))) {
            PORTA.OUTSET = (1 << ledPin);
            _delay_us(0.4);
            PORTA.OUTCLR = (1 << ledPin);
            _delay_us(0.85); 
        } else {
            PORTA.OUTSET = (1 << ledPin);
            _delay_us(0.2); 
            PORTA.OUTCLR = (1 << ledPin);
            _delay_us(0.9); 
        }
    }
}

void colorWipe(uint8_t green, uint8_t red, uint8_t blue, uint8_t ledPin, uint8_t numLeds) {
    for (int i = 0; i < numLeds; i++) {
        sendColor(green, red, blue, ledPin); 
        _delay_us(500); 
    }
}

void setup() {
    // Set eye_led_array and letter_led_array as output
    PORTA.DIRSET = (1 << EYE_LED_ARRAY) | (1 << WING_LED_ARRAY);
    
    // Set button pin as input with pull-up resistor
    PORTA.DIRCLR = (1 << BUTTON_PIN);
    PORTA.OUTSET = (1 << BUTTON_PIN); 

    // Set minibadge clock pins as output
    PORTA.DIRSET = (1 << MINIBADGE_CLK_CTRL_H_PIN) | (1 << MINIBADGE_CLK_CTRL_L_PIN);
    
    // Ensure all LEDs are off at the start
    colorWipe(0, 0, 0, EYE_LED_ARRAY, NUM_EYE_LEDS);
    colorWipe(0, 0, 0, WING_LED_ARRAY, NUM_WING_LEDS);
    _delay_ms(100);
}

bool isButtonPressed() {
    return (PORTA.IN & (1 << BUTTON_PIN)) == 0;
}

void updateMinibadgeClock() {
    if (PORTA.IN & (1 << MINIBADGE_CLK_CTRL_H_PIN)) {
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_H_PIN);
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_L_PIN); 
    } else {
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_H_PIN); 
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_L_PIN); 
    }
}

void loop() {
    static uint32_t lastClockUpdate = 0;
    const uint32_t clockUpdateInterval = 1000; // 1 second
    static uint8_t ledIndex = 0;
    static uint32_t lastUpdateTime = 0;
    const uint32_t updateInterval = 200; 

    // Check if it's time to update the LED colors
    if (millis() - lastUpdateTime >= updateInterval) {
        lastUpdateTime = millis();

        uint8_t green, red, blue;

        // Update the color for the current LED based on the mode
        switch (currentMode) {
            case LORD_OF_THE_FLIES:
                lordOfTheFliesLedMode(ledIndex, green, red, blue);
                break;
            case NIGHT_OF_DARKNESS:
                nightOfDarknessLedMode(ledIndex, green, red, blue);
                break;
            case THE_SHADOWS:
                theShadowsLedMode(ledIndex, green, red, blue);
                break;
            case MAYBE_TODAY_BEELZEBUB:
                maybeTodayBeelzebubLedMode(ledIndex, green, red, blue);
                break;
            case FLY_BEELZEBUB:
                flyBeelzebubLedMode(ledIndex, green, red, blue);
                break;
            case ANCIENT_GODS:
                ancientGodsLedMode(ledIndex, green, red, blue);
                break;
            case FORCES_OF_DARKNESS: 
                forcesOfDarknessLedMode(ledIndex, green, red, blue);
                break;
            case SEVEN_PRINCES: 
                sevenPrincesLedMode(ledIndex, green, red, blue);
                break;
            case BEELZEBUB_REVEALED:
                beelzebubRevealedLedMode(ledIndex, green, red, blue);
                break;
            case NIGHT_OF_DISCORD: 
                nightOfDiscordLedMode(ledIndex, green, red, blue);
                break;
        }

        // Light up the current LED
        sendColor(green, red, blue, WING_LED_ARRAY); 
        _delay_us(500); 

        // Update eye LEDs to match the current LED color
        for (uint8_t i = 0; i < NUM_EYE_LEDS; i++) {
            sendColor(green, red, blue, EYE_LED_ARRAY); 
        }

        // Update all previous LEDs to maintain their color
        for (uint8_t i = 0; i < NUM_WING_LEDS; i++) {
            uint8_t prevGreen, prevRed, prevBlue;
            switch (currentMode) {
                case LORD_OF_THE_FLIES:
                    lordOfTheFliesLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case NIGHT_OF_DARKNESS:
                    nightOfDarknessLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case THE_SHADOWS:
                    theShadowsLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case MAYBE_TODAY_BEELZEBUB:
                    maybeTodayBeelzebubLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case FLY_BEELZEBUB:
                    flyBeelzebubLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case ANCIENT_GODS:
                    ancientGodsLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case FORCES_OF_DARKNESS: 
                    forcesOfDarknessLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case SEVEN_PRINCES: 
                    sevenPrincesLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case BEELZEBUB_REVEALED: 
                    beelzebubRevealedLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
                case NIGHT_OF_DISCORD: 
                    nightOfDiscordLedMode(((i + ledIndex) * 256 / NUM_WING_LEDS) % 256, prevGreen, prevRed, prevBlue);
                    break;
            }
            sendColor(prevGreen, prevRed, prevBlue, WING_LED_ARRAY); 
        }

        // Update LED index for the next iteration
        ledIndex = (ledIndex + 1) % NUM_WING_LEDS;
    }

    // Button logic for mode change
    if (isButtonPressed()) {
        currentMode = static_cast<LedMode>((currentMode + 1) % 10); 
        _delay_ms(300); 
    }

    // Re-check the minibadge clock update
    if (millis() - lastClockUpdate >= clockUpdateInterval) {
        updateMinibadgeClock();
        lastClockUpdate = millis();
    }
}
