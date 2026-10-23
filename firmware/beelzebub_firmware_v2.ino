#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>   // ATtiny814 has 128 bytes of EEPROM, 8 KB of Flash

// ===========================================================================
//  Beelzebub badge LED controller 
//
//  LED blocks - each has its own colour palette and its own animation:
//    EYES         - the 4 eye LEDs, two per eye   (EYE_LED_ARRAY)
//    THORAX       - the 2 LEDs by the head  (WING_LED_ARRAY, chain LEDs 1-2)
//    LEFT WING    - the 10 LEDs of the big wing    (WING_LED_ARRAY)
//    RIGHT WING   - the 8 LEDs of the second wing  (WING_LED_ARRAY)
//    (the 20 wing-line LEDs are one chain; the blocks are scattered along it,
//     so the animations know where each LED really sits on the badge)
//
//  SCENES - ready-made looks you cycle through with one short press:
//     1 Beelzebub Revealed   2 Lord of the Flies    3 Night of Darkness
//     4 The Shadows          5 Maybe Today, Beelzebub  6 Fly, Beelzebub
//     7 Ancient Gods         8 Forces of Darkness   9 Seven Princes
//    10 Night of Discord    11 The Hive            12 Plague of Flies
//    13 Brimstone           14 Compound Eyes       15 Carrion Feast
//   SLOW & DREAMY scenes (smooth colour fades, nothing snaps):
//    16 Dreaming Hive       17 Stagnant Swamp      18 Sunken Fire
//    19 Wing Shimmer        20 Night Flight        21 Fever Dream
//    22 The Watcher (bright eyes, dim glowing wings)
//    23 Crimson Stare (bright red eyes, dim red wings)
//    24 Beelzebub Awakens   <- a new or reset badge starts here
//    25 Rotting Sunset
//   SPECIAL-EFFECT scenes:
//    26 Wingbeat            27 The Swarm           28 Plague Storm
//    29 The Twin Princes    30 Pulse of the Hive   31 Sweeping Wings
//    32 Beelzebub Stirs     33 Midnight Radar
//    34 MY CUSTOM           <- your own look from the menu (never overwritten
//                              by the other scenes)
//    The scene number shows as LEDs on the wings. Above 20, ALL 20 LEDs light
//    up: the lit (white / yellow) ones count how far past 20 you are, the rest
//    are blood red. The same display is used for palette / animation numbers in the menu.
//
//  EXTRA EFFECTS - some scenes also get these on top of their animations (fixed per scene, table SCENE_LAYER):
//    TWINKLE = random LEDs flare up like sparks
//    BURSTS  = now and then (every ~20 s) the animation races at up to twice its speed for ~2 s
//    SWELL   = the animation speeds up and slows down every ~5 s
//    Every scene has its own base tempo (table SCENE_SPEED): the creepy scenes drift, the angry ones are brisk.
//    LIGHTNING: scenes 4 and 28 (left wing) strike when picked, then once every 2-4 minutes; the 5-tap "Hellstorm" egg strikes often.
//
//  BUTTON:
//    Run mode : short press = next scene (a row of LEDs shows the number: white,
//                 or YELLOW if the scene is a favourite)
//               while AUTO-CYCLING: ONE short press STAYS on the scene you are
//                 looking at and stops the cycling (3 taps + hold also steps it to off)
//               hold (0.8 s) = open the THEME MENU
//             SHORTCUTS - tap quickly, then hold on the last tap:
//               SCENE LIST
//                 2 taps + hold = mark / unmark this scene as a FAVOURITE
//                                 (green blink = added, red blink = removed)
//                 3 taps + hold = AUTO-CYCLING through your favourites, one step each time:
//                                 off -> 4 s -> 8 s -> 20 s -> 60 s -> off
//                                 (1-4 green blinks = the step, 2 red blinks = off;
//                                 with no favourites marked it cycles every scene;
//                                 to take a scene out of the cycle, unmark it: 2 taps + hold)
//               HIDDEN EXTRAS
//                 4 taps + hold = MORSE message on the eyes
//                                 ("LORD OF THE FLIES"); any press stops it
//                 5 taps        = "Hellstorm" show WITH lightning (12 s)
//               MENUS
//                 hold          = THEME MENU (palettes and animations of MY CUSTOM)
//                 1 tap + hold  = SETTINGS MENU (brightness, eye brightness, speed)
//    In a menu: short press = change the value on the current page
//               hold        = go to the next page (after the last page the
//                             menu closes and your choices are SAVED)
//               no press for 15 s = menu closes and saves
//    POWER-UP : hold the button while plugging the badge in:
//                 eyes turn RED after 8 s  -> let go now = RESET everything to default
//                 Let go before 8 s = normal start.
//    RESET    : the power-up hold above is the only way (it clears favourites too).
//
//  THEME MENU (hold): a row of BLUE wing LEDs shows the page number on entry, every 5th
//  LED magenta to make it easy to count. The block you are editing stays bright and the
//  others dim: PALETTE pages show the palette itself as a steady swatch (one colour per
//  LED), ANIMATION pages play the animation live.
//     1  Eyes palette              2  Eyes animation
//     3  Thorax palette            4  Thorax animation
//     5  Left wing palette         6  Left wing animation
//     7  Right wing palette        8  Right wing animation
//  Changing anything here copies the scene you are looking at into MY CUSTOM (the last
//  scene) and edits it there. After page 8 the menu closes and SAVES.
//
//  COLOR OPTIONS (palette pages 1, 3, 5, 7 - each press moves to the next, in this order):
//     1  Lord of the Flies        Gangrene: dead-flesh greens, bruise purple, black
//     2  Night of Darkness        Blood: bright to deep red, a hot red flash
//     3  The Shadows              Cursed: purples fading to dark crimson
//     4  Maybe Today, Beelzebub   Toxic: acid greens, then red, with black between
//     5  Fly, Beelzebub           Midnight: deep blues fading to dark crimson
//     6  Ancient Gods             Ossuary: old bone, dark bone, dried blood, black
//     7  Forces of Darkness       Hellfire: furnace oranges and reds
//     8  Seven Princes            Hell's court: red, orange, black, sick green, dark red
//     9  Beelzebub Revealed       Unholy: violet and blood red, black between
//    10  Night of Discord         Ghost: cold spectral blue-violet, black
//    11  The Hive                 Venom: acid yellow-green and violet, black between
//    12  Plague                   Plague: sickly yellow-greens and rot black
//    13  Brimstone                Brimstone: red, fire orange, sulphur yellow
//    14  Compound Eye             Evil eye: ember, red, amber, dark red, gold
//    15  Iridescent               Witchfire: violet and ghostly green flames
//    16  Carrion                  Carrion: blood red with black gaps, wine
//
//  ANIMATION OPTIONS (animation pages 2, 4, 6, 8 - same order, then back to 1):
//     1  Drift          the scrolling palette
//     2  Sparkle        a dim glow with bright sparks popping at random
//     3  Storm          a dim glow with lightning (a strike every few minutes)
//     4  Ember          smooth random flicker, like glowing embers
//     5  Crossfade      the block slowly melts from one palette colour into the next
//     6  Currents       a smooth gradient of palette colours slowly flows along
//     7  Gaze           slowly brightens, holds, slowly dims (never fully off; the eyes take turns)
//     8  Ripple         soft rings spread out from the base of the wings
//     9  Dim            a dim, slowly breathing glow (keeps the eyes standing out)
//    10  Radar          a bright beam sweeps back and forth round the base of the wings
//    11  Wingbeat       the whole wing flaps: light pulses out from the base to the tips
//    12  Web            a spider web: rings pulse out along glowing spokes
//    13  Whirl          two bright arms spin round the wing base
//    14  Dying          LEDs sputter out and come back, like failing lights
//    15  Spiral         a spiral of light winds out from the base of the wings
//    16  Chase          running lights: every third LED lit, crawling along the wing
//    17  Throb          the whole block throbs together, like a heart
//    18  Comet          a bright comet with a fading tail runs along the LEDs
//    19  Rise           embers rise up the badge from the abdomen
//    20  Candle         a flickering candle: each LED gutters at its own pace
//    21  Flare          the colour stays lit and two slow (about 1 s) white flashes with a pause between
//  The eyes always melt smoothly from colour to colour and never go dark. Animations 5-8 also blend smoothly between the palette colours.
//  The extra effects (twinkle, bursts, swell) are not menu choices: MY CUSTOM
//  keeps those of the scene you started from.
//
//  SETTINGS MENU (1 tap + hold): these apply to ALL scenes, not just MY CUSTOM. A row of
//  ORANGE LEDs shows the page number; the value shows as a row of WHITE LEDs (1 = first):
//     1  Brightness, all LEDs (1 = brightest ... 4 = dimmest)
//     2  Eye brightness, relative to the rest (1 = same ... 4 = dimmest)
//     3  Speed      (1 = slow, 2 = normal, 3 = fast)
//  (Favourites and auto-cycling are set with the tap shortcuts, not in a menu.)
// ===========================================================================

#define EYE_LED_ARRAY 4
#define NUM_EYE_LEDS 4
#define WING_LED_ARRAY 5
#define NUM_WING_LEDS 20
#define NUM_SLOTS (NUM_WING_LEDS + NUM_EYE_LEDS)   // slots 0-19 = wing chain, 20-23 = eyes
#define BUTTON_PIN 1
#define MINIBADGE_CLK_CTRL_H_PIN 3
#define MINIBADGE_CLK_CTRL_L_PIN 2

