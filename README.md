# TLC

Typed Lua Compiler

## Build + Install

```sh
cd src
make install
```

## Usage

```sh
tlc --help                                # show help
tlc -o result.lua example.tl              # single file
tlc --config custom.conf -i src -o dst    # directory
```

## Gperf

```sh
gperf --no-strlen --compare-lengths config.gperf
gperf --no-strlen --compare-lengths args.gperf
```
