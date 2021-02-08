#   GOSTBOX -- A 95-byte program for the 256-byte Streebog/Kuznyechik S-Box.

[gostbox.com](https://github.com/mjosaarinen/pi86/blob/main/gostbox.com)
is a 95-byte DOS program for generating the 256-byte GOST
Standard S-BOX used in Hash Function "Streebog" ( GOST R 34.11-2012,
[RFC 6986](https://tools.ietf.org/html/rfc6986#section-6.2) )
and Block Cipher "Kuznyechik" ( GOST R 34.12-2015,
[RFC 7801](https://tools.ietf.org/html/rfc7801#section-4.1) ).
Note that the "old" DES-rival GOST cipher is still alive, nowadays called
"Magma" (which was apparently a code name used for it in the KGB days).
Streebog and Kuznyechik are unrelated to it.

##  Why? About the S-Box

The actual design process of the Russian S-Boxes has not been published.
The decomposition used here is based on the ``TKLog'' representation
derived (reverse engineered) by Léo Perrin:
[Partitions in the S-Box of Streebog and Kuznyechik](https://doi.org/10.13154/tosc.v2019.i1.302-329)
IACR Transactions on Symmetric Cryptology, 2019(1), 302–329, 2019.

Russian [TK26](https://tc26.ru/) cryptographers have
[reported](https://cdn.virgilsecurity.com/assets/docs/memo-on-kuznyechik-s-box.pdf)
and
[maintained](https://cdn.virgilsecurity.com/assets/docs/meeting-report-for-the-discussion-on-kuznyechik-and-streebog.pdf)
that a pseudo-random process was used during design and that the S-Box
has no secret structure.

One can take "pseudo-random" to mean many things, but the fact that a
program of about 1/3 of the size of the S-Box can generate it shows that
it cannot be random. While 8086/DOS is obviously a suboptimal encoding,
it is not an artificial "language" and is certainly devoid of any advanced
features. One can see this as an application of the relation of
[program-size complexity](https://en.wikipedia.org/wiki/Kolmogorov_complexity)
a.k.a. Solomonoff-Kolmogorov-Chaitin complexity to information-theoretic
entropy for the purposes of demonstrating "non-randomness".

By contrast, the structure of the nonlinear components of the Belgian/US NIST
[AES](https://doi.org/10.1007/978-3-662-04722-4) and
Chinese GM/T standard [SM4](http://www.sicris.cn/CN/Y2016/V2/I11/995)
algorithms were known from the outset to be based on finite field inversion
([Nyberg's S-Boxes](https://doi.org/10.1007/3-540-48285-7_6) provide
good resistance against differential and linear cryptanalysis).
Pseudo-randomness has never been claimed for these, and various compact
representations are possible, thanks to clear their algebraic structure.
In fact, one can convert one to another and implement
[SM4 using AES instructions](https://github.com/mjosaarinen/sm4ni)
for speed and resistance against timing attacks.


##  Source Code

I usually write assembler code for RISC-V and ARM targets, but the good old
8086 of original IBM PCs had an amazing code density and backward
compatibility with 8-bit [8080](https://en.wikipedia.org/wiki/Intel_8080)
from 1974. This 8/16-bit code runs out-of-box on Microsoft operating
systems up to about Windows XP, but nowadays with 64-bit Windows and Linux
targets I'd recommend [dosbox](https://www.dosbox.com/).

The file [gostbox.asm](gostbox.asm) contains NASM (16-bit) assembler source
for the DOS executable
[gostbox.com](https://github.com/mjosaarinen/pi86/blob/main/gostbox.com),
while [tklog.c](tklog.c) has a more readable source code for
the decomposition used. The file [sbox256.dat](sbox256.dat) can be used
for comparison.

On Ubuntu or Debian Linux (regardless of CPU platform) all the prerequisites
can be installed with:
```console
$ sudo apt install dosbox nasm gcc make
```

Compiling (in case you've touched `gostbox.asm`):
```console
$ make
gcc -Wall -Wextra -O2 -g -c tklog.c -o tklog.o
nasm -f bin gostbox.asm -o gostbox.com  
gcc -Wall -Wextra -O2 -g -o xtest tklog.o 
````

To run it, just pipe/redirect (DOS `>`) output to a file.
```console
$ dosbox gostbox.com
```console
Will spawn a new window and execute the program, producing gibberish.
You can redirect it to a file for comparison with `sbox256.dat`
```console
C:\>gostbox > my.box
C:\>exit
```console

Now you shoold have a file `MY.BOX` (note the case) back on local machine
which you can verify:
```console
$ cmp MY.BOX sbox256.dat 
$ hexdump -C  MY.BOX
```

##  Rot-Differential

In hindsight, the Streebog/Kuznyechik S-Box is easily distinguishable
from random due to its logarithmic structure. Consider the difference
```
d = Pi[x] xor Pi[x <<< 1]
```
That is, d is the xor-difference between f(x) and f(y) where y is
x cyclically rotated left by 1 bit.
In case of the Streebog/Kuznyechik S-Box, in 56/256 = 21.9% of cases we
have d = 0x12 and in 34/256 = 13.2% cases d = 0x34. AES has no "d" value
more than 3/256 times, while for SM4 the maximum is 4/256.

This does not mean that Streebog or Kuznyechik are broken, but it
simply that the S-Boxes do not benefit from protection against algebraic
attacks that a random S-Box was supposed to give them.

The [tklog.c](tklog.c) code tests two decompositions of the S-Box and
prints the rot-differential table out as well.

```console
$ ./xtest 
gostbox distance = 0
tklog distance = 0

Rotational differentials	S[x) ^ S[x <<< 1]:
 00 33 12 20 34 94 12 12 12 34 69 43 34 34 10 12
 10 12 12 34 12 12 12 34 12 34 12 34 12 20 20 12
 12 34 8F 12 10 34 12 EF 34 12 34 34 34 12 12 12
 10 12 12 34 10 12 12 12 34 12 12 12 12 12 10 12
 34 10 12 12 3D 1E 34 12 12 C6 12 12 21 12 80 C6
 12 10 10 34 12 34 12 34 12 34 34 12 10 12 34 12
 12 12 10 34 20 10 12 20 12 12 34 97 10 34 78 34
 12 12 34 34 34 10 34 12 34 20 20 34 12 12 34 34
 31 E4 4A BF FA 72 86 84 A0 A2 F8 4C 4A 99 74 C2
 F8 55 B0 FF 66 D6 B1 F3 58 46 42 F0 45 AB DE 55
 93 C6 A8 D8 34 A2 58 88 6A EE 34 90 16 D8 91 C6
 0C 6C 3D 55 B6 31 CB 9E F9 9E 90 03 3D 55 28 6C
 F2 F1 1B 89 FF 5A 3F F4 3F E9 FF 58 60 89 D4 C3
 76 68 83 0D F4 BD 67 BF C8 C1 F4 BF 69 A5 50 5A
 78 31 CB 53 1A CF D0 D2 D0 D2 1A 2E ED 75 B4 33
 28 02 5F 89 7D 24 51 92 77 B4 5B 02 5F 89 28 00
```

##  Acknowledgments

Thanks to [Léo Perrin](https://who.paris.inria.fr/Leo.Perrin/) and Sean Curran.