#define FRAME_MS 40                // animation frame time (25 frames per second)
#define DEBOUNCE_MS 30
#define LONG_PRESS_MS 800
#define MENU_TIMEOUT_MS 15000UL
#define MENU_DIM_SHIFT 3           // non-selected blocks are dimmed to 1/8 in the menu
#define NUM_CYCLE_LEVELS 4         // auto-cycle time steps (see AUTO_CYCLE_TABLE)
#define MORSE_UNIT_MS 160          // length of one Morse "dot"
#define TAP_GAP_MS 800UL           // tap-tap-hold shortcuts: taps (and the final hold) must follow each other this fast
// LED current limit: every frame is scaled down if the LEDs would draw more than
// this. A WS2812 colour channel at full draws about 20 mA (all 24 LEDs white = 1.4 A!).
#define MAX_LED_CURRENT_MA 350
#define PAGE_OVERLAY_MS 1000UL     // how long the page-number LEDs stay lit
#define VALUE_OVERLAY_MS 800UL     // how long the value LEDs stay lit
#define EGG_PRESSES 5              // quick presses needed for the easter egg...
#define EGG_WINDOW_MS 2500UL       // ...within this time
#define EGG_DURATION_MS 12000UL    // how long the hidden show runs
#define SCENE_OVERLAY_MS 500UL     // how long the scene-number LEDs stay lit
#define RESET_HOLD_MS 8000         // hold the button this long at power-up to reset

// Scenes: 0-32 use the palette/animation table below, 33 is your own custom look.
// (NUM_SCENES must stay at 34 or less: the scene number is shown on the 20 wing LEDs.)
#define NUM_PRESETS 33
#define SCENE_DEFAULT 23     // Beelzebub Awakens
#define SCENE_CUSTOM 33
#define NUM_SCENES 34
#define FAV_BYTES 5                 // one bit per scene
#if NUM_SCENES > FAV_BYTES * 8
#error "FAV_BYTES too small for NUM_SCENES"
#endif
#if NUM_SCENES > 40
#error "Too many scenes to show the scene number on 20 LEDs"
#endif

// --- saved settings (EEPROM) -----------------------------------------------
#define EEPROM_ADDR 0
#define EEPROM_MAGIC 0xB3          // marks saved data as valid (changed with the layout)

enum {
    LORD_OF_THE_FLIES,         // Gangrene: dead-flesh greens, bruise purple, black
    NIGHT_OF_DARKNESS,         // Blood: bright to deep red, a hot red flash
    THE_SHADOWS,               // Cursed: purples fading to dark crimson
    MAYBE_TODAY_BEELZEBUB,     // Toxic: acid greens, then red, with black between
    FLY_BEELZEBUB,             // Midnight: deep blues fading to dark crimson
    ANCIENT_GODS,              // Ossuary: old bone, dark bone, dried blood, black
    FORCES_OF_DARKNESS,        // Hellfire: furnace oranges and reds
    SEVEN_PRINCES,             // Hell's court: red, orange, black, sick green, dark red
    BEELZEBUB_REVEALED,        // Unholy: violet and blood red, black between
    NIGHT_OF_DISCORD,          // Ghost: cold spectral blue-violet, black
    THE_HIVE,                  // Venom: acid yellow-green and violet, black between
    PLAGUE,                    // Plague: sickly yellow-greens and rot black
    BRIMSTONE,                 // Brimstone: red, fire orange, sulphur yellow
    COMPOUND_EYE,              // Evil eye: ember, red, amber, dark red, gold
    IRIDESCENT,                // Witchfire: violet and ghostly green flames
    CARRION,                   // Carrion: blood red with black gaps, wine
    NUM_LED_MODES
};
typedef uint8_t LedMode;

// Animations - how a block moves. Any block can use any animation.
enum {
    FX_DRIFT,           // 1: the scrolling palette
    FX_SPARKLE,         // 2: dim glow with bright sparks popping at random
    FX_STORM,           // 3: dim glow with random lightning flashes (a strike every few minutes)
    FX_EMBER,           // 4: smooth random flicker, like glowing embers
    // --- SLOW effects: these also blend smoothly BETWEEN the palette colours ---
    FX_CROSSFADE,       // 5: the whole block slowly melts from one palette colour into the next
    FX_CURRENTS,        // 6: a smooth gradient of palette colours slowly flows along the LEDs
    FX_GAZE,            // 7: slowly opens to full glow, holds, slowly closes (eyes: one after the other)
    FX_RIPPLE,          // 8: soft rings of light spread outward from the base of the wings
    // --- special effects (these use the normal scrolling palette) ---
    FX_DIM,             // 9: a dim, slowly breathing glow (so the eyes stand out)
    FX_RADAR,           // 10: a bright beam sweeps round the base of the wings
    FX_WINGBEAT,        // 12: the wing flaps: light pulses out from the base to the tips
    FX_WEB,        // 13: a spider web: rings pulse out along glowing spokes
    FX_WHIRL,        // 15: two bright arms spin round the wing base
    FX_DYING,        // 16: LEDs sputter out and come back, like failing lights
    FX_SPIRAL,   // 18: a spiral of light winds out from the base of the wings
    FX_CHASE,           // running lights: every third LED lit, crawling along the wing
    FX_THROB,           // the whole block throbs together, like a heart
    FX_COMET,           // a bright comet with a fading tail runs along the LEDs
    FX_RISE,   // 19: embers rise up the badge from the abdomen
    FX_CANDLE,   // 21: a flickering candle: each LED gutters at its own pace
    FX_FLARE,   // 22: the colour stays lit with two slow white flares
    NUM_EFFECTS
};
typedef uint8_t EffectMode;

#define FIRST_SMOOTH_FX FX_CROSSFADE   // effects FIRST_SMOOTH_FX..LAST_SMOOTH_FX blend smoothly between palette colours
#define LAST_SMOOTH_FX FX_RIPPLE

enum {
    BLOCK_EYES,
    BLOCK_THORAX,
    BLOCK_LEFT_WING,
    BLOCK_RIGHT_WING,
    NUM_LED_BLOCKS
};
typedef uint8_t LedBlock;

// Pages 0-7 map onto blocks: page/2 = block, even = palette, odd = animation.
enum MenuPage {
    PAGE_EYES_COLOR,  PAGE_EYES_EFFECT,
    PAGE_THX_COLOR,   PAGE_THX_EFFECT,
    PAGE_LW_COLOR,    PAGE_LW_EFFECT,
    PAGE_RW_COLOR,    PAGE_RW_EFFECT,
    PAGE_BRIGHTNESS, PAGE_EYE_BRIGHTNESS, PAGE_SPEED,
    NUM_PAGES
};

enum { UI_RUN, UI_MENU };
typedef uint8_t UiState;

#define NUM_BRIGHTNESS_LEVELS 4
#define NUM_SPEED_LEVELS 3
const uint8_t BRIGHTNESS_TABLE[NUM_BRIGHTNESS_LEVELS] = { 255, 150, 80, 35 };
const uint8_t SPEED_INC[NUM_SPEED_LEVELS] = { 1, 2, 4 };   // 2 = the same speed as the Cthulhu badge
const uint32_t AUTO_CYCLE_TABLE[NUM_CYCLE_LEVELS] = { 4000UL, 8000UL, 20000UL, 60000UL };   // ms per scene

// Hidden message, sent in Morse on the eyes. . = dot, - = dash, space = letter gap, / = word gap.
const char MORSE_MSG[] = ".-.. --- .-. -../--- ..-./- .... ./..-. .-.. .. . ...";   // LORD OF THE FLIES

// ---- Where each LED really sits -------------------------------------------
// The 20 wing-line LEDs are one chain (first one at the thorax, last at the tip of the
// right wing) but they belong to different blocks, in this order:
const uint8_t LED_BLOCK[NUM_WING_LEDS] = {
    BLOCK_THORAX, BLOCK_THORAX, BLOCK_RIGHT_WING, BLOCK_LEFT_WING, BLOCK_LEFT_WING,
    BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_RIGHT_WING,
    BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_LEFT_WING, BLOCK_RIGHT_WING,
    BLOCK_RIGHT_WING, BLOCK_RIGHT_WING, BLOCK_RIGHT_WING, BLOCK_RIGHT_WING, BLOCK_RIGHT_WING };
// ...and each LED's number inside its own block:
const uint8_t LED_LOCAL[NUM_WING_LEDS] = { 0, 1, 0, 0, 1, 2, 3, 4, 5, 1, 6, 7, 8, 9, 2, 3, 4, 5, 6, 7 };
// Picture of the badge, measured from the PCB (one entry per slot: 20 wing LEDs, then the 4 eyes):
//   LED_Y: 0 = top ... 255 = bottom
//   LED_R: distance from where the wings join the body          LED_A: angle round that point
const uint8_t LED_Y[NUM_SLOTS] = { 169,185,211,225,243,166,158,97,68,150,122,88,53,6,80,46,140,100,54,23, 210,171,197,177 };
const uint8_t LED_R[NUM_SLOTS] = { 90,65,51,76,130,2,63,147,183,42,49,96,148,223,104,136,80,101,147,179, 186,182,233,225 };
const uint8_t LED_A[NUM_SLOTS] = { 128,139,173,219,229,64,7,22,25,109,64,44,39,36,82,77,112,96,88,85, 138,129,133,130 };
// Number of LEDs in each block, and how the palette is spread over a block's LEDs.
const uint8_t BLOCK_SPAN[NUM_LED_BLOCKS] = { 4, 2, 10, 8 };

