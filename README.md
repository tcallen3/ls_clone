# NAME

`ls` - list directory contents

# SYNOPSIS

`ls [-AacdFfhiklnqRrSstuw] [file...]`

# DESCRIPTION

This is intended to be a clone of the `ls(1)` utility supporting a subset
of the options provided by that program on NetBSD. It uses the `fts(3)`
library for filesystem traversal and its output is designed to largely
mimic the corresponding output of the system `ls` utility for the subset
of options supported.

One noticeable difference versus system `ls` is that this implementation 
always separates output entries using newlines, to avoid some complexity
in format wrangling. Less immediately obvious, but more significantly,
this version does not support text locales or other cross-region
portability features, so it is limited to US-ASCII systems.

# NOTES

The output is affected by the `BLOCKSIZE` and `TZ` environment variables,
analogously to the behavior of system `ls`.

The supported options and general structure are derived from Jan Schaumann's
CS 631 course, [Advanced Programming in the UNIX Environment](https://stevens.netmeister.org/631/).

# KNOWN ISSUES

Unlike system `ls`, this program does not print a blocksize summary for 
each directory in `-l` mode. The current implementation with `fts` makes
this somewhat tricky and there is no plan to modify the current behavior.

A subtle difference is that system `ls` does not appear to compare file 
times down to the full precision available. At a guess, it looks down to 
second or millisecond times and then groups two files equal if they match 
to this precision. This implementation does use the full precision, which 
means file times very rarely collide. The main observable effect is a 
different ordering of filenames in output when there are a number of files 
that were all modified/accessed within a short span, as is typical for 
operations performed programmatically.
