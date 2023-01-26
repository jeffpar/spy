## SPY: A Custom v86-mode Debugger for the 80386

This is a resurrected version of a tool I wrote some 30 years ago to help debug issues with
DOS (or other real-mode x86 software), especially issues occurring during the boot process,
or issues that couldn't easily be debugged with a conventional debugger.  It effectively
reboots the machine in v86-mode, with the debugger running in protected-mode, waiting for a
hotkey (F12) or any unexpected faults, and interacts with the user via VGA and PC keyboard or
serial port.

Since this tool was never meant to be an actual product, it makes lots of assumptions,
does the bare minimum required to get the job done, probably contains a number of mistakes
and bugs, and will no doubt crash in a variety of circumstances.

But, that said, I still think it's a cool and potentially still useful piece of software.

### Overview

Operation is simple: on an 80386 machine, from a real-mode DOS prompt, run SPY.  Think of SPY
like a glorified TSR, except that instead of terminating-and-staying resident somewhere within
the first 640K, SPY switches to protected mode, relocates itself above 1Mb, saves the contents
of the screen, switches to its built-in debugger, and then waits for you to give it a command.

The SPY debugger's '?' command will give you a list of other commands.  At this point, you
would normally just type 'g', which returns execution back to the real-mode portion of SPY --
except that *now* the machine is running in v86-mode instead of real-mode.

You can press the F12 key at any time to jump back into the SPY debugger, inspect memory,
set breakpoints, etc.  You can even inspect the original screen using the 'v' command; pressing
any other key returns control to the debugger.  I have no recollection of how "complete"
the debugger is, and I'm sure I will find more bugs to fix as I begin using it -- all I've done
so far is get it loaded and launched into v86-mode successfully.

SPY assumes your machine has at least 2Mb of RAM.  Well, actually, it doesn't really care how
much memory you have *below* the 1Mb line (which could be 640K or less), but it does assume you
have at least 320K or so above the 1Mb line; most of that memory is used saving/restoring the
entire VGA state whenever entering/exiting the SPY debugger.

### The Tools

When I originally archived this project, I also saved a set of tools that can be used to
build SPY from scratch -- which was fortunate, because I'm not sure how easy it would be find
some of those tools today.

SPY is a bit unusual because it's a COM file with a mixture of 16-bit and 32-bit code, and
the 32-bit code is a mixture of assembly and C.  And except for the initialization code, it
does not use any operating system or ROM functions.  All screen, keyboard, and serial port
hardware operations are handled internally.

The C compiler, [CL3232](tools/), was an early internal version of Microsoft's 32-bit C compiler
that wasn't really designed for producing code in this environment -- a small flat memory model
where CS != DS.

On re-examination, the compiler didn't *appear* to make any assumptions about CS and DS,
with one exception: when generating code for *switch* statements, it would produce in-line
jump tables, and it would "JMP" through those tables *without* a CS override.  In other words,
there was an implicit assumption that CS == DS.

So, I wrote a crude little C program, [FIXASM](tools/FIXASM.C), that looks for those "JMP DWORD PTR"
instructions and inserts a "CS:" override.  This also meant changing the [MAKEFILE](MAKEFILE) to
have **CL3232** produce assembly files instead object files, running the assembly files through
**FIXASM**, and then assembling them with **ML**.  Problem solved.

[As an added bonus, using **CL3232** to produce assembly files instead of object files seems to have
eliminated some overhead, because the overall size of the COM file dropped by about 5K.  I haven't
looked into it yet, but I did verify that all DEBUG code, including asserts, was still in place.
Strange.]

There were a number of other minor problems, like some unexpected padding between code and
data sections, failure to preserve the initial interrupt mask registers (IMRs), and failure to
allocate memory for the VGA save/restore operations.  I call these problems "minor", but
they all took a while to track down.  Fortunately, the [PCjs Debugger](http://www.pcjs.org/machines/pcx86/compaq/deskpro386/vga/2048kb/debugger/machine.xml) was a big help.  I even had to use the VSCode debugger to debug a problem
with the PCjs debugger, so as an added bonus, I ended up fixing a few [PCjs bugs](https://github.com/jeffpar/pcjs/commit/a2d169129bc8727cd1739f5fa2de50196a1cc587#diff-f6421b18c663fde433cf56c0333dc0961b21e80dc4842aad8d4686452a3f866a) as well.

All these issues made a few things clear: I had probably used an *older* version of **CL3232**
than what was archived here (I'll keep looking for it, but I probably don't have it anymore), and
I had probably started reworking some of the SPY code in 1993 and never finished the changes.

In any case, SPY is once again operational -- or at least, really close.

![First Boot](images/First_Boot.jpg)

### Trivia

In the [VxD](vxd/) folder, I've included another variation of this tool that was designed to load as
a Windows 3.x/Windows 95 VxD.  I'm not sure how I found the time to work on these things while also
keeping up with all my Windows 95 and MS-DOS 6 development responsibilities.  Probably a lot of late nights.
