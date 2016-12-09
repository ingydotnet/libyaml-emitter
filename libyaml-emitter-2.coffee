fs = require 'fs'
lines = fs.readFileSync('/dev/stdin').toString().split "\n"
lines.pop()
say = console.log
die = (msg)->
  say msg
  process.exit 1

main = ->
  while (line = lines.shift())?
    if line.match /^[+-]STR/
      null
    else if line.match /\+DOC/
      if line.match /---/
        emit '---'
    else if line.match /\-DOC/
      if line.match /\.\.\./
        emit '...'
    else if line.match /\+MAP/
      emit '{'
    else if line.match /-MAP/
      emit '}'
    else if line.match /\+SEQ/
      emit '['
    else if line.match /-SEQ/
      emit ']'
    else if line.match /=VAL/
      m = line.match /^=VAL(?: &(\w+))?(?: <([^>]+)>)? (['"|>:])(.*)/ ||
        die "Invalid scalar event format: '#{line}'"
      [dummy, anchor, tag, kind, value] = m
      anchor ||= ''
      tag ||= ''
      emit "&#{anchor} <#{tag}> #{kind}#{value}"
    else if line.match /=ALI/
      m = line.match /^=ALI(?: \*(\w+))/ ||
        die "Invalid scalar event format: '#{line}'"
      [dummy, anchor] = m
      emit "*#{anchor}"
    else
      die ">>#{line}<<"

emit = (text)->
  say text

main()
