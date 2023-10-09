# Tic-Tac-Toe-Solver

Tic-Tac-Toe is a simple game and can be easily solved with AI. Just take a look online at the hundreds of tutorials. My goal is to create a repository of solving algorithms for all variations of Tic-Tac-Toe. I will probably be unable to solve all of them, but I plan to do at least several more. Here are the current games and solutions:

3x3 and 4x4:

  The solution to 3x3 was trivial. The basic minimax algorithm was more than fast enough to solve it. However, 4x4 presented a much harder challenge. It was an order of magnitude more complex and required an adapted solution. I implemented alpha-beta pruning and a transposition table to help speed the code up to the point where it could solve the game. It was able to solve the game in about 15 secs, which I felt was a little slow. To make it more user friendly I implemented a beginning move dictionary to help speed up the first few calculations.

  Note: the rules of 4x4 tic tac toe are somewhat odd, follow this link to learn them: https://mamabeefromthehive.blogspot.com/2012/01/4-square-tic-tac-toe.html.

3x3-moveable:

  This is a game where each player plays with three pieces and moves their least recently used piece to a new position. Due to move order being important, I decided to remove the tranposition table. While I think that it should be possible to implement, the transposition table was giving me quite a few problems so I chose to get rid of it. To make up for the speed loss, I implemented a null window search. In the end, the algorithm was easily fast enough to solve the game and should be fun to play against.

5x5:

  The biggest challenge of them all. This algorithm is easily the most sophisticated, and is the most recent of all the algorithms. It uses Negamax with a null window search and an LRUCache for a transposition table. This code isn't fast enough to search through the entire game, so it uses iterative deepening to search within a given time limit. It also employs heuristics to play more towards the center of the board in the early game. I believe that this code could be improved in numerous ways, maybe even to the point where it could solve the game.
  Possible future improvements:
  
      - Eliminate board symmetries (especially in the early game)
      - Binary representation of game states (bitboard)
      - Anticipate losing moves (For more info: https://blog.gamesolver.org/solving-connect-four/09-anticipate-losing-moves/)