// ---- Scene table ----------------------------------------------------------
// One row per scene: palette and animation for { eyes, thorax, left wing, right wing }.
const uint8_t SCENE_MODE[NUM_PRESETS][NUM_LED_BLOCKS] = {
    { BEELZEBUB_REVEALED, BEELZEBUB_REVEALED, BEELZEBUB_REVEALED, BEELZEBUB_REVEALED },  //  1 Beelzebub Revealed
    { LORD_OF_THE_FLIES, LORD_OF_THE_FLIES, LORD_OF_THE_FLIES, LORD_OF_THE_FLIES },  //  2 Lord of the Flies
    { NIGHT_OF_DARKNESS,     NIGHT_OF_DARKNESS,     NIGHT_OF_DARKNESS,     NIGHT_OF_DARKNESS },      //  3 Night of Darkness
    { THE_SHADOWS,           THE_SHADOWS,           THE_SHADOWS,           THE_SHADOWS },            //  4 The Shadows
    { MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB },  //  5 Maybe Today, Beelzebub
    { FLY_BEELZEBUB,         FLY_BEELZEBUB,         FLY_BEELZEBUB,         FLY_BEELZEBUB },          //  6 Fly, Beelzebub
    { ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS },  //  7 Ancient Gods
    { FORCES_OF_DARKNESS,    FORCES_OF_DARKNESS,    FORCES_OF_DARKNESS,    FORCES_OF_DARKNESS },     //  8 Forces of Darkness
    { SEVEN_PRINCES,         SEVEN_PRINCES,         SEVEN_PRINCES,         SEVEN_PRINCES },          //  9 Seven Princes
    { NIGHT_OF_DISCORD, NIGHT_OF_DISCORD, NIGHT_OF_DISCORD, NIGHT_OF_DISCORD },  // 10 Night of Discord
    { THE_HIVE, THE_HIVE, THE_HIVE, THE_HIVE },  // 11 The Hive
    { PLAGUE,                PLAGUE,                PLAGUE,                PLAGUE },                 // 12 Plague of Flies
    { BRIMSTONE,             BRIMSTONE,             BRIMSTONE,             BRIMSTONE },              // 13 Brimstone
    { COMPOUND_EYE, BRIMSTONE, BRIMSTONE, BRIMSTONE },  // 14 Compound Eyes (ember eyes, honey wings)
    { CARRION,               CARRION,               CARRION,               CARRION },                // 15 Carrion Feast
    { THE_HIVE, THE_HIVE, THE_HIVE, THE_HIVE },  // 16 Dreaming Hive (slow)
    { MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB, MAYBE_TODAY_BEELZEBUB },  // 17 Stagnant Swamp (slow)
    { BRIMSTONE,             BRIMSTONE,             BRIMSTONE,             BRIMSTONE },              // 18 Sunken Fire (slow)
    { IRIDESCENT,            IRIDESCENT,            IRIDESCENT,            IRIDESCENT },             // 19 Wing Shimmer (slow)
    { THE_SHADOWS,           FLY_BEELZEBUB,         FLY_BEELZEBUB,         FLY_BEELZEBUB },          // 20 Night Flight (slow)
    { SEVEN_PRINCES,         SEVEN_PRINCES,         SEVEN_PRINCES,         SEVEN_PRINCES },          // 21 Fever Dream (slow)
    { COMPOUND_EYE, THE_SHADOWS, THE_SHADOWS, THE_SHADOWS },  // 22 The Watcher (ember eyes, dim purple wings)
    { NIGHT_OF_DARKNESS, NIGHT_OF_DARKNESS, NIGHT_OF_DARKNESS, NIGHT_OF_DARKNESS },  // 23 Crimson Stare (blood red eyes and dim wings)
    { COMPOUND_EYE, ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS },  // 24 Beelzebub Awakens (ember eyes, bone wings)
    { LORD_OF_THE_FLIES, LORD_OF_THE_FLIES, LORD_OF_THE_FLIES, LORD_OF_THE_FLIES },  // 25 Rotting Sunset (gangrene)
    { NIGHT_OF_DISCORD, NIGHT_OF_DISCORD, NIGHT_OF_DISCORD, NIGHT_OF_DISCORD },  // 26 Wingbeat
    { THE_HIVE, THE_HIVE, THE_HIVE, THE_HIVE },  // 27 The Swarm (venom)
    { PLAGUE,                PLAGUE,                PLAGUE,                PLAGUE },                 // 28 Plague Storm
    { FORCES_OF_DARKNESS, FORCES_OF_DARKNESS, FORCES_OF_DARKNESS, NIGHT_OF_DISCORD },  // 29 The Twin Princes (fire and ice)
    { IRIDESCENT, IRIDESCENT, IRIDESCENT, IRIDESCENT },  // 30 Pulse of the Hive (witchfire)
    { ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS },  // 31 Sweeping Wings (bone)
    { THE_SHADOWS, THE_SHADOWS, THE_SHADOWS, THE_SHADOWS },  // 32 Beelzebub Stirs (cursed purple)
    { FLY_BEELZEBUB, FLY_BEELZEBUB, FLY_BEELZEBUB, FLY_BEELZEBUB },  // 33 Midnight Radar
};
// Tempo of each scene (16 = a brisk walk): the dreamy scenes crawl, the angry ones race.
// Tempo of each scene in sixteenths of the Cthulhu speed (16 = the same as Cthulhu): the creepy scenes drift, the angry ones are brisk.
const uint8_t SCENE_SPEED[NUM_SCENES] = {
    14, 18,  9, 16, 12, 18,  9, 20, 16, 20,   //  1-10
    14, 20, 11, 10,  9, 10, 11,  9, 12, 14,   // 11-20
    10, 12, 12, 11, 10, 16, 20, 14, 16, 12,   // 21-30
    16, 10, 16, 14 };                         // 31-33, MY CUSTOM                                                                     // 31-33, MY CUSTOM                                                                     // 31-33, MY CUSTOM                                                                     // 31-33, MY CUSTOM

const uint8_t SCENE_EFFECT[NUM_PRESETS][NUM_LED_BLOCKS] = {
    { FX_FLARE, FX_THROB, FX_COMET, FX_WHIRL },  //  1 beating eyes, a web and a spinning wing
    { FX_THROB, FX_EMBER, FX_WHIRL, FX_SPARKLE },  //  2 buzzing flies, dying lights
    { FX_FLARE, FX_THROB, FX_SPARKLE, FX_WHIRL },  //  3 darkness creeping through the wings
    { FX_FLARE, FX_EMBER, FX_STORM, FX_CHASE },  //  4 sparks and marching lights
    { FX_CROSSFADE, FX_RIPPLE, FX_SPIRAL, FX_COMET },  //  5 poison dripping down and rising
    { FX_THROB, FX_DRIFT, FX_WINGBEAT, FX_CHASE },  //  6 a flapping wing and a spinning one
    { FX_FLARE, FX_CHASE, FX_COMET, FX_WHIRL },  //  7 old web, whips of light
    { FX_RADAR, FX_RADAR, FX_WHIRL, FX_RADAR },  //  8 hellfire radar and spinning arms
    { FX_FLARE, FX_RADAR, FX_CHASE, FX_WINGBEAT },  //  9 spinning arms and surf
    { FX_WHIRL, FX_CANDLE, FX_DYING, FX_CHASE },  // 10 glitching, dying lights
    { FX_FLARE, FX_CANDLE, FX_CHASE, FX_SPIRAL },  // 11 a humming hive, spirals
    { FX_WHIRL, FX_EMBER, FX_THROB, FX_COMET },  // 12 flies everywhere
    { FX_FLARE, FX_THROB, FX_WINGBEAT, FX_SPARKLE },  // 13 dripping, spiralling fire
    { FX_GAZE, FX_GAZE, FX_WEB, FX_COMET },  // 14 watching eyes, webs and stripes
    { FX_FLARE, FX_THROB, FX_WHIRL, FX_COMET },  // 15 creeping rot, whips of light
    { FX_FLARE, FX_CROSSFADE, FX_CHASE, FX_COMET },  // 16 slow: eyes open, candle wing, honey flows
    { FX_COMET, FX_CURRENTS, FX_WINGBEAT, FX_CHASE },  // 17 slow swamp: rising bubbles
    { FX_GAZE, FX_CROSSFADE, FX_SPARKLE, FX_DYING },  // 18 slow fire: guttering candle
    { FX_FLARE, FX_CURRENTS, FX_SPIRAL, FX_COMET },  // 19 slow spiral shimmer
    { FX_RADAR, FX_CROSSFADE, FX_RADAR, FX_RISE },  // 20 night flight: rising embers
    { FX_CROSSFADE, FX_THROB, FX_WHIRL, FX_CHASE },  // 21 slow spiral, melting colours
    { FX_FLARE, FX_DIM, FX_DIM, FX_DIM },  // 22 bright watching eyes, dim glowing wings
    { FX_DRIFT, FX_DIM, FX_DIM, FX_DIM },  // 23 bright steady eyes, dim glowing wings
    { FX_FLARE, FX_THROB, FX_COMET, FX_SPARKLE },  // 24 slow: eyes open, honey flows, a candle wing
    { FX_FLARE, FX_THROB, FX_WHIRL, FX_COMET },  // 25 slow sunset: rising embers
    { FX_THROB, FX_RIPPLE, FX_WINGBEAT, FX_WINGBEAT },  // 26 wings flapping
    { FX_WHIRL, FX_CANDLE, FX_CHASE, FX_SPARKLE },  // 27 the swarm
    { FX_FLARE, FX_EMBER, FX_STORM, FX_DYING },  // 28 sparks and dying lights
    { FX_COMET, FX_RADAR, FX_WHIRL, FX_WEB },  // 29 beam, spinning arms and dripping blood
    { FX_FLARE, FX_RIPPLE, FX_THROB, FX_WEB },  // 30 a beating heart, spiral and web
    { FX_EMBER, FX_RISE, FX_SPIRAL, FX_WHIRL },  // 31 bands of light sweep one wing, the other spins
    { FX_FLARE, FX_CANDLE, FX_WHIRL, FX_CHASE },  // 32 creeping dark, whips of light
    { FX_RADAR, FX_RADAR, FX_COMET, FX_WHIRL },  // 33 a radar beam and a spinning arm
};

// Ambient layers: extra life that every scene gets on top of its animations.
//   TWINKLE = random LEDs flare up like sparks, FADE = a slow wave of dimming and brightening
//   rolls through the LEDs, SURGE = the animation speeds up and slows down over ~20 s.
#define LAYER_TWINKLE 1
#define LAYER_FADE 2
#define LAYER_SURGE 4
#define LAYER_FLUSH 8      // the scene's speed swells and ebbs, as in the Cthulhu badge
const uint8_t SCENE_LAYER[NUM_PRESETS] = {      // 1 twinkles, 4 short bursts of speed now and then, 8 the speed swells and ebbs
    4, 13, 0, 13, 0, 4, 0, 12, 0, 13,         //  1-10
    5, 13, 0, 0, 0, 1, 0, 5, 1, 13,           // 11-20
    0, 1, 1, 4, 0, 4, 13, 13, 12, 0,          // 21-30
    4, 0, 12 };                               // 31-33                               // 31-33                                // 31-33



