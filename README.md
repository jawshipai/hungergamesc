# hungergamesc
a hunger games simulator where you can put in a list of your friends and a list of wacky scenarios and get one winner! comes with it's own players.txt and scenarios.txt but feel free to write your own! more info in the README.txt
# ------------------
Welcome to Jawsh's Hunger Games Simulator!
This simulator requires 2 extra files to run:

1) "players.txt" : 
A list of players seperated by "\n" (make sure there's a new line at the end)

FILE EXAMPLE:
|"players.txt"|
|-------------|
|Alice        |
|Bob          |
|Charlie      |
|Dave         |
|             | 

^^ notice the last line is blank!

***NOTE: I plan to make this more modular later so you may play out cases
where one player starts with much more advantage than the others!***


2) "scenarios.txt" :
A list of scenarios seperated by "\n" (make sure there's a new line at the end)
This is a bit more complicated but once you get it, it'll be fine!
Because this is the first version these are the only features:
The format goes as follows:

<number-of-participants>#<scenario-with-syntax-and-arbitrary-text><NEW-LINE>

The number of participants is merely for the code to easily access how many
players are in the scenario.

Please keep in mind that the text aside from your scenario-syntax that you 
include in your scenario doesn't change the functionality!

Scenario Syntax - Player References:
"@r" - indicates a random player that isn't added to a scenario pool
"@p" - indicates a random player added to the scenario pool
"@w" - indicates a winner of a scenario from the pool players
"@l" - indicates a random loser that is eliminated

EXAMPLE 1:
"1#@r is wandering around..." :
    "1#" means this scenario has 1 participant
    "@r" takes a random player from the alive players

EXAMPLE 2:
"2#@p is wandering around and @p does a sneak attack! They fight and @w wins!"
    "2#" means this scenario has 2 participants
    "@p" twice, means there are two players added to the pool
    "@w" means the 2 pool players fight, and there is one winner!

EXAMPLE 3:
"1#@l fell into a ditch and got eliminated!"
    "1#" means this scenario has 1 participant
    "@l" means there is 1 player who is eliminated

FILE EXAMPLE:
|"scenarios.txt"|
|-------------|
|1#@r is wandering around...        |
|2#@p is wandering around and @p does a sneak attack! They fight and @w wins!          |
|1#@l fell into a ditch and got eliminated!      |
|             | 

^^ notice the last line is blank!

Right now every player has an advantage of 100 and cannot raise or lower it but in the future
scenarios will beable to modify the players 
