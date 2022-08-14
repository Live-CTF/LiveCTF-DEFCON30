# Stacks #

This challenge is a DEF CON OpenCTF challenge from 2011 by richthofen that I
don't believe ever got used. I re-wrote it in 2014 for Ghost in the Shellcode,
but it didn't get used then, either. Maybe this time it'll see the light of day?
Also, hopefully people will find it interesting since it's kind of a time
capsule. We don't write pwnables like this anymore - they're generally a lot
more difficult!

I appear to have lost the solution script for this somewhere along the way, but
I seem to recall the intended solution being something like this:

1. Create a bunch of stacks using :new
2. Use :top to leak out their location on the heap (off-by-one will print the address)
3. Use operations to overwrite pointer in the stack struct to leak out the "stack canary"
4. Craft a custom stack that can be used to write to an address
5. Use the custom stack to write to something like the GOT
6. Trigger a jump back to your custom stack's data area
7. ??? (heap should be executable)
8. PROFIT!

There should hopefully be an alternative solution requiring a heap metadata
overwrite and using :del to potentially trigger a double-free or something as
well, but I don't believe that was ever tested or landed. If the above seems too
easy, you could see if maybe this will work instead. Otherwise, it'll either be
a red herring or a surprise conclusion!

Since this was originally intended to run on Ubuntu 14.04, there are probably
binary protections that get turned on that might ruin this. I've tried to turn
off as many as possible in the `Rakefile`, but I might've missed some. Please
make whatever changes are necessary to make this work! But, also, please turn
anything back on that can be. Don't want to make things *too* easy.

To build, simply do (requires `sudo apt-get install rake`):

```sh
rake debug
```

This'll turn off a few things in the executable to make things easier. You can
also insert any code you might need with `#ifdef _DEBUG` to make landing things
simpler.

Once you are ready to make the real deal, do:

```sh
rake release
```

This will compile out all of the `_DEBUG` code and strip the binary.

Good luck! Hope this was useful!