// ---- Settings (saved) -----------------------------------------------------
// blockMode/blockEffect are the LIVE look (copied from the current scene).
LedMode blockMode[NUM_LED_BLOCKS] = { COMPOUND_EYE, ANCIENT_GODS, ANCIENT_GODS, ANCIENT_GODS };
EffectMode blockEffect[NUM_LED_BLOCKS] = { FX_FLARE, FX_CURRENTS, FX_CURRENTS, FX_CANDLE };        // = Beelzebub Awakens (the default scene)
// Everything that is saved lives in this one array; the index IS the EEPROM address.
// Defaults: Beelzebub Awakens, MY CUSTOM = scene 1, normal brightness / speed, no favourites.
uint8_t cfg[21] = { EEPROM_MAGIC, SCENE_DEFAULT,
                    BEELZEBUB_REVEALED, BEELZEBUB_REVEALED, BEELZEBUB_REVEALED, BEELZEBUB_REVEALED,
                    FX_FLARE, FX_DRIFT, FX_WEB, FX_WHIRL,
                    0, 0, 1, 0, 1,
                    0, 0, 0, 0, 0,
                    LAYER_TWINKLE | LAYER_FADE };
#define currentScene cfg[1]
#define customMode (cfg + 2)           // MY CUSTOM palettes, one per block
#define customEffect (cfg + 6)         // MY CUSTOM animations
#define CFG_BRIGHT 10                  // cfg[10..12] = brightness, eye brightness, speed
#define brightnessLevel cfg[10]
#define eyeBrightnessLevel cfg[11]     // eyes are scaled by this on top of the main brightness
#define speedLevel cfg[12]
#define CFG_AUTOCYCLE 13
#define autoCycle cfg[13]
#define cycleLevel cfg[14]             // index into AUTO_CYCLE_TABLE (default 8 s)
#define favMask (cfg + 15)             // one bit per scene: favourite for auto-cycle
#define customLayer cfg[20]            // ambient layers of MY CUSTOM
uint8_t liveLayer = LAYER_TWINKLE | LAYER_FADE;       // ambient layers of the scene on screen

// ---- Runtime state --------------------------------------------------------
UiState uiState = UI_RUN;
uint8_t menuPage = 0;
bool menuSettings = false;           // the settings menu (brightness, eye brightness, speed) instead of the theme menu
uint32_t lastMenuActivity = 0;

uint32_t overlayUntil = 0;           // page / value indicator on the wing LEDs
uint8_t overlayCount = 0;
uint8_t overlay2Count = 0;           // LEDs up to this count (after the first group) use the 2nd colour
uint8_t overlay2G = 0, overlay2R = 0, overlay2B = 0;
uint8_t overlayG = 0, overlayR = 0, overlayB = 0;

uint32_t eggUntil = 0;               // easter egg
uint8_t eggPresses = 0;
uint32_t eggWindowStart = 0;

uint8_t ledIndex = 0;                // scroll position
uint8_t flareLeft = 0;               // frames left of the eyes' white flares
uint8_t stepAcc = 0, surgeAcc = 0;   // fractional animation speed
uint8_t surgeLeft = 0, surgeEnv = 0;   // a surge now and then: frames left, and how strong it is right now (0-255)
uint16_t phase = 0;                  // free-running animation phase
uint16_t slow = 0;                   // phase for the slow colour fades; wraps at 10240 so the colours never jump
uint8_t stormFlash = 0;
uint16_t stormGap = 0;               // frames until the next strike (scenes only, not the egg)
uint8_t sparkle[NUM_SLOTS];
uint8_t emberLevel[NUM_SLOTS];       // current ember brightness per LED
uint8_t emberTarget[NUM_SLOTS];      // brightness it is drifting towards
uint16_t rngState = 0xACE1;
uint32_t lastAutoCycle = 0;
bool morseActive = false;            // hidden Morse message
bool morseOn = false;                // eyes lit right now
uint8_t morseIdx = 0;
uint32_t morseNextAt = 0;
uint32_t lastShortAt = 0;            // when the last short press ended
bool blankOut = false;               // LEDs dark while a tap-tap-hold shortcut is being held (hides the scene steps)
uint8_t tapState = 0;                // auto-cycle state when the taps began: 0 = off, 1-4 = step length
uint8_t tapMoves = 0;                // how many of those taps moved the scene on
uint8_t tapRun = 0;                  // quick taps in a row (for the tap-tap-hold shortcuts)

// Forward declarations (the original relied on the Arduino auto-prototypes)
void sendByte(uint8_t byte, uint8_t ledPin);
void sendColor(uint8_t g, uint8_t r, uint8_t b, uint8_t ledPin);

// ---------------------------------------------------------------------------
//  Scenes
// ---------------------------------------------------------------------------

// Make 'scene' the live look.
void applyScene(uint8_t scene) {
    currentScene = scene;
    liveLayer = (scene == SCENE_CUSTOM) ? customLayer : SCENE_LAYER[scene];
    for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
        if (scene == SCENE_CUSTOM) {
            blockMode[i] = customMode[i];
            blockEffect[i] = customEffect[i];
        } else {
            blockMode[i] = (LedMode)SCENE_MODE[scene][i];
            blockEffect[i] = (EffectMode)SCENE_EFFECT[scene][i];
        }
        if (blockEffect[i] == FX_STORM) stormGap = 0;     // lightning scene: the first strike comes right away
    }
}

// ---------------------------------------------------------------------------
//  Saved settings (EEPROM). eeprom_update_byte only writes changed bytes,
//  so the EEPROM wears very little.
//  Layout: [0]=magic [1]=scene [2-5]=custom palettes [6-9]=custom animations
//          [10]=brightness [11]=eye brightness [12]=speed [13]=auto-cycle
//          [14]=auto-cycle time [15-19]=favourite scenes (bit mask) [20]=custom layers
// ---------------------------------------------------------------------------
void saveSettings() {
    for (uint8_t i = 0; i < sizeof(cfg); i++)
        eeprom_update_byte((uint8_t*)(EEPROM_ADDR + i), cfg[i]);
}

// Largest allowed value + 1 for each saved byte (0 = anything goes); bad data is ignored.
const uint8_t LIMIT[21] = { 0, 0,
    NUM_LED_MODES, NUM_LED_MODES, NUM_LED_MODES, NUM_LED_MODES,
    NUM_EFFECTS, NUM_EFFECTS, NUM_EFFECTS, NUM_EFFECTS,
    NUM_BRIGHTNESS_LEVELS, NUM_BRIGHTNESS_LEVELS, NUM_SPEED_LEVELS, 2, NUM_CYCLE_LEVELS,
    0, 0, 0, 0, 0,
    16 };

void loadSettings() {
    if (eeprom_read_byte((const uint8_t*)EEPROM_ADDR) != EEPROM_MAGIC) return; // nothing saved yet
    for (uint8_t i = 2; i < sizeof(cfg); i++) {
        uint8_t v = eeprom_read_byte((const uint8_t*)(EEPROM_ADDR + i));
        if (!LIMIT[i] || v < LIMIT[i]) cfg[i] = v;
    }
    uint8_t sc = eeprom_read_byte((const uint8_t*)(EEPROM_ADDR + 1));
    if (sc < NUM_SCENES) applyScene(sc);
}

void saveScene() { eeprom_update_byte((uint8_t*)(EEPROM_ADDR + 1), currentScene); }

// Favourite scenes (for auto-cycle).
bool isFavorite(uint8_t scene) { return (favMask[scene >> 3] >> (scene & 7)) & 1; }

bool flipFavorite() {                            // mark / unmark the scene on screen; true = now a favourite
    favMask[currentScene >> 3] ^= (1 << (currentScene & 7));
    return isFavorite(currentScene);
}

uint8_t sceneStep(uint8_t from, int8_t d) {      // scene number d steps away (wraps round)
    return (uint8_t)(from + NUM_SCENES + d) % NUM_SCENES;
}

// Next scene for auto-cycle: the next favourite, or simply the next scene if none are marked.
uint8_t nextAutoScene() {
    bool any = false;
    for (uint8_t i = 0; i < FAV_BYTES; i++) if (favMask[i]) any = true;
    uint8_t sc = currentScene;
    for (uint8_t k = 0; k < NUM_SCENES; k++) {
        sc = sceneStep(sc, 1);
        if (!any || isFavorite(sc)) return sc;
    }
    return currentScene;
}

// Everything back to the factory look: Beelzebub Awakens, custom = scene 1, normal settings.
void resetToDefaults() {
    for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
        customMode[i] = (LedMode)SCENE_MODE[0][i];
        customEffect[i] = (EffectMode)SCENE_EFFECT[0][i];
    }
    for (uint8_t i = CFG_BRIGHT; i < 20; i++) cfg[i] = 0;     // brightness, auto-cycle off, no favourites ...
    speedLevel = 1;
    cycleLevel = 1;
    customLayer = SCENE_LAYER[0];
    applyScene(SCENE_DEFAULT);
    saveSettings();
}

