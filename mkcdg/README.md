# MkCDG

_Simple tool to generate cdg files based on xml files_

Rendering is in part based on the [Karaoke Lyric Editor](https://github.com/gyunaev/karlyriceditor) by George Yunaev.


# Build

This needs the Fontconfig library (and development headers).

   cmake .
   make

This creates the `mkcdg` binary

# Running

Ignore `mkcdg --help` Metainfo should be included in the xml input file.

    mkcdg song.xml song.cdg
