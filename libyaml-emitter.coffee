fs = require 'fs'
lines = fs.readFileSync('/dev/stdin').toString().split "\n"
lines.pop()
say = console.log
die = (msg)->
  say msg
  process.exit 1
line = ''

main = ->
  do_stream()

do_stream = ->
  readline '+STR'
  while line != '-STR'
    do_document()
    readline()
  readline 'EOS'

do_document = ->
  readline '+DOC'
  if line.match /---/
    emit '---'
  readline()
  do_node()
  if line.match /\.\.\./
    emit '...'
  assert '-DOC'

do_node = ->
  if line.match /^\+MAP/
    do_map()
  else if line.match /^\+SEQ/
    do_seq()
  else if line.match /^=VAL/
    do_scalar()
  else if line.match /^=ALI/
    do_alias()
  else
    die "Expected new node event but got '#{line}"
  readline()

do_map = ->
  emit '{'
  readline()
  while not line.match /^-MAP/
    do_node()
    emit ':'
    do_node()
    emit ','
  emit '}'

do_seq = ->
  emit '['
  readline()
  while not line.match '-SEQ'
    do_node()
    emit ','
  emit ']'

do_scalar = ->
  m = line.match /^=VAL(?: &(\w+))?(?: <([^>]+)>)? (['"|>:])(.*)/ ||
    die "Invalid scalar event format: '#{line}'"
  [dummy, anchor, tag, kind, value] = m
  anchor ||= ''
  tag ||= ''
  emit "&#{anchor} <#{tag}> #{kind}#{value}"

do_alias = ->
  assert '=ALI'
  m = line.match /^=ALI(?: \*(\w+))/ ||
    die "Invalid scalar event format: '#{line}'"
  [dummy, anchor] = m
  emit "*#{anchor}"

readline = (event)->
  if event == 'EOS'
    if not eos()
      die "Expected EOS but more events to process"
    else
      return
  if eos() and event != 'EOS'
    die "Expected 'EOS' event but got '#{lines[0]}'"
  line = lines.shift()
  if event?
    assert event

emit = (text)->
  say text

eos = ->
  Boolean not lines.length

assert = (event)->
  regex=event.replace /^\+/, '\\+'
  if not line.match ///^#{regex}///
    die "Expected '#{event}' but got '#{line}"

main()
