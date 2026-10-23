# Beelzebub Minibadge Holder

These are the instructions for the updated firmware. If you bought the badge BEFORE Saintcon 2026, you have the original firmware (beelzebub_firmware_v1.ino) which only has a handful of LED scenes that shuffle through with a click. If you bought it AT Saintcon 2026 or after, it has the updated firmware (beelzebub_firmware_v2.ino) which includes all of the LED scenes and functionality listed below. I recommend upgrading if you have the old firmware. Feel free to reach out on Discord if you need help upgrading. 

# Beelzebub Minibadge Holder Upgraded Firmware Instructions

*"Maybe Tooday Beelzebub"*

<img src="/images/beelzebub_2.jpg" width="500" />


## Assembly Instructions

You should have the following items:

- Badge PCB
- (10) 8 pin headers
- (5) Minibadge kits
- Lanyard
- Battery Holder (3 AAA)
- 3 AAA batteries
- Instruction book

The Beelzebub Minibadge Holder is mostly assembled. The only required assembly is to solder the 10 minibadge headers to the front and the optional battery holder to the back. Be careful while soldering these that you don't damage any of the components on the back. If you use the battery holder, make sure that you match it to the outline on the back before soldering. Last, install the lanyard and it ready to use. Note on battery usage: Because of the number of LEDs and potential minibadges on the badge, the batteries don't last long. I recommend using the USB to power if planning to run for more than 3-4 hours. 

Verify power by making sure switch is in Down position for USB power and Up position for battery power. 

## Operating Instructions

The minibadge holder has four LED groups:

- the eyes (4 LEDs, two in each eye)
- the thorax (2 LEDs)
- the left wing (10 LEDs)
- the right wing (8 LEDs)

Each group has its own color palette and its own animation. The 20 LEDs of the thorax and wings are one chain, so the animations know where each LED really sits on the badge. The eyes always stay lit and melt smoothly from color to color.

It has one button, and everything is done with taps and holds.

A new or reset badge starts on the Beelzebub Awakens scene, scene 24 (scenes are listed below). When the badge is powered off all custom settings are saved.

### Button Controls

| Action | What it does |
| --- | --- |
| Short press | Next scene. A row of LEDs shows the scene number, white, or yellow if the scene is a favourite. |
| Hold (0.8 s) | Open the Theme Menu. |
| 1 tap + hold | Open the Settings Menu. |
| 2 taps + hold | Marks or unmarks the current scene as a favorite. Green blink = added, red blink = removed. |
| 3 taps + hold | Start auto-cycling (see below). |
| 4 taps + hold | Play the Morse message on the eyes ("LORD OF THE FLIES"). Any press stops it. |
| 5 quick taps | Play the Hellstorm show with lightning (12 s). |

"Tap + hold" means tap quickly, then hold on the last tap.

### Scene number display

- Scenes 1–20 light that many LEDs on the wings.
- Above 20, all 20 LEDs light up. The white or yellow ones count how far past 20 you are, and the rest are blood red.
- Every 5th LED is magenta to make it easy to count.
- The menus use the same display for palette and animation numbers.

### Auto-cycling

Auto-cycling switches scenes by itself. There's not a menu for it. You set it with 3 taps + hold: tap quickly three times, then hold the button down at least 0.8s. Each time you do this, the badge moves to the next step:

| Step | Blinks | Switches scene every |
| --- | --- | --- |
| Off | 2 red | never |
| 1 | 1 green | 4 seconds |
| 2 | 2 green | 8 seconds |
| 3 | 3 green | 20 seconds |
| 4 | 4 green | 60 seconds |

After step 4, the next 3 taps + hold turns it off again, and the steps repeat.

- **Which scenes:** it cycles through scenes that have been marked as favorites. If you haven't marked any, it cycles through every scene.
- **To stop it:** one short press stays on the scene you're looking at and stops the cycling. You can also keep stepping with 3 taps + hold until you reach off.
- **To take a scene out of the cycle:** unmark it with 2 taps + hold.
- **Saved:** the step you choose is kept when the badge is powered off.

### LED Scenes

| # | Scenes |
| --- | --- |
| 1–15 | Beelzebub Revealed, Lord of the Flies, Night of Darkness, The Shadows, Maybe Today Beelzebub, Fly Beelzebub, Ancient Gods, Forces of Darkness, Seven Princes, Night of Discord, The Hive, Plague of Flies, Brimstone, Compound Eyes, Carrion Feast |
| 16–25 (slow) | Dreaming Hive, Stagnant Swamp, Sunken Fire, Wing Shimmer, Night Flight, Fever Dream, The Watcher, Crimson Stare, Beelzebub Awakens, Rotting Sunset |
| 26–33 (special effects) | Wingbeat, The Swarm, Plague Storm, The Twin Princes, Pulse of the Hive, Sweeping Wings, Beelzebub Stirs, Midnight Radar |
| 34 | CUSTOM (your own theme.. ) |

Scene 24, Beelzebub Awakens, is the one a new or reset badge starts on.

Every scene has its own speed: the creepy scenes drift slowly and the angry ones are brisk. Some scenes also have extra effects built in:

- **Twinkle:** random LEDs flare like sparks.
- **Bursts:** now and then the animation races at up to twice its speed for a couple of seconds, then settles back.
- **Swell:** the animation speeds up and slows down every 5 s or so.
- **Eye flares:** in scenes that use the Flare animation on the eyes, the eyes stay lit and flash white twice, with a pause in between.
- **Lightning:** scenes 4 (The Shadows) and 28 (Plague Storm), on the left wing. It strikes when you pick the scene, then once every 2–4 minutes. In the 5-tap Hellstorm show it strikes often.

