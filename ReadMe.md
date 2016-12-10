libyaml-emitter
===============

Emitter CLI for libyaml

# Try it Now with Docker:

```
curl -s https://raw.githubusercontent.com/ingydotnet/libyaml-emitter/master/test/example-2.27-invoice.events | docker run -iv $PWD:/cwd ingy/libyaml-emitter
```

# Synopsis

```
git clone https://github.com/ingydotnet/libyaml-emitter
cd libyaml-emitter
make build
make test
```

# Usage

Print the YAML for a libyaml-parser events file (or stdin):

```
./libyaml-emitter file.events
./libyaml-emitter < file.events
cat file.events | ./libyaml-emitter
```

Run with Docker:

```
alias my-yaml-emitter='docker run -iv $PWD:/cwd ingy/libyaml-emitter'
my-yaml-emitter file.events
my-yaml-emitter < file.events
cat file.yaml | my-yaml-emitter
```

# Build

## Native Build

```
make build
```

## Docker Image Build

```
DOCKER_USER=ingy make docker
```
