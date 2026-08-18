*This project has been created as part of the 42 curriculum by cclarke.*

# Codexion

### Description
The purpose of this project is the learn how to programme with threads and to avoid such related
complications as deadlock and data races. It specifically involves a number of 'coders' (threads) who must
compile, debug and refator within a given amount of time (in milliseconds), for fear of 'burning out', as
well as wait for a dongle to cooldown after having been used by a previous coder (the cooldown also being
in milliseconds). There are as many dongles as coders, hence the problems concerning deadlock and burnout.
### Instruction
`make` creates the programme, which is then executed as follows: `./codexion 4 1000 300 200 200 5 50 fifo`.
In order, the arguments are `number_of_coders`, `time_to_burnout`, `time_to_compile`, `time_to_debug`,
`time_to_refactor`, `number_of_compiles_required`, `dongle_cooldown`, `scheduler`, the last being
necessarily one of two keywords: `fifo` (first in, first out) and `edf` (earliest deadline first).
Furthermore, `make clean` will remove all `.o` files, `make fclean` will remove all `.o` files as well as
the executable, and `make re` will recompile the executable with brand new `.o` files.
### Resources
The only resources I used were the videos *Introduction to threads, mutex_locks and race conditions in C,
ELI5* and *The dining Philosophers in C: threads, race conditions and deadlocks* both by Oceano. AI was
used in order to explain certain concepts for my dumb brain to understand, debugging, and certain testing
scenarios.

### Additional Information
##### Blocking Cases Handled
Deadlock was prevented simply by assigning every first dongle to alternating hands; that is, rather than
having every coder take their left dongle first (thus causing a deadlock), every second coder will take
their right dongle first, the rest having their left dongle be the one taken first. This leaves at the
*very least* 2 coders to start compiling when there is an even number of coders, and 1 coder when their
number is odd.\
Coders are prevented from starving by way of correct queue implementation and the assumntion that
appropriate arguments are given. (Impossible numbers will thus mathematically necessitate burnout.)\
Dongle cooldowns were dealt with by putting coders to sleep, but only the amount of time that was left for
cooldown.\
Logs were properly serialised by way of mutexes.\
##### Thread Synchronization Mechanisms
Mutexes and conds were used basically everywhere necessary, which is a lot of places. Of especial help
was the implementation of 'getters' and 'setters', which are functions that return and set (in the case of
my programme) long and boolean values ('shared resources'), keeping them thread safe by being wrapped in
mutex-locks and -unlocks in the meantime.\
A particular example of race condition prevention is used with the `end_process` boolean value, which
every thread needs to check (using the abovementioned 'getter') multiple times in each one of its cycles
(e.g. after finishing a cycle, spending time compiling, or spending time waiting for dongle availability).
A coder burning out triggers the monitor thread to change the value of `end_process` to `true` (this time
using a 'setter'), which is why it is important that each coder checks it very often.\
I use conds for coders as well in order to have them wait before they are all finished being created. In the
`thread_job()` function, every coder waits at the preliminary while loop in which `pthread_cond_wait()` is
placed and the boolean `all_coders_ready` (also mutex protected) is the constituent of the conditional
clause. After every coder thread is done being created, `all_coders_ready`, protected by mutex, is set to
`true`, `pthread_cond_broadcast()` is executed, and each thread moves past the point.
