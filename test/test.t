#!/bin/bash

got=$(mktemp)

count=0
for test in test/*.yaml; do
  want=${test//yaml/events}
  ./libyaml-parser $test > $got
  rc=0
  diff=$(diff $want $got) || rc=$?
  if [[ $rc -eq 0 ]]; then
    echo "ok $((++count))"
  else
    echo "not ok $((++count))"
  fi
done

echo "1..$count"
