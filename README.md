Epic Survival Game Series
-------------------------

Third-person survival game for Unreal Engine 4 made entirely in C++. Originally built as a 6 section tutorial series, now available as open-source C++ sample project.

See [the main documentation page on the unreal engine Wiki](https://wiki.unrealengine.com/Survival_sample_game).

For questions & feedback visit [the official thread on the unreal engine forums](https://forums.unrealengine.com/showthread.php?63678-Upcoming-C-Gameplay-Example-Series-Making-a-Survival-Game)

**Looking for a step by step guide?** I released my **[Unreal Engine 4 Mastery: Create Multiplayer Games with C++](https://www.udemy.com/unrealengine-cpp/?couponCode=TLGH14)!** Which includes two games teaching you C++, Networking for multiplayer and multiple AI including advanced behavior trees for ranged shooter AI.

Get the Udemy Course now and learn C++ for Unreal Engine 4: [DISCOUNT LINK](https://www.udemy.com/unrealengine-cpp/?couponCode=TLGH14)


**Last updated for 4.24!**

### NEW: Mod Support
Includes two small mod examples including a Pink Rifle extension and Flashlight replacement mod. [Check out the Modding Sample Project for guidelines & cooker profile setup](https://wiki.unrealengine.com/Modding:_Adding_mod-support_to_your_Unreal_Engine_4_project)

Example commandline arg to load the Mod gamemode with the built-in level "CoopLandscape" in a cooked game build:
SurvivalGame.exe /Game/Maps/CoopLandscape_Map?game=/ExtendedRifleMod/SurvivalGameMode_PinkRifle.SurvivalGameMode_PinkRifle_C

There is currently no supporting UI to load up specific mod content, please note that the modding pipeline is a experimental and intended for early adopters only!

### Section 1
This section sets up the third person character movement with animation, object interaction, simple hunger system, all with networking support.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_1)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?64833-Announcing-Section-1-for-Survival-Game)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-1)

### Section 2
Adds weapon support for the character, a flashlight, UT-style inventory with on-character visual representation of the carried items and deals with damage, death and respawns for players.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_2)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?66263-Announcing-Section-2-for-Survival-Game)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-2)

### Section 3
Introduces AI "Zombie" enemy to our game using PawnSensing and Behavior Tree.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_3)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?67859-Announcing-Section-3-for-Survival-Game)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-3)

### Section 4
Introduces a gameloop with enemies, items, scoring and a time of day.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_4)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?69308-Announcing-Section-4-for-Survival-Game-Setting-up-the-survival-game-loop)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-4)

### Section 5
Introduces the ability to carry around objects like barriers and discusses game networking.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_5)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?71057-Announcing-Section-5-of-Survival-Game-Networking-your-game)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-5)

### Section 6
The final section in the series focuses on bug fixing and a bit of polish to the existing features. This section is compatible with the 4.8 release.

- [Documentation page](https://wiki.unrealengine.com/Survival_Sample_Game:_Section_6)
- [Forum Thread](https://forums.unrealengine.com/showthread.php?72313-Announcing-Section-6-of-Survival-Game-Adding-some-polish)
- [Branch on Github](https://github.com/tomlooman/EpicSurvivalGameSeries/tree/Section-6)

