# Chess Game CSV
## Overview
For this program all pieces of a chess game have been implemented with their movement and capture abilities. Moves that are illegal for the selected piece will not be done and will ask the user to enter a valid command. All movements are updated in "board.csv" using low-level reads and writes. 
## How to Play
All commands are to be made through the command line. 
Whichever color captures the opponent's king wins.
### Commands
- show
    * `show`
    * Shows the current board.
- move
    * `mv x0 y0`
    * Moves a piece that the current player can use. x is the column letter of the player's piece, y is the column letter of the space to move the piece to, with 0 being each space's respective row number. 
    * Ex: `mv a2 a4`
        * On a fresh board, this moves the white pawn at a2 two spaces forward to a4.
- capture
    * `cp x0 y0`
    * Moves a piece the current player can use to a location with a piece belonging to the opponent. x is the column letter of the player's piece, y is the column letter of the opponent's piece, with 0 being each piece's respective row number. 
    * Ex: `cp h1 h7`
        * With h2-h6 being empty, the white rook at h1 would capture the black pawn at h7.
- skip
    * `skip`
    * Skips the current player's turn and switches to the opponent's turn.
- exit
    * `exit`
    * Ends the game.
## Features To Be Added
* Check - warning for when king is in danger of being captured.
* reset
    * `reset`
    * Resets the all pieces on board to starting position.
* Illegal moves for king when enemies are needed.

