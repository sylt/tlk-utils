# tlk-utils
Basic utilities for manipulating TLK (dialogue) files used by Bioware Infinity Engine. Some games that uses TLK files are:

* Baldur's Gate
* Baldur's Gate 2
* Neverwinter Nights
* Neverwinter Nights 2
* Mass Effect 2
* Star Wars: Knights of the Old Republic

# Building

```
$ mkdir -p build
$ cd build
$ cmake ..
$ make
```

# Utilities

* **tlkview**: Used to view all entries or a specific entry in a TLK file.
* **tlkreplace**: Used to replace the contents of a specific TLK file entry with something else.
* **tlkcombine**: Used to combine the dialogue of two TLK files into one. The primary use of this is to combine two dialogue files of separate languages. For example, if one were to combine English and Spanish, the resulting dialogue file would contain entries looking like: "Selecciona la apariencia de tu personaje (Select the Appearance of your Character)".

# Sample usage of tlkcombine

Combining language files are as simple as running:

```
./tlkcombine dialog-english.tlk dialog-spanish.tlk my-output-file.tlk
```

This will produce the file `my-output-file.tlk` which you can copy to your game directory as `dialog.tlk`. Just make sure you have your original `dialog.tlk` file backed up. Female dialogue require the dialogf.tlk file to be present, otherwise it will fall back to the dialog.tlk file.

If you also add the `-l` flag to the tlkcombine command, you will know what lines the program had problems with interleaving. These lines might require manual editing (i.e., use tlkview + tlkreplace).

# Language files created with help of this project:

Neverwinter Nights 1 language interleaves/combinations:
* [Spanish/English](https://neverwintervault.org/project/nwn1/other/spanishenglish-interleaved-dialogue-language-learners)
* [French/English](https://neverwintervault.org/project/nwn1/other/frenchenglish-interleaved-dialogue-language-learners)
* [Italian/English](https://neverwintervault.org/project/nwn1/other/italianenglish-interleaved-dialogue-language-learners)
* [German/English](https://neverwintervault.org/project/nwn1/other/germanenglish-interleaved-dialogue-language-learners)
* [Japanese/English](https://neverwintervault.org/project/nwn1/other/japaneseenglish-interleaved-dialogue-language-learners)


