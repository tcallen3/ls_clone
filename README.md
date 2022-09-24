# NAME

`ls` - list directory contents

# SYNOPSIS

`ls [-AacdFfhiklnqRrSstuw] [file...]`

# DESCRIPTION

This is intended to be a clone of the `ls` utility supporting a subset
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

