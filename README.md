# blinks-shared-task
> Finding a value and sharing it accross all connected blinks.

## How to play
Connect all your blinks (ideally 6 or less) together and tap one. Blue will fan out while they figure out how many of them there are and then they will all show that number in lit faces (up to 6...)


## Dev Note
Check out the `main.ino` comments for an explanation of whats going on. Ideally, you should be able to coppy `.distributed-task.h` and its cpp  over to your own project and use them similarly to the `main.ino` example. Here we are just counting blinks but in [Raid](https://github.com/mdm373/blinks-overtake) I used the same logic to tally up the scores for each player.
