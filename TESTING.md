# TESTING.md 
## CS50 Final Project 
## Team we_free 

### Test erraneous commandline arguments 
Our program wrote error messages to the stderr in each of these cases 
* invalid number of arguments 
* invalid number of avatars 
* invalid difficulty
* invalid hostname 


### Valid runs 
These results will be produced in a logfile in the Results directory 
Each logfile will be named according to the user, number of avatars and difficulty. 
They contain each move every avatar makes and if the avatar is successful - the `AM_SOLVED` message.  
We tested various avatar difficulties with varying levels of avatar 
  * this included testing every valid number of avatars and every maze difficulty through level 7
  * Our program was able to solve each of these test cases in a varying number of moves 
* Note: if running numerous test cases of higher difficulty, the server may send a disk quota error, and will stop logging to   the file. After 5 mintues, the server messages will be deleted and you may continue testing/logging 