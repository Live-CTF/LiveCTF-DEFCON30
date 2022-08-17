# LiveCTF 2022

Repository for storing challenges, infrastructure notes, streaming configuration and other version controlled bits of information for the DEF CON CTF final LiveCTF event.

## Challenges used in LiveCTF

| #                   | Challenge Name                                     | Competitors                            | Victor                    |
|---------------------|----------------------------------------------------|----------------------------------------|---------------------------|
| Round of 16: 1      | [syscall-me-maybe](./challenges/syscall-me-maybe)             | Shellphish vs PTB_WTL                  | Shellphish                |
| Round of 16: 2      | [open-to-interpretation](./challenges/open-to-interpretation) | StarBugs vs ./V /home/r/.bin/tw        | StarBugs                  |
| Round of 16: 3      | [nopcoing](./challenges/nopcoing)                             | OSUSEC vs Maple Mallard Magistrates    | Maple Mallard Magistrates |
| Round of 16: 4      | [nerd-sniped](./challenges/nerd-sniped)                       | the new organizers vs Balsn.217@TSJ.tw | the new organizers        |
| Round of 16: 5      | [fairy-nuff](./challenges/fairy-nuff)                         | DiceGuesser vs Sauercloud              | Sauercloud                |
| Round of 16: 6      | [seek-and-destroy](./challenges/seek-and-destroy)             | 侍‎ vs perfect r✪✪✪t                    | perfect r✪✪✪t             |
| Round of 16: 7      | [nvulns](./challenges/nvulns)                                 | r3kapig vs Water Paddler               | r3kapig                   |
| Round of 16: 8      | [quick-cast](./challenges/quick-cast)                         | Katzebin vs Straw Hat                  | Straw Hat                 |
| Quarter-Finals: 1   | [story-time](./challenges/story-time)                         | StarBugs vs the new organizers         | StarBugs                  |
| Quarter-Finals: 2   | [stacks](./challenges/stacks)                                 | Shellphish vs Maple Mallard Magistrates| Maple Mallard Magistrates |
| Quarter-Finals: 3   | [pacman](./challenges/pacman)                                 | r3kapig vs Sauercloud                  | [Went to sudden death]    |
| Quarter-Finals: 3.5 | [over-easy](./challenges/over-easy)                           | r3kapig vs Sauercloud                  | r3kapig                   |
| Quarter-Finals: 4   | [no-roplem](./challenges/no-roplem)                           | Straw Hat vs perfect r✪✪✪t             | perfect r✪✪✪t             |
| Semi-Finals: 1      | [aes-of-spades](./challenges/aes-of-spades)                   | Maple Mallard Magistrates vs StarBugs  | StarBugs                  |
| Semi-Finals: 2      | [loopy-brain](./challenges/loopy-brain)                       | r3kapig vs perfect r✪✪✪t               | [Went to sudden death]    |
| Semi-Finals: 2.5    | [fast-updater](./challenges/fast-updater)                     | r3kapig vs perfect r✪✪✪t               | perfect r✪✪✪t             |
| Finals              | [f-in-the-stack](./challenges/f-in-the-stack)                 | StarBugs vs perfect r✪✪✪t              | StarBugs                  |



## Challenge structure

```
├── <challenge>
│   ├── challenge
│   │   ├── build.sh           # Commands for building challenge binary/binaries and handout
│   │   ├── config.toml        # Config file for flag submitter
│   │   ├── Dockerfile         # Dockerfile for building challenge image
│   │   ├── run.sh             # Command(s) to start the challenge
│   │   └── src                # This is where the source for the challenge will go
│   │       └── challenge.c
│   └── solution
│       ├── requirements.txt   # Solution script requirements
│       └── solve.py           # Challenge solve script
```

## Building challenges
1. `./livectf.py build <challenge>`

## Managing challenges
Challenge management (creation/building/starting/stopping) is done through `livectf.py`, which is a light wrapper around docker. See `./livectf.py --help` for usage.

### Adding a challenge
1. `./livectf.py create <challenge>` will copy the template challenge to a new challenge directory.
2. Add your challenge source in `<challenge>/challenge/src`
3. Update the solution script in `<challenge>/solution/solve.py`

(For challenges that include more than one source file + one binary):

4. If needed, edit `REQUIRED_PACKAGES` in `<challenge>/challenge/Dockerfile` to include any extra build/runtime dependencies
5. If needed, change the build commands in `<challenge>/challenge/build.sh`
6. If needed, change `HANDOUT_FILES` in `<challenge>/challenge/build.sh`


### Adding a solution for a challenge
1. Add any pip requirements to `<challenge>/solution/requirements.txt`
2. Put your solution script in `<challenge>/solution/solve.py`