// ---------------------------------------------------------------------------
//  Colour palettes. One row per LedMode, 5 colours each, stored as {G, R, B}
//  (the order the LEDs want). FORCES_OF_DARKNESS only uses its first 4.
// ---------------------------------------------------------------------------
const uint8_t PALETTE[NUM_LED_MODES][5][3] = {
    { {130,70,0}, {75,35,0}, {0,80,110}, {0,40,60}, {0,0,0} },  // LORD_OF_THE_FLIES: Gangrene: dead-flesh greens, bruise purple, black
    { {0,255,0}, {0,200,0}, {0,140,0}, {0,80,0}, {40,255,0} },  // NIGHT_OF_DARKNESS: Blood: bright to deep red, a hot red flash
    { {0,90,150}, {0,170,255}, {0,90,150}, {0,35,70}, {0,120,20} },  // THE_SHADOWS: Cursed: purples fading to dark crimson
    { {255,40,0}, {200,0,0}, {70,0,0}, {0,210,0}, {0,0,0} },  // MAYBE_TODAY_BEELZEBUB: Toxic: acid greens, then red, with black between
    { {0,60,255}, {0,40,200}, {0,20,120}, {0,10,60}, {0,120,20} },  // FLY_BEELZEBUB: Midnight: deep blues fading to dark crimson
    { {135,150,100}, {88,100,62}, {45,55,30}, {0,110,0}, {0,0,0} },  // ANCIENT_GODS: Ossuary: old bone, dark bone, dried blood, black
    { {100,255,0}, {55,255,0}, {25,210,0}, {160,255,0}, {0,0,0} },  // FORCES_OF_DARKNESS: Hellfire: furnace oranges and reds
    { {0,255,0}, {90,255,0}, {0,0,0}, {170,30,0}, {0,100,0} },  // SEVEN_PRINCES: Hell's court: red, orange, black, sick green, dark red
    { {0,150,255}, {0,70,130}, {0,0,0}, {0,230,0}, {0,0,0} },  // BEELZEBUB_REVEALED: Unholy: violet and blood red, black between
    { {120,120,255}, {70,70,200}, {20,30,120}, {0,60,110}, {0,0,0} },  // NIGHT_OF_DISCORD: Ghost: cold spectral blue-violet, black
    { {230,170,0}, {0,0,0}, {0,120,200}, {0,60,110}, {0,0,0} },  // THE_HIVE: Venom: acid yellow-green and violet, black between
    { {230,170,0}, {180,120,0}, {110,60,0}, {200,190,0}, {30,20,0} },  // PLAGUE: Plague: sickly yellow-greens and rot black
    { {0,255,0}, {60,255,0}, {130,255,0}, {200,200,0}, {0,110,0} },  // BRIMSTONE: Brimstone: red, fire orange, sulphur yellow
    { {110,255,0}, {0,255,0}, {190,255,0}, {0,180,0}, {200,255,0} },  // COMPOUND_EYE: Evil eye: ember, red, amber, dark red, gold
    { {0,150,255}, {0,0,0}, {255,30,0}, {120,15,0}, {0,0,0} },  // IRIDESCENT: Witchfire: violet and ghostly green flames
    { {0,255,0}, {0,0,0}, {0,160,0}, {0,40,10}, {0,120,40} },  // CARRION: Carrion: blood red with black gaps, wine
};

uint8_t paletteSize(LedMode m) { return (m == FORCES_OF_DARKNESS) ? 4 : 5; }

const uint8_t* getModeColor(LedMode mode, uint8_t index) {
    return PALETTE[mode][index % paletteSize(mode)];
}

uint8_t col[3];                // the colour (G, R, B) the two functions below work out

// The colour of LED 'local' of a block. The eyes step through the palette together;
// the other blocks spread the palette over their own LEDs and scroll it with 'step'.
void blockPaletteColor(LedBlock block, uint8_t local, uint8_t step) {
    uint8_t idx = step;
    if (block != BLOCK_EYES)
        idx = (uint8_t)((uint16_t)(local + step) * 256u / BLOCK_SPAN[block]);
    const uint8_t* c = getModeColor(blockMode[block], idx);
    col[0] = c[0]; col[1] = c[1]; col[2] = c[2];
}

// ---------------------------------------------------------------------------
//  LED output
// ---------------------------------------------------------------------------

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
    for (uint8_t i = 0; i < numLeds; i++) {
        sendColor(green, red, blue, ledPin);
        _delay_us(500);
    }
}

// ---------------------------------------------------------------------------
//  Small helpers
// ---------------------------------------------------------------------------

bool timeLeft(uint32_t until) {            // true while millis() is before 'until'
    return (int32_t)(until - millis()) > 0;
}

uint8_t rand8() {                          // tiny xorshift pseudo-random generator
    rngState ^= rngState << 7;
    rngState ^= rngState >> 9;
    rngState ^= rngState << 8;
    return (uint8_t)(rngState ^ (rngState >> 8));
}

uint8_t scale8(uint8_t v, uint8_t s) {     // v * s / 255 (s = 255 leaves v unchanged)
    return ((uint16_t)v * ((uint16_t)s + 1)) >> 8;
}

uint8_t absd(uint8_t a, uint8_t b) { return (a > b) ? a - b : b - a; }   // distance between two numbers

uint8_t tri8(uint8_t x) {                  // triangle wave 0..254
    return (x < 128) ? (uint8_t)(x << 1) : (uint8_t)((255 - x) << 1);
}

bool stormInUse() {                                // any block running the lightning animation?
    for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) if (blockEffect[i] == FX_STORM) return true;
    return false;
}

LedMode nextMode(LedMode m)       { return (m + 1 >= NUM_LED_MODES) ? 0 : m + 1; }
EffectMode nextEffect(EffectMode e) { return (e + 1 >= NUM_EFFECTS) ? 0 : e + 1; }

// In the menu, pages 1-8 dim every block except the one being edited.
bool blockIsDimmed(LedBlock block) {
    return (uiState == UI_MENU) && (menuPage < PAGE_BRIGHTNESS) && (block != (LedBlock)(menuPage / 2));
}

// Show 'count' LEDs on the wing chain in a fixed colour for a moment.
void showOverlay(uint8_t count, uint8_t g, uint8_t r, uint8_t b, uint32_t ms) {
    overlayCount = count;
    overlayG = g; overlayR = r; overlayB = b;
    overlay2Count = 0;
    overlayUntil = millis() + ms;
}

// A number as white LEDs (scene number, or a palette / animation number in the
// menu). Past 20 all 20 LEDs light up; the white ones count the numbers beyond
// 20 and the rest are blood red (34 = 14 white + 6 red).
void showCount(uint8_t n, uint32_t ms, bool fav) {
    uint8_t g = fav ? 70 : 60, r = fav ? 90 : 60, b = fav ? 0 : 60;   // YELLOW = a favourite scene
    if (n <= NUM_WING_LEDS) {
        showOverlay(n, g, r, b, ms);
    } else {
        showOverlay(n - NUM_WING_LEDS, g, r, b, ms);
        overlay2Count = NUM_WING_LEDS;
        overlay2G = 0; overlay2R = 110; overlay2B = 0;
    }
}

// ---------------------------------------------------------------------------
//  Smooth colour (used by the slow effects)
// ---------------------------------------------------------------------------

uint8_t blend8(uint8_t a, uint8_t b, uint8_t f) {          // a -> b as f goes 0 -> 255
    return (b >= a) ? a + (((uint16_t)(b - a) * f) >> 8)
                    : a - (((uint16_t)(a - b) * f) >> 8);
}

// The colour of palette entry 'idx' as the EYES show it: never dark. A dark (black) entry
// (or any dim entry) repeats the entry before it, so the eyes never blink out while the colours melt into each other.
const uint8_t* eyeEntry(LedMode mode, uint8_t idx) {
    const uint8_t* c = getModeColor(mode, idx);
    for (uint8_t k = 1; k < 5 && c[0] < 100 && c[1] < 100 && c[2] < 100; k++)
        c = getModeColor(mode, idx + 20 - k);
    return c;
}

// t is in 8.8 fixed point: every 256 steps = one palette entry. The colour
// between two entries is a smooth mix of them, so nothing ever snaps.
void smoothColor(LedMode mode, int16_t t, bool eye) {
    uint8_t n = paletteSize(mode);
    uint16_t u = (uint16_t)(t + 10240) % ((uint16_t)n * 256);   // +10240 (a multiple of every span) keeps it positive
    uint8_t idx = (uint8_t)(u >> 8);
    uint8_t nxt = (idx + 1 == n) ? 0 : idx + 1;
    uint8_t f = (uint8_t)u;
    const uint8_t* c0 = eye ? eyeEntry(mode, idx) : getModeColor(mode, idx);
    const uint8_t* c1 = eye ? eyeEntry(mode, nxt) : getModeColor(mode, nxt);
    for (uint8_t k = 0; k < 3; k++) col[k] = blend8(c0[k], c1[k], f);
}

// Where in the palette a slow effect is, for the LED at 'pos' (distance out from the wing base):
// the base moves with 'slow' (the first two effects at full speed, the rest at half) and each
// effect spreads the colours over the LEDs by its own amount (negative = flows the other way).
const int8_t SMOOTH_SPREAD[LAST_SMOOTH_FX - FIRST_SMOOTH_FX + 1] = { 10, -70, 20, 40 };   // crossfade, currents, gaze, ripple
int16_t smoothT(EffectMode fx, uint8_t pos) {
    int16_t base = (fx <= FX_CURRENTS) ? slow : (slow >> 1);
    return base + pos * SMOOTH_SPREAD[fx - FIRST_SMOOTH_FX];
}

// ---------------------------------------------------------------------------
//  Animations
// ---------------------------------------------------------------------------

#define NUM_POS 16          // 'pos' runs 0..15 out along the wings (comets wrap round this)

