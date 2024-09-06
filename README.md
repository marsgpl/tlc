# TLC

Typed Lua Compiler and Language Server

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
tlc --server                              # start as language server
```

## Gperf

```sh
gperf --no-strlen --compare-lengths args.gperf
gperf --no-strlen --compare-lengths config.gperf
gperf --no-strlen --compare-lengths tokens.gperf
```
