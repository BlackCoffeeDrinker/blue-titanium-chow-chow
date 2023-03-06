# For fun, Game Engine
## Folder structure
 - Engine
   - The main game engine, shouldn't be modified per game
   - Normally shipped as headers and a static/shared library
 - Game_Light
   - The game data and code this is modified per game
 - Platform_*
   - Entry point
   - Platform code for routing OS events

## Engine Principals
TODO
* InputEvents
  * Generic input event created by a subsytem
* Actions
  * Things you can bind stuff to
  * Actions are things like RUN, USE, WALK_FORWARD, QUIT
  * Anything that has code that needs to run when the user does something
  * Bind InputEvents to Actions
