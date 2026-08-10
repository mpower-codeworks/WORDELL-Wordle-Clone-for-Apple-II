## Wordell: A Wordle Clone for Apple II
Wordell is a Wordle clone for Apple II written in C and assembly.
It features 2,315 possible unique games. It requires 64kb and an
80 column card. File system is ProDOS. The main target is Apple
//e. It may work on a ][+ with an 80 column card, but that
hasn't been tested.

## Screenshots

<table>
    <tr>
        <td align="left" width="50%" valign="middle">
            <img src="images/wordle-a2.jpg" width="100%" alt="wordle-a2"><br>
        </td>
        <td align="left" width="50%" valign="middle">
            <img src="images/wordle-game.jpg" width="100%" alt="wordle-game"><br>
        </td>
    </tr>
    <tr>
        <td align="left" width="50%" valign="middle">
            <img src="images/wordle-game2.jpg" width="100%" alt="wordle-game22"><br>
        </td>
        <td align="left" width="50%" valign="middle">
            <img src="images/wordle-game3.jpg" width="100%" alt="wordle-game3"><br>
        </td>
    </tr>
</table>

## How Wordell Works

Wordell uses two word lists:

- `ALL.TXT` - 12,947 words accepted as guesses.
- `SOLUTION.TXT` - 2,315 words that can be selected as answers.

The Apple II does not read these text files directly. They
are converted on a modern PC into fixed 5-byte records.

At startup Wordell opens `ALL5.BIN` and `SOL5.BIN`. Pressing **S**
starts a timer-based random seed. A solution number is selected and
exactly five bytes are read from `SOL5.BIN`.

Guess validation is kept small and fast. Only the matching first-letter
group from `ALL5.BIN` is loaded into RAM. `FASTWORD.S` then searches
that RAM buffer in 6502 assembly.

Hints use:

- `#` - correct letter and position
- `o` - correct letter, wrong position
- `_` - letter not in the word

Used letters are removed from the visible
alphabet as the game progresses.

### Player Stats

Player stats are recorded in `WORDLE.INI` at the end
of each game. These can be reset to zero at any time.

## Building the Word Files

You don't need to so this unless you with to make new
word/solution lists.

`MAKEPACK.C` is a Windows tool. It converts the normal
text word lists into the files used by the Apple II
version.

It reads:

```text
ALL.TXT
SOLUTION.TXT
```

It writes:

```text
ALL5.BIN
SOL5.BIN
WORDDATA.H
```

Each packed word is exactly five bytes. No
CR, LF, or zero terminator is stored.

`WORDDATA.H` is generated at the same time. It contains
the solution count plus the offset and size of each
starting-letter group in `ALL5.BIN`.

`ALL.TXT` must remain grouped by starting letter.
`MAKEPACK.C` packs the list as supplied; it does
not sort it.

### Build MAKEPACK with TCC

```bat
tcc makepack.c
```

This produces a very small `MAKEPACK.EXE`,
about 6kb with the current source.

### Build MAKEPACK with MSVC

From a Visual Studio Developer Command Prompt:

```bat
cl /nologo /O1 /Os /MD /GS- MAKEPACK.C /FeMAKEPACK.EXE /link /OPT:REF /OPT:ICF /INCREMENTAL:NO
```

The MSVC build is about 12kb with the current source.

TCC and CL produce the same data files. TCC is convenient
and tiny. MSVC is useful when TCC is not installed or when
building with the standard Microsoft toolchain.

Run `MAKEPACK.EXE` in the directory containing `ALL.TXT` and
`SOLUTION.TXT`. Copy the generated `ALL5.BIN` and `SOL5.BIN`
to the ProDOS disk with `WORDLE.SYSTEM`. `WORDDATA.H` stays
with the source and is compiled into Wordell.