// Advance everything that changes from frame to frame.
void stepAnimation() {
    uint8_t inc = SPEED_INC[speedLevel];
    uint8_t rate = SCENE_SPEED[currentScene];
    if (liveLayer & LAYER_FLUSH)                             // SWELL scenes also breathe in speed, as the Cthulhu ones do (about 0.6x to 1.5x every 5 s)
        rate = (uint8_t)(rate * (5 + (tri8((uint8_t)slow) >> 5))) >> 3;
    if (surgeLeft) surgeLeft--;                              // now and then a short burst (2.4 s) of up to twice the speed, then back to normal
    else if ((liveLayer & LAYER_SURGE) && rand8() < 2 && rand8() < 128) surgeLeft = 60;
    surgeEnv = tri8((uint8_t)((surgeLeft * 17) >> 2));
    rate += scale8(surgeEnv, rate);
    if (!flareLeft && rand8() < 2) flareLeft = 72;           // every ~8 s: flash, a pause, flash the flare eyes flash white twice (1 s each)
    if (flareLeft) flareLeft--;
    surgeAcc += rate * inc;
    uint8_t spd = surgeAcc >> 4;
    surgeAcc &= 15;
    phase += spd;
    slow += inc;
    if (slow >= 10240) slow -= 10240;
    stepAcc += spd;
    if (stepAcc >= 10) {                       // normal speed: every 5 frames = 200 ms
        stepAcc -= 10;
        if (++ledIndex >= NUM_POS) ledIndex = 0;
    }

    // Storm: random lightning. Odd counts show white, so one strike flickers.
    bool rare = !timeLeft(eggUntil);                              // every lightning scene: one strike every 2-4 minutes
    if (stormFlash > 0) {
        stormFlash--;
    } else if (rare && stormGap) {
        stormGap--;
    } else if (rare ? stormInUse() : rand8() < 6) {    // (the egg: a strike every ~1.5 s)
        stormFlash = 3 + (rand8() & 3) * 2;
        if (rare) stormGap = 3000 + rand8() * 12;        // scenes: one strike every 2-4 minutes
    }

    // Sparks: fade every LED, light a random one now and then (twinkle layer, dying lights).
    if (rand8() < 75) sparkle[scale8(rand8(), NUM_SLOTS - 1)] = 255;

    // Embers: every LED drifts smoothly towards a random brightness.
    for (uint8_t s = 0; s < NUM_SLOTS; s++) {
        if (sparkle[s]) sparkle[s] -= (sparkle[s] >> 3) + 1;
        if (rand8() < 50) emberTarget[s] = 90 + scale8(rand8(), 165);
        if (emberTarget[s] > emberLevel[s]) emberLevel[s] += (emberTarget[s] - emberLevel[s] + 3) >> 2;
        else                                emberLevel[s] -= (emberLevel[s] - emberTarget[s] + 3) >> 2;
    }

}

// A soft ring of light: age < 0 = approaching, 0.. = just passed (bright, fading).
uint8_t ringLevel(int16_t age) {
    if (age >= -32 && age < 0)  return 15 + (age + 32) * 7;          // soft leading edge
    if (age >= 0 && age < 96)   return 255 - ((age * 5) >> 1);       // glowing tail
    return 15;
}

uint8_t wrapN(uint8_t x, uint8_t n) {      // x mod n without a divide (x is always small)
    while (x >= n) x -= n;
    return x;
}

uint8_t lvl(uint8_t base, uint8_t amp, uint8_t v) {      // base + v scaled to 0..amp
    return base + (((uint16_t)v * amp) >> 8);
}

// A ring that spreads out as 'dist' (0..255) grows; 'speed' is the phase it has reached.
uint8_t ringAt(uint8_t front, uint8_t dist) {
    return ringLevel((int16_t)(uint8_t)(front + 32 - dist) - 32);   // wraps cleanly
}

// Brightness (0-255) that an animation gives to one LED.
// pos  = how far out along the wings (0-15, eyes 0/6/12/18)   slot = unique LED number (0-23)
// lag  = phase offset so the four eyes do not move in lock-step (0 for the wings)
// The geometry effects (ripple, radar, wingbeat, sweep, web, infect, whirl, drip) also use the
// measured position of the LED on the PCB: LED_Y / LED_R / LED_A.
uint8_t effectLevel(EffectMode fx, uint8_t pos, uint8_t slot, uint8_t lag) {
    switch (fx) {
        case FX_STORM:
            return (stormFlash & 1) ? 255 : 40;
        case FX_SPARKLE:                          // a dim glow; random LEDs pop bright and fade
            return (sparkle[slot] > 55) ? sparkle[slot] : 55;
        case FX_EMBER:
            return emberLevel[slot];
        case FX_RIPPLE:                           // soft rings leaving the wing base
            return ringAt((uint8_t)(phase >> 1), LED_R[slot]);
        case FX_WEB:                              // rings along spokes: a spider web
            return lvl(8, 247, scale8(tri8((uint8_t)(LED_R[slot] * 3 - (phase << 1))), tri8((uint8_t)(LED_A[slot] * 5))));
        case FX_WHIRL: {                          // two arms spinning round the wing base
            uint8_t t = tri8((uint8_t)(LED_A[slot] * 2 - (phase << 1)));
            return lvl(8, 247, ((uint16_t)t * t) >> 8);
        }
        case FX_DYING:                            // LEDs sputter out and come back
            return 255 - scale8(sparkle[slot], 235);
        case FX_CHASE:                            // marching lights
            return ((uint8_t)(slot + (phase >> 4)) % 3) ? 30 : 255;
        case FX_THROB:                            // a strong pulse, all together
            return lvl(40, 215, scale8(tri8((uint8_t)(phase << 1)), tri8((uint8_t)(phase << 1))));
        case FX_COMET:                            // a comet runs along the chain of LEDs
            return ringAt((uint8_t)(phase << 1), (uint8_t)(slot * 13));
        case FX_SPIRAL:                           // a spiral winds out from the wing base
            return ringAt((uint8_t)(phase >> 1), (LED_R[slot] >> 1) + (LED_A[slot] >> 1));
        case FX_RISE:                             // embers rise up the badge
            return ringAt((uint8_t)(phase >> 1), 255 - LED_Y[slot]);
        case FX_CANDLE:                           // each LED gutters like a candle flame
            return lvl(70, 185, emberLevel[slot]);
        case FX_DIM:                              // dim glow that slowly breathes
            return lvl(18, 36, tri8((uint8_t)((phase >> 2) + pos * 12 - lag)));
        case FX_CURRENTS:                         // gentle shimmer on top of the flowing colours
            return lvl(190, 65, tri8((uint8_t)((phase >> 1) - pos * 14)));
        case FX_GAZE: {                           // open (2.5 s) - hold (4 s) - close (4 s) - dark (1.5 s)
            uint16_t p = phase - lag - pos * 12;
            uint8_t c = (p >> 3) % 64;
            if (c < 16) return 90 + c * 10;
            if (c < 36) return 255;
            if (c < 56) return 90 + (55 - c) * 8;
            return 90;                                // never fully dark
        }
        case FX_RADAR: {                          // a beam swings round the wing base (a bright beam with a faint trail)
            uint8_t b = tri8((uint8_t)(phase >> 1));
            uint8_t d = absd(LED_A[slot], b);
            return (d < 28) ? (uint8_t)(255 - d * 8) : 14;
        }
        case FX_WINGBEAT:                         // flap: a pulse runs from the base out to the tips
            return ringAt((uint8_t)(phase << 1), LED_R[slot]);
        default:                                  // FX_DRIFT and FX_CROSSFADE: full brightness
            return 255;
    }
}

// Colours are first worked out into these buffers (interrupts ON, so the
// millis() timer keeps ticking), then clocked out in one quick burst with
// interrupts OFF. Keeping that burst short stops millis() losing ticks.
uint8_t prevBuf[NUM_SLOTS * 3];        // last frame shown, for the gliding fades
uint8_t frameBuf[NUM_SLOTS * 3];       // the 20 wing pixels first, then the 4 eyes (G, R, B each)
uint8_t fillPos = 0;
#define EYE_PIXELS (frameBuf + NUM_WING_LEDS * 3)

void pushPixel(uint8_t g, uint8_t r, uint8_t b) {
    if (fillPos < sizeof(frameBuf)) { frameBuf[fillPos++] = g; frameBuf[fillPos++] = r; frameBuf[fillPos++] = b; }
}

// Queue a colour after applying the brightness settings.
void sendOut(uint8_t g, uint8_t r, uint8_t b, bool isEye) {
    uint8_t br = BRIGHTNESS_TABLE[brightnessLevel];
    if (isEye) br = scale8(br, BRIGHTNESS_TABLE[eyeBrightnessLevel]);
    pushPixel(scale8(g, br), scale8(r, br), scale8(b, br));
}

// The ambient layers (fade wave, twinkles) applied to the brightness m of one LED.
uint8_t layerLevel(uint8_t m, uint8_t pos, uint8_t slot) {
    if (slot >= NUM_WING_LEDS) return m;                   // the eyes never twinkle or fade: they keep their own steady animation
    if (liveLayer & LAYER_FADE)                           // a slow wave of dimming rolls through the LEDs
        m = scale8(m, lvl(70, 185, tri8((uint8_t)((phase >> 2) - pos * 10))));
    if ((liveLayer & LAYER_TWINKLE) && sparkle[slot] > m) m = sparkle[slot];   // random flares
    return m;
}

