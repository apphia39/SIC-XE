#  SIC/XE Machine
- project1 : Shell<br>
- project2 : Assembler<br>
- project3 : Linker&Loader
<br>

## How to compile
```  
$ make
```
<br>

## How to run
```  
$ ./main.out
```
<br>

## Allowed commands
- h[elp]<br>
- d[ir]<br>
- hi[story]<br>
- q[uit]<br>
- du[mp] [start, end]<br>
- e[dit] address, value<br>
- f[ill] start, end, value<br>
- reset<br>
- opcode mnemonic<br>
- opcodelist<br>
- assemble filename<br>
- type filename<br>
- symbol<br>
- progaddr [address]<br>
- loader [object filename1] [object filename2] [object filename3]<br>
- bp [address]<br>
- bp clear<br>
- run<br>
<br>

## How to process errors
#### 1. Invalid command
- if shell command is not allowed.
     
#### 2. Command number error
- if the number of args are not correct

#### 3. Command format error
- if shell command includes any other characters except for lowercase alphabet
  - if addresses and values are not hexadecimal
  - if there are more commas(,) than allowed

#### 4. Invaid memory
- if address boundary is invalid (address < 0 || address > FFFFF)
  - if start address is larger that end address

#### 5. Invalid value
- if value boundary is invalid (value < 0 || value > FF)

#### 6. Invalid mnemonic
- if mnemonic is not in the opcode.txt

#### 7. File error
- File open error : if there is no file in the current directory

#### 8. Assembler error
- File open error : if there is no file in the current directory
- Duplicated symbol : if there is duplicated symbol in .asm file
- Invalid mnemonic : if mnemonic is not directive, or not in the OPTAB
- Invalid operand <br>
  : if register, operand should be A, X, L, B, S, T, F, PC, SW <br>
    else, operand should be valid number(address or valid integer) or operand should be included in the SYMTAB
- NO END : if there is no END in the .asm file
- Invalid address : if locctr(address) boundary is invalid(locctr < 0 || locctr > FFFF)
- File format error : if source file is not .asm file
- Invalid label : if label is in the OPTAB(label should be differenct with mnemonic)

#### 9. Linking Loader error
- File open error : if there is no file in the current directory
- Duplicated external symbol : if there is duplicated external symbol in .obj file
- Undefined external symbol : if there is no external symbol in any control section
<br>

### About writer
  Author : 20181603, Minseon Kim<br>
  Date   : 2021-03-09 ~ 2021-05-04
  
