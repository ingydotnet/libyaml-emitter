#!/bin/bash

got=$(mktemp)

count=0
for test in test/*.events; do
  want=${test//events/yaml}
  rc=0
  ./libyaml-emitter $test > $got || rc=$?
  if [[ $rc -ne 0 ]]; then
    echo "not ok $((++count)) - Error code $rc"
    continue
  fi
  rc=0
  diff=$(diff -u $want $got) || rc=$?
  if [[ $rc -eq 0 ]]; then
    echo "ok $((++count))"
  else
    echo "not ok $((++count))"
    diff=${diff//$'\n'/$'\n'# }
    echo "# $diff"
  fi
done

echo "1..$count"