// Compute and queue one LED of a normal block.
void sendLed(LedBlock block, uint8_t local, uint8_t pos, uint8_t slot, uint8_t lag) {
    EffectMode fx = blockEffect[block];
    // Menu, palette page of this block: show the palette itself, steady, one colour per LED
    // (a block with fewer than 5 LEDs steps through its colours so you still see them all).
    bool swatch = (uiState == UI_MENU) && (menuPage < PAGE_BRIGHTNESS) && !(menuPage & 1) && (block == menuPage / 2);
    if (swatch) {
        const uint8_t* c = getModeColor(blockMode[block], local + ((BLOCK_SPAN[block] < 5) ? ledIndex : 0));
        col[0] = c[0]; col[1] = c[1]; col[2] = c[2];
    }
    else if (fx >= FIRST_SMOOTH_FX && fx <= LAST_SMOOTH_FX) smoothColor(blockMode[block], smoothT(fx, pos), block == BLOCK_EYES);
    else if (block == BLOCK_EYES) smoothColor(blockMode[block], (slow << 1) + pos * 24, true);   // eyes always melt smoothly from colour to colour (each eye a different shade)
    else if (fx == FX_DRIFT)   blockPaletteColor(block, local, ledIndex);
    else                       smoothColor(blockMode[block], (slow >> 1) + local * 24, true);   // one slowly melting colour per wing, so the light patterns stand out
    uint16_t ph = phase;                                 // the eyes run their effect a little behind each other: a pulse chases from eye to eye
    phase -= lag;
    uint8_t m = swatch ? 255 : effectLevel(fx, pos, slot, lag);
    phase = ph;
    if (block == BLOCK_EYES && fx >= FX_RADAR && fx <= FX_RISE) m = lvl(105, 150, m);   // a moving pulse, but the eye never goes dark
    if (!swatch) m = layerLevel(m, pos, slot);           // palette preview stays steady
    if (!swatch && block == BLOCK_EYES && fx != FX_GAZE)                  // every eye slowly breathes, each at its own time (never dark)
        m = scale8(m, lvl(90, 165, tri8((uint8_t)((phase >> 1) - lag))));
    if (fx == FX_STORM && (stormFlash & 1)) col[0] = col[1] = col[2] = 255;   // lightning is white
    if (block == BLOCK_EYES) {                           // the eyes flush white when the scene surges; FLARE eyes also flare slowly on their own
        uint8_t w = 0;
        uint8_t u = (flareLeft > 48) ? flareLeft - 48 : (flareLeft <= 24) ? flareLeft : 0;   // flash (1 s) ... a beat of dark ... flash (1 s)
        uint8_t f = u ? tri8((uint8_t)((u * 85) >> 3)) : 0;
        if (fx == FX_FLARE && f > w) w = f;
        for (uint8_t k = 0; k < 3; k++) col[k] = blend8(col[k], 255, w);
    }
    uint8_t sh = blockIsDimmed(block) ? MENU_DIM_SHIFT : 0;
    for (uint8_t k = 0; k < 3; k++) col[k] = scale8(col[k], m) >> sh;
    sendOut(col[0], col[1], col[2], block == BLOCK_EYES);
}

// How the badge is drawn: normal scenes, the hidden easter egg (a hellish storm
// with lightning), or the Morse message.
#define LOOK_NORMAL 0
#define LOOK_STORM 1
#define LOOK_MORSE 2

void showEyes(uint8_t look) {
    for (uint8_t i = 0; i < NUM_EYE_LEDS; i++) {
        if (look == LOOK_MORSE) {    // eyes ARE the Morse lamp
            if (morseOn) sendOut(120, 255, 0, true);
            else         sendOut(0, 0, 0, true);
        } else if (look == LOOK_STORM) {   // eyes pulse bright blood-red
            uint8_t p = layerLevel(tri8((uint8_t)(phase << 2)), i * 6, NUM_WING_LEDS + i);
            if (stormFlash & 1) sendOut(255, 255, 255, true);
            else sendOut(p >> 4, p, 0, true);
        } else {
            sendLed(BLOCK_EYES, i, (i >> 1) * 12, NUM_WING_LEDS + i, (i >> 1) * 128);   // 2 LEDs per eye: both LEDs of an eye match, the two eyes are offset
        }
    }
}

// The wing LEDs are one chain, so they are sent in chain order 0..19; each one
// looks up which block it belongs to.
void showWings(uint8_t look) {
    bool overlay = timeLeft(overlayUntil);
    for (uint8_t i = 0; i < NUM_WING_LEDS; i++) {
        if (overlay && i < overlayCount) {            // menu / scene indicator
            if ((i % 5) == 4) pushPixel(0, 90, 60);   // every 5th LED is magenta: easy to count
            else              pushPixel(overlayG, overlayR, overlayB);
        } else if (overlay && i < overlay2Count) {
            pushPixel(overlay2G, overlay2R, overlay2B);
        } else if (look == LOOK_STORM) {              // dark smouldering wings, white lightning (during Morse the wings simply carry on with their scene)
            uint8_t m = tri8((uint8_t)((phase << 2) - LED_R[i])) >> 2;
            if (stormFlash & 1) sendOut(255, 255, 255, false);
            else sendOut(m >> 2, m, 0, false);
        } else {
            sendLed((LedBlock)LED_BLOCK[i], LED_LOCAL[i], LED_R[i] >> 4, i, 0);
        }
    }
}

// Scale the whole frame down if the LEDs would draw more than MAX_LED_CURRENT_MA.
uint16_t sumBuf(const uint8_t* b, uint8_t n) {
    uint16_t t = 0;
    while (n--) t += *b++;
    return t;
}

void scaleBuf(uint8_t* b, uint8_t n, uint8_t f) {
    while (n--) { *b = scale8(*b, f); b++; }
}

void limitCurrent() {
    uint16_t total = sumBuf(frameBuf, sizeof(frameBuf));   // at most 18360
    uint16_t mA = ((total >> 2) * 5) >> 4;         // 20 mA per colour channel at 255 (total * 5 / 64)
    if (mA <= MAX_LED_CURRENT_MA) return;
    uint8_t f = (uint8_t)((MAX_LED_CURRENT_MA * 128u) / mA) * 2;   // 16-bit maths only
    scaleBuf(frameBuf, sizeof(frameBuf), f);
}

void renderFrame() {
    uint8_t look = LOOK_NORMAL;
    if (morseActive) look = LOOK_MORSE;
    else if (timeLeft(eggUntil)) look = LOOK_STORM;
    fillPos = 0;
    showWings(look);         // work out all colours (interrupts stay on)
    showEyes(look);
    if (blankOut) {                                  // dark during a shortcut hold
        scaleBuf(frameBuf, sizeof(frameBuf), 0);
    }
    // Glide: every LED moves half way to its new brightness each frame, so steps and
    // jumps melt into smooth fades. Skipped for lightning, menus and number displays.
    bool glide = !blankOut && uiState == UI_RUN && !timeLeft(overlayUntil) && look == LOOK_NORMAL && !stormInUse();
    for (uint8_t i = 0; i < sizeof(frameBuf); i++) {
        uint8_t c = frameBuf[i], p = prevBuf[i];
        if (glide) c = (p + c + (c > p)) >> 1;
        frameBuf[i] = prevBuf[i] = c;
    }
    limitCurrent();

    // Interrupts are off only while a strip is clocked out (about 0.7 ms), so
    // the millis() tick cannot stretch an LED bit - and cannot be lost either.
    noInterrupts();
    for (uint8_t i = 0; i < NUM_WING_LEDS; i++)
        sendColor(frameBuf[i * 3], frameBuf[i * 3 + 1], frameBuf[i * 3 + 2], WING_LED_ARRAY);
    interrupts();
    _delay_us(500); // latch

    noInterrupts();
    for (uint8_t i = 0; i < NUM_EYE_LEDS; i++)
        sendColor(EYE_PIXELS[i * 3], EYE_PIXELS[i * 3 + 1], EYE_PIXELS[i * 3 + 2], EYE_LED_ARRAY);
    interrupts();
    _delay_us(500); // latch
}

void fillAll(uint8_t g, uint8_t r, uint8_t b) {      // every LED one colour (0,0,0 = all off)
    colorWipe(g, r, b, EYE_LED_ARRAY, NUM_EYE_LEDS);
    colorWipe(g, r, b, WING_LED_ARRAY, NUM_WING_LEDS);
}

// Quick blink of every LED in one colour - confirms entering/leaving the menu etc.
void blinkAll(uint8_t g, uint8_t r, uint8_t b, uint8_t times) {
    for (uint8_t t = 0; t < times; t++) {
        fillAll(g, r, b);
        _delay_ms(120);
        fillAll(0, 0, 0);
        _delay_ms(120);
    }
}

void blinkOnOff(bool on, uint8_t times) { blinkAll(on ? 80 : 0, on ? 0 : 80, 0, times); }   // green = on, red = off

// ---------------------------------------------------------------------------
//  Setup / minibadge clock
// ---------------------------------------------------------------------------

// Hidden Morse message. One element (dot or dash) per step; times are scheduled
// from the previous step so the rhythm never drifts.
void startMorse() {
    morseActive = true;
    morseOn = false;
    morseIdx = 0;
    morseNextAt = millis() + 600;          // short pause, then it starts
}

void stopMorse() {
    morseActive = false;
    morseOn = false;
    lastAutoCycle = millis();
}

void updateMorse() {
    if (!morseActive || timeLeft(morseNextAt)) return;
    if (morseOn) {                         // element finished: 1 unit of dark
        morseOn = false;
        uint32_t gap = MORSE_UNIT_MS;
        char nx = MORSE_MSG[morseIdx];
        if (nx == ' ')      { gap += 2UL * MORSE_UNIT_MS; morseIdx++; }   // letter gap = 3 units
        else if (nx == '/') { gap += 6UL * MORSE_UNIT_MS; morseIdx++; }   // word gap = 7 units
        morseNextAt += gap;
    } else {                               // start the next dot or dash
        char c = MORSE_MSG[morseIdx++];
        if (c == 0) { stopMorse(); return; }
        morseOn = true;
        morseNextAt += (c == '-') ? 3UL * MORSE_UNIT_MS : 1UL * MORSE_UNIT_MS;
    }
}

bool isButtonPressed() {
    return (PORTA.IN & (1 << BUTTON_PIN)) == 0;
}

// Minibadge clock: 1 second with H high / L low, then 1 second with H low / L high.
bool clockPhase = false;

void setMinibadgeClock(bool phaseHigh) {
    if (phaseHigh) {
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_H_PIN);
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_L_PIN);
    } else {
        PORTA.OUTCLR = (1 << MINIBADGE_CLK_CTRL_H_PIN);
        PORTA.OUTSET = (1 << MINIBADGE_CLK_CTRL_L_PIN);
    }
}

void updateMinibadgeClock() {
    clockPhase = !clockPhase;      // our own state - no need to read the pin back
    setMinibadgeClock(clockPhase);
}

