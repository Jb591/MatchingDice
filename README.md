# Matching Dice

In this project, I am going to build “Matching Dice” game. Matching dice is a simple dice game with one dealer, 4 players, and two dice. The 4 players (let’s call them A, B, C and D) work in teams where A and C are on the same team and B and D are on the other team. The goal of the game is to have both players on the same team get the same sum. The winning team is the one in which a player scores the same sum as their partner’s last toss. The Dealer gives the dice to an initial player selected at random and players take turns throwing the dice in the order A, B, C and D. The dealer then waits until one player scores the same sum as their partner’s last toss and declares the winning team. Initially, the dealer waits for the players to be ready, hands the dice to the initial player and the game proceed from that player going around.

## Implementation

The main function should create a thread for the dealer and 4 threads for the players (total of 5 threads). I used pthreads semaphores and conditions variables (e.g., to make the dealer wait on the condition that a player scored the same sum). Notice that we want to keep the threads synchronized and to protect any shared objects. The main program takes a seed as an argument for the random number generation which will be used to randomize the dice throwing process).

## Makefile

I have included a makefile with the following instructions :

`make`

`make clean`

`make run`

* make compiles all of the code
* make clean removes all of the files and replaces all .txt files
* make run runs the program

### Installation

Clone this repo for easy access

`git clone https://github.com/Jb591/MatchingDice.git`
