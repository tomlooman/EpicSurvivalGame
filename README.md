Epic Survival Game Series
-------------------------

**Last updated for 4.26!**

Third-person survival game for Unreal Engine 4 made entirely in C++. Originally built as a 6 section tutorial series, now available as open-source C++ sample project.

See [the main documentation page](https://www.tomlooman.com/survival-sample-game-for-ue4/) for walkthrough of many of the features.

![equipment header](https://www.tomlooman.com/wp-content/uploads/2015/04/section6_equipment03.jpg)

![animation blueprint graph example](https://www.tomlooman.com/wp-content/uploads/2015/04/section6_advancedanimbp031.jpg)

![Refreshed look for Landscape Map](https://www.tomlooman.com/wp-content/uploads/2021/01/survivalgame_refresh_05.jpg)

For questions & feedback visit [the official thread on the unreal engine forums](https://forums.unrealengine.com/showthread.php?63678-Upcoming-C-Gameplay-Example-Series-Making-a-Survival-Game)

**Looking for a step by step guide?** I released my **[Unreal Engine 4 Mastery: Create Multiplayer Games with C++](https://www.udemy.com/unrealengine-cpp/?couponCode=TLGH14)!** Which includes two games teaching you C++, Networking for multiplayer and multiple AI including advanced behavior trees for ranged shooter AI.

Get the Udemy Course now and learn C++ for Unreal Engine 4: [DISCOUNT LINK](https://www.udemy.com/unrealengine-cpp/?couponCode=TLGH14)

### Section 1
This section sets up the third person character movement with animation, object interaction, simple hunger system, all with networking support.

- [Documentation page](https://www.tomlooman.com/survival-sample-game-for-ue4/section-one/)

### Section 2
Adds weapon support for the character, a flashlight, UT-style inventory with on-character visual representation of the carried items and deals with damage, death and respawns for players.

- [Documentation page](https://www.tomlooman.com/survival-sample-game-for-ue4/section-two/)

### Section 3
Introduces AI "Zombie" enemy to our game using PawnSensing and Behavior Tree.

- [Documentation page](https://www.tomlooman.com/survival-sample-game-for-ue4/section-three/)

### Section 4
Introduces a gameloop with enemies, items, scoring and a time of day.

- [Documentation page](https://www.ue4community.wiki/Legacy/Survival_Sample_Game:_Section_4)

### Section 5
Introduces the ability to carry around objects like barriers and discusses game networking.

- [Documentation page](https://www.ue4community.wiki/Legacy/Survival_Sample_Game:_Section_5)

### Section 6
The final section in the series focuses on bug fixing and a bit of polish to the existing features. This section is compatible with the 4.8 release.

- [Documentation page](https://www.ue4community.wiki/Legacy/Survival_Sample_Game:_Section_6)

### NEW: Mod Support
Includes two small mod examples including a Pink Rifle extension and Flashlight replacement mod. [Check out the Modding Sample Project for guidelines & cooker profile setup](https://www.tomlooman.com/add-mod-support-to-your-unreal-engine-4-game/)

Example commandline arg to load the Mod gamemode with the built-in level "CoopLandscape" in a cooked game build:
SurvivalGame.exe /Game/Maps/CoopLandscape_Map?game=/ExtendedRifleMod/SurvivalGameMode_PinkRifle.SurvivalGameMode_PinkRifle_C

There is currently no supporting UI to load up specific mod content, please note that the modding pipeline is a experimental and intended for early adopters only!