void setup() {
    // Set eye LED array and wing LED array as output
    PORTA.DIRSET = (1 << EYE_LED_ARRAY) | (1 << WING_LED_ARRAY);

    // Set button pin as input with pull-up resistor
    PORTA.DIRCLR = (1 << BUTTON_PIN);
    PORTA.OUTSET = (1 << BUTTON_PIN);

    // Set minibadge clock pins as output
    PORTA.DIRSET = (1 << MINIBADGE_CLK_CTRL_H_PIN) | (1 << MINIBADGE_CLK_CTRL_L_PIN);

    setMinibadgeClock(clockPhase);   // start with a defined clock state

    // Ensure all LEDs are off at the start
    fillAll(0, 0, 0);

    // Power-up hold: 8 s (eyes turn red) then let go = reset.
    bool didReset = false;
    if (isButtonPressed()) {
        uint16_t held = 0;
        while (isButtonPressed() && held < RESET_HOLD_MS) {
            _delay_ms(10);
            held += 10;
        }
        if (held >= RESET_HOLD_MS) {
            colorWipe(0, 80, 0, EYE_LED_ARRAY, NUM_EYE_LEDS);                                // red eyes
            while (isButtonPressed()) _delay_ms(10);                                         // wait for let-go
            resetToDefaults();
            blinkAll(80, 0, 0, 3);                                                           // green blinks = reset done
            didReset = true;
        }
    }
    if (!didReset) loadSettings();
    _delay_ms(100);
}

// ---------------------------------------------------------------------------
//  Button handling (non-blocking, short press vs long press)
// ---------------------------------------------------------------------------

void showScene() {
    showCount(currentScene + 1, SCENE_OVERLAY_MS, isFavorite(currentScene));
}

void showPageNumber() {
    if (menuSettings) showOverlay(menuPage + 1 - PAGE_BRIGHTNESS, 60, 100, 0, PAGE_OVERLAY_MS);   // orange row = settings page
    else              showOverlay(menuPage + 1, 0, 0, 100, PAGE_OVERLAY_MS);                       // blue row = theme page
}

void showValue(uint8_t n) {
    showCount(n, VALUE_OVERLAY_MS, false);                    // white row = chosen value
}

void closeMenu() {
    uiState = UI_RUN;
    overlayUntil = 0;
    lastAutoCycle = millis();   // auto-cycle waits a full interval after the menu
    saveSettings();
}

// Change the value shown on the current menu page.
void menuShortPress() {
    lastMenuActivity = millis();
    if (menuPage < PAGE_BRIGHTNESS) {
        overlayUntil = 0;                      // show the change at once (the page number row goes away)
        // Editing: copy the scene you are looking at into MY CUSTOM (the last scene) first.
        if (currentScene != SCENE_CUSTOM) {
            for (uint8_t i = 0; i < NUM_LED_BLOCKS; i++) {
                customMode[i] = blockMode[i];
                customEffect[i] = blockEffect[i];
            }
            customLayer = liveLayer;
            currentScene = SCENE_CUSTOM;
        }
        LedBlock blk = (LedBlock)(menuPage / 2);
        if ((menuPage & 1) == 0) {
            blockMode[blk] = nextMode(blockMode[blk]);
            customMode[blk] = blockMode[blk];
        } else {
            blockEffect[blk] = nextEffect(blockEffect[blk]);
            customEffect[blk] = blockEffect[blk];
            if (blockEffect[blk] == FX_STORM) {          // picking lightning: one strike now, so you can see it
                stormFlash = 7;
                stormGap = 3000 + rand8() * 12;
            }
        }
    } else {                                          // brightness, eye brightness, speed
        uint8_t* p = &cfg[CFG_BRIGHT + menuPage - PAGE_BRIGHTNESS];   // their places in the saved settings
        if (++*p >= ((menuPage == PAGE_SPEED) ? NUM_SPEED_LEVELS : NUM_BRIGHTNESS_LEVELS)) *p = 0;
        showValue(*p + 1);
    }
}

// Shortcuts (run mode): mark / unmark the scene you are on as a favourite,
// and start / stop auto-cycling. Both save straight away.
void toggleFavorite() {
    bool fav = flipFavorite();
    saveSettings();
    blinkOnOff(fav, 1);                  // green blink = now a favourite, red = no longer
    showScene();                                              // number shows yellow for a favourite
}

// 3 taps + hold steps through: off -> 4 s -> 8 s -> 20 s -> 60 s -> off.
// Green blinks = the step length (1 = 4 s ... 4 = 60 s), 2 red blinks = off.
void toggleCycling() {
    uint8_t st = tapState;                  // the first tap of a running cycle already stopped it
    st = (st >= NUM_CYCLE_LEVELS) ? 0 : st + 1;
    autoCycle = (st != 0);
    if (st) cycleLevel = st - 1;
    lastAutoCycle = millis();
    saveSettings();
    blinkOnOff(autoCycle, autoCycle ? st : 2);
}

void onShortPress() {
    if (morseActive) { stopMorse(); return; }          // any press stops the Morse message
    if (uiState == UI_MENU) {
        menuShortPress();
        return;
    }
    uint32_t now = millis();
    tapRun = (tapRun && (now - lastShortAt) < TAP_GAP_MS) ? tapRun + 1 : 1;   // for the tap-tap-hold shortcuts
    if (tapRun == 1) { tapMoves = 0; tapState = autoCycle ? cycleLevel + 1 : 0; }
    lastShortAt = now;
    if (autoCycle && !timeLeft(eggUntil)) {            // cycling: one press STAYS on this scene
        autoCycle = false;
        eeprom_update_byte((uint8_t*)(EEPROM_ADDR + CFG_AUTOCYCLE), 0);
        saveScene();
        showScene();
        eggPresses = 0;
        return;
    }
    // Run mode: next scene
    tapMoves++;
    applyScene(sceneStep(currentScene, 1));
    showScene();                                     // white row = scene number (yellow = favourite)
    saveScene();

    // Easter egg: EGG_PRESSES quick presses
    if (eggPresses == 0 || (now - eggWindowStart) > EGG_WINDOW_MS) {
        eggPresses = 0;
        eggWindowStart = now;
    }
    eggPresses++;
    if (eggPresses >= EGG_PRESSES) {
        eggPresses = 0;
        applyScene(sceneStep(currentScene, -EGG_PRESSES));  // undo the scene steps
        overlayUntil = 0;
        saveScene();
        eggUntil = now + EGG_DURATION_MS;
    }
}

void onLongPress(uint32_t pressStart) {
    blankOut = false;
    if (morseActive) { stopMorse(); return; }
    // Tap-and-hold shortcuts: 1 tap + hold = settings menu, 2 = favourite, 3 = start/stop
    // cycling, 4 = Morse message. The taps moved the scene on, so that is undone first.
    uint8_t taps = (tapRun && (pressStart - lastShortAt) < TAP_GAP_MS) ? tapRun : 0;
    tapRun = 0;
    eggPresses = 0;
    if (uiState == UI_RUN && taps >= 1 && taps <= 4) {
        applyScene(sceneStep(currentScene, -tapMoves));   // undo the scene steps the taps made
        overlayUntil = 0;
        saveScene();
        if (taps == 2)      toggleFavorite();
        else if (taps == 3) toggleCycling();
        else if (taps == 4) startMorse();
        if (taps != 1) return;
    }
    if (uiState == UI_RUN) {
        uiState = UI_MENU;
        menuSettings = (taps == 1);
        menuPage = menuSettings ? PAGE_BRIGHTNESS : 0;
        lastMenuActivity = millis();
        blinkAll(80, 80, 80, 2);
        showPageNumber();
    } else {
        lastMenuActivity = millis();
        menuPage++;
        if (menuPage >= (menuSettings ? NUM_PAGES : PAGE_BRIGHTNESS)) {      // left the last page: close and save
            closeMenu();
            blinkAll(80, 80, 80, 1);
        } else {
            showPageNumber();
        }
    }
}

void handleButton() {
    static bool wasPressed = false;
    static bool longHandled = false;
    static uint32_t pressStart = 0;
    static uint32_t releasedAt = 0;

    bool pressed = isButtonPressed();
    uint32_t now = millis();

    if (pressed && !wasPressed) {
        if ((now - releasedAt) < 40) return;      // contact bounce right after letting go: ignore
        wasPressed = true;
        longHandled = false;
        pressStart = now;
    } else if (pressed && wasPressed) {
        if (tapRun >= 1 && (pressStart - lastShortAt) < TAP_GAP_MS && (now - pressStart) >= 300) blankOut = true;
        if (!longHandled && (now - pressStart) >= LONG_PRESS_MS) {
            longHandled = true;
            onLongPress(pressStart);
        }
    } else if (!pressed && wasPressed) {
        wasPressed = false;
        blankOut = false;
        releasedAt = now;
        if (!longHandled && (now - pressStart) >= DEBOUNCE_MS) {
            onShortPress();
        }
    }

    // Leave the menu (and save) if nobody touches the button for a while
    if (uiState == UI_MENU && (now - lastMenuActivity) >= MENU_TIMEOUT_MS) {
        closeMenu();
    }
}

// ---------------------------------------------------------------------------
//  Main loop
// ---------------------------------------------------------------------------

void loop() {
    static uint32_t lastClockUpdate = 0;
    const uint32_t clockUpdateInterval = 1000; // 1 second per half-cycle
    static uint32_t lastFrameTime = 0;

    uint32_t now = millis();

    // Minibadge clock first, so nothing below can delay it.
    // The schedule advances by exactly 1000 ms each time (no drift).
    if ((now - lastClockUpdate) >= clockUpdateInterval) {
        lastClockUpdate += clockUpdateInterval;
        if ((now - lastClockUpdate) >= clockUpdateInterval) lastClockUpdate = now;  // fell far behind (menu blink): resync
        updateMinibadgeClock();
    }

    handleButton();
    now = millis();

    // Auto-cycle: step through the scenes. Paused in the menu and during the egg.
    if (autoCycle && uiState == UI_RUN && !timeLeft(eggUntil) && !morseActive &&
        (now - lastAutoCycle) >= AUTO_CYCLE_TABLE[cycleLevel]) {
        lastAutoCycle = now;
        applyScene(nextAutoScene());                   // not saved, so it never wears the EEPROM
    }

    updateMorse();

    // Draw one animation frame
    if ((now - lastFrameTime) >= FRAME_MS) {
        lastFrameTime = now;
        stepAnimation();
        renderFrame();
    }
}

