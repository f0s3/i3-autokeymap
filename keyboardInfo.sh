#!/bin/sh
setxkbmap -query | grep layout | cut -c 13-
