#/usr/bin/env bash
while :
do
    lrc=$(qdbus io.github.brucezhang1993.LyricsSeeker / io.github.brucezhang1993.LyricsSeeker.lyricsCurrentLine '')
    echo -ne "\r                                                                            "
    echo -ne "\r$lrc"
    sleep .3
done
