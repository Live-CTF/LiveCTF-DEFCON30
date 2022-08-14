# LiveCTF 2022

This is an initial release after the end of the CTF. We will clean up the cobwebs after we get some rest :)

Repository for storing challenges, infrastructure notes, streaming configuration and other version controlled bits of information for the DEF CON CTF final LiveCTF event.


## Challenge structure

```
├── <challenge>
│   ├── challenge
│   │   ├── build.sh           # Commands for building challenge binary/binaries and handout
│   │   ├── Dockerfile         # Dockerfile for building challenge image
│   │   ├── flag               # If you don't know, you're probably in the wrong place
│   │   ├── run.sh             # Command(s) to start the challenge
│   │   └── src                # This is where the source for the challenge will go
│   │       └── challenge.c
│   └── solution
│       ├── requirements.txt   # Solution script requirements
│       └── solve.py           # Challenge solve script
```

## Building challenges
1. `make -C base-image`
2. `./livectf.py build <challenge>`

## Managing challenges
Challenge management (creation/building/starting/stopping) is done through `livectf.py`, which is a light wrapper around docker. See `./livectf.py --help` for usage.

### Adding a challenge
1. `./livectf.py create <challenge>` will copy the template challenge to a new challenge directory.
2. Add your challenge source in `<challenge>/challenge/src`
3. Change the flag in `<challenge>/challenge/flag`

(For challenges that include more than one source file + one binary):

4. If needed, edit `REQUIRED_PACKAGES` in `<challenge>/challenge/Dockerfile` to include any extra build/runtime dependencies
5. If needed, change the build commands in `<challenge>/challenge/build.sh`
6. If needed, change `HANDOUT_FILES` in `<challenge>/challenge/build.sh`


### Adding a solution for a challenge
1. Add any pip requirements to `<challenge>/solution/requirements.txt`
2. Put your solution script in `<challenge>/solution/solve.py`