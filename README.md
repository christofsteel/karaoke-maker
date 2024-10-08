# karaoke-maker

This is a set of programs I used to make simple karaoke (CDG) files.

## Workflow


### Requirements
You need the following:

  - A version of a song without the main lyrics. Use an AI vocal remover, play the instruments yourself, I don't care. We will call that file `instrumental.mp3`
  - The lyrics of the song.
  - (optional) A version of the song WITH the lyrics, that use the same timing as `instrumental.mp3`. We will call that file `song.mp3`

Format the lyrics of the song in a file (let's call it `lyrics.txt`) as described in the README of `mklyrics`.

### Building the XML

Run `mklyrics` (see README of `mklyrics` for detailed usage):

    python mklyrics.py -l lyrics.txt -m song.mp3 -s 0 -o lyrics.xml 

If you mess up at some point (let us say after ~x seconds), save your progress (`s`), and quit (`q`), and then edit your lyric file by removing the sections, 
you already synchronized correctly. Save it to `lyrics-starting-from-x.txt` and start again with

    python mklyrics.py -l lyrics-starting-from-x.txt -m song.mp3 -s x -o lyrics-starting-from-x.xml 

This will start the song at x seconds. You probably want to take a value of x, that is slightly before the paragraph you messed up.
The XML files won't get merged automatically, you need to copy over the ` <block> ` tags manually.

**Note**, `mklyrics` does not end after the last paragraph... just save (`s`) and quit (`q`) manually...

### Editing the XML

Once you have the correct timings, you can edit the metadata of the XML and fine tune it. The XML is structured in `block`s, `track`s, `line`s, `word`s, and `syllables`.
Let's ignore `tracks`, since this workflow does not generate multiple tracks. A block is everything you see for a slide at once. Lines, words and syllables should be 
self-explanatory.

You can fine tune `start` and `end` points of each syllable in milliseconds and add line breaks/block breaks by shifting the tags around.

You can add the following metadata to the `song` tag:

  - `artist`, the artist of the song
  - `title`, the title of the song
  - `fontname`, the name of the font, such that Fontconfig can identify it
  - `fontsize`, the size of the font in pixel
  - `titlecolor`, the color of the title
  - `bordercolor`, the color of the outer canvas
  - `bgtype`, either `color` (default) or `image`
  - `bg`, the color of the inner region (I would advise this to be the same color as `bordercolor`), if `bgtype` is `color` or a path to an image, that is set as the background.
  - `unsung`, the text color of the unsung part of the lyrics
  - `sung`, the text color of the sung part of the lyrics

Most metadata can also be specified at the block level. 

Colors are all in html/hex notation (e.g. #000000 for black)

### Generate CDG file

Now you can run `mkcdg`, to get your CDG file:

    ./mkcdg lyrics.xml song.cdg

### Playback

You can use `mpv` to play back the CDG files. If you use `--scale=oversample` as an option, you get that nice blocky rendering for CDG files:

    mpv song.cdg --audio-file=instrumental.mp3 --scale=oversample
