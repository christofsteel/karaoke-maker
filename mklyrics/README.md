# mklyrics

Generate timed xml lyric files, synchronized to a music file.

## Installation

You need to have `mpg123` installed and in your path. Additionally install all packages in `requirements.txt`.


## Input

Input files are simple text files with the lyrics. Each paragraph in the lyrics will generate a new slide. 
If you want to consider syllables in your output, you should annotate them by using the `|` char.

### Example

    Ma|ry had a lit|tle lamb,
    lit|tle lamb, lit|tle lamb
    Ma|ry had a lit|tle lamb
    Whose fleece was white as snow.

    And e|very|where that Ma|ry went
    Ma|ry went, Ma|ry went,
    E|very|where that Ma|ry went
    The lamb was sure to go.

## Running

You need a music file to play along the text. You can run `mklyrics` by

    python mklyrics -l lyrics.txt -m song.mp3 -s 0 -o lyrics.xml

This starts playing the song in the background. You now need to manually sync the lyrics by pressing the `space` bar at the correct times.
Each press of the `space` bar advances the lyrics by one syllable (or word, if that word only has one syllable). 
By default, the current syllable is hold until the next syllable starts, if a syllable ends before that 
(e.g. because of a musical break or a pause in the lyrics), you can signify that by pressing the `b` button, when the syllable ends.
Since rhythm is a complicated thing, and one tends to make a lot of mistakes, the lyric file is not saved automatically. 
If you want to save the current progress, you need to press the `s` key.
You can press the `q` key at any time, you want to quit.

The `-s 0` option means, that a song is played from the start. If you want to start at a later time, you can give an offset in seconds.
This is useful, if the song starts with a long intro, or you had an error in a previous run, but want to keep that start and start a new run 
shortly before you had a synchronization error. 

**Note:** Files are not automatically merged, they are simply overwritten. If you had an error previously, save the next run to a different xml file
and merge the two files my hand.

**Note 2**, `mklyrics` does not end after the last paragraph... just save (`s`) and quit (`q`) manually...


