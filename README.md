# Blitz

## Current Development | Compiler v0.1

- Takes input file to a bf program, and an output file to write intel x64 assembler
- Unmatched closing bracket detection
- Uses combination optimization
- Only allocates 1kb max memory

## End Goal

- JIT compiler
- web interface instead of stdin and stdout
- aggressive optimization for common patterns
- robust error handling and logging
- easy configuration
- maybe higher level language support
    - or ensure support for other projects such as [brain](https://github.com/brain-lang/brain)

## Pipeline Plan

### Development

1) Generate tokenized assembly style view
2) Convert to structured hex format

### Execution

1) Tokenize input
2) Allocate and map memory
3) Compile and write tokens
4) Jump

## Optimizations

| Name        | Discription                                                          | Support | Future Work                                        |
|-------------|----------------------------------------------------------------------|---------|----------------------------------------------------|
| combination | combine operators into block operations                              | GOOD    | none                                               |
| null        | set current cell to zero                                             | NONE    | detect pattern [-]                                 |
| multiply    | impliment multiplication / copy pattern natively                     | NONE    | detect multiplication / copy pattern [y(x>x+)yx<-] |
| io          | efficently handle safe input / output of c style strings             | NONE    | detect [.>] and ,[>,]                              |
| slide       | efficently slide along nonzero data                                  | NONE    | detect [>] and [<]                                 |
| compare     | compare cells or memory blocks                                       | NONE    | unsure                                             |
| search      | search for specific value                                            | NONE    | unsure                                             |
| refactor    | find and analyze 'closed sections' and refactor with high level view | NONE    | unsure                                             |