### Menus

- Short press changes the value on the current page.
- Hold goes to the next page. After the last page the menu closes and your choices are saved.
- If you press nothing for 15 seconds, the menu closes and saves.

### Theme Menu (Hold 0.8 sec)

This menu changes CUSTOM only. On entry, a row of blue LEDs shows the page number (every 5th LED is magenta to help you count). The block (ex: Eye LEDs) you're editing stays bright and the others dim.

When you enter the CUSTOM menu, the current scene is copied into the CUSTOM slot. You can then edit it directly and it will automatically save.

Use a short press to move through the menu options. For instance, if on Page 1 (eyes palette) you'll see the eye LED colors change as you cycle through them via short button presses. To move to the next page, hold the button (0.8 seconds). All pages are listed below. There is a page for each LED block color and animation / effects. So you'll edit each one individually as you step through the pages.

| Page | Edits |
| --- | --- |
| 1 | Eyes palette |
| 2 | Eyes animation |
| 3 | Thorax palette |
| 4 | Thorax animation |
| 5 | Left wing palette |
| 6 | Left wing animation |
| 7 | Right wing palette |
| 8 | Right wing animation |

- Palette pages show the palette as a steady swatch, one color per LED.
- Animation pages play the animation live.
- Choosing Storm (lightning) fires one strike straight away so you can see it took. In normal running it still strikes only every few minutes.
- The extra effects (twinkle, bursts, swell) aren't menu choices. CUSTOM keeps the ones from the scene you started from.

#### Palette options (press to step through; wraps back to 1)

1. Lord of the Flies: dead-flesh greens, bruise purple, black
2. Night of Darkness: bright to deep red, a hot red flash
3. The Shadows: purples fading to dark crimson
4. Maybe Today, Beelzebub: acid greens, then red, with black between
5. Fly, Beelzebub: deep blues fading to dark crimson
6. Ancient Gods: old bone, dark bone, dried blood, black
7. Forces of Darkness: furnace oranges and reds
8. Seven Princes: red, orange, black, sick green, dark red
9. Beelzebub Revealed: violet and blood red, black between
10. Night of Discord: cold spectral blue-violet, black
11. The Hive: acid yellow-green and violet, black between
12. Plague: sickly yellow-greens and rot black
13. Brimstone: red, fire orange, sulphur yellow
14. Compound Eye: ember, red, amber, dark red, gold
15. Iridescent: violet and ghostly green flames
16. Carrion: blood red with black gaps, wine

#### Animation options (press to step through; wraps back to 1)

1. Drift: the scrolling palette
2. Sparkle: a dim glow with bright sparks popping at random
3. Storm: a dim glow with lightning (a strike every few minutes)
4. Ember: smooth random flicker, like glowing embers
5. Crossfade: the block slowly melts from one palette color into the next
6. Currents: a smooth gradient of palette colors slowly flows along
7. Gaze: slowly brightens, holds, slowly dims (never fully off; the eyes take turns)
8. Ripple: soft rings spread out from the base of the wings
9. Dim: a dim, slowly breathing glow (keeps the eyes standing out)
10. Radar: a bright beam sweeps back and forth round the base of the wings
11. Wingbeat: the whole wing flaps: light pulses out from the base to the tips
12. Web: a spider web: rings pulse out along glowing spokes
13. Whirl: two bright arms spin round the wing base
14. Dying: LEDs sputter out and come back, like failing lights
15. Spiral: a spiral of light winds out from the base of the wings
16. Chase: running lights: every third LED lit, crawling along the wing
17. Throb: the whole block throbs together, like a heart
18. Comet: a bright comet with a fading tail runs along the LEDs
19. Rise: embers rise up the badge from the abdomen
20. Candle: a flickering candle: each LED gutters at its own pace
21. Flare: the color stays lit and flashes white twice, with a pause in between

The eyes always melt smoothly from color to color and never go dark. Animations 5–8 also blend smoothly between the palette colors.

### Settings Menu (1 tap + hold)

These settings apply to all scenes, not just CUSTOM. On entry, a row of orange LEDs shows the page number. The value shows as a row of white LEDs, where 1 LED means the first level. As before, short press to cycle through each page option, and long press to go to the next page. For example, if you want to change the speed to fast you would: Long Press (enter Page 1 - brightness), Long Press (enter Page 2 - Eye Brightness), Long Press (enter Page 3 - Speed). Short Press to step from Normal Speed to Fast Speed. Long Press to exit menu or let it time out in 15 seconds.

| Page | Setting | Levels (default in bold) |
| --- | --- | --- |
| 1 | Brightness (all LEDs) | 1 = **brightest**, 2, 3, 4 = dimmest |
| 2 | Eye brightness (relative to the rest) | 1 = **same as the rest**, 2, 3, 4 = dimmest |
| 3 | Speed | 1 = slow, 2 = **normal**, 3 = fast |

### Power-up reset

1. Hold the button while plugging the badge in.
2. After 8 seconds the eyes turn red.
3. Let go. Everything resets to default, including favourites, CUSTOM and the settings, and the badge blinks green 3 times.

If you let go before 8 seconds, the badge just starts normally.

## Programming

The microcontroller is an Attiny814. You can program using the 3-pin sneaky connector (doesn't require soldering).

- Pin 1: Power
- Pin 2: UDPI
- Pin 3: Ground

There are multiple ways to program so if you already have your method then you are good to go. If you have not programmed an ATtiny with UDPI then I recommend following the instructions located here to get started: https://wolles-elektronikkiste.de/en/using-megatinycore.

If you use the method described here with Arduino, you do not need to connect a resistor in series with pin 6 because this is already installed on the board.

