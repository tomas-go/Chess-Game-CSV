# Chess Game CSV
## Overview
For this program all pieces of a chess game have been implemented with their movement and capture abilities. Moves that are illegal for the selected piece will not be done and will ask the user to enter a valid command. All movements are updated in "board.csv" using low-level reads and writes. 
## How to Play
All commands are to be made through the command line. 
### Commands
- show
    * `show`
    * Shows the current board.
- move
    * `mv x0 y0`
    * Moves a piece that the current player can use. x is the column letter of the player's piece, y is the column letter of the space to move the piece to, with 0 being each spaces respective row number. 
    * Ex: `mv a2 a4`
        * This moves the white pawn at a2 two spaces forward to a4.
- capture
    * `cp x0 y0`
    * 

