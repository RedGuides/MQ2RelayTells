---
tags:
  - command
---

# /rtsetkey

## Syntax

<!--cmd-syntax-start-->
```eqcommand
/rtsetkey <section> <key> <value>
```
<!--cmd-syntax-end-->

## Description

<!--cmd-desc-start-->
Change any entry in the INI file. This is handy if you want to quickly change the relay channel for your characters, e.g. `/rtsetkey Settings ChatChannel "/tell bobby"` (if the value has a space, you must use quotes)
<!--cmd-desc-end-->

## Examples

- Relay all tells to a group text in dannet,  
`/rtsetkey Settings ChatChannel "/dgtell groupname"`