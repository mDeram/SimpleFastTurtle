# SimpleFastTurtle
SFT is an interpreted programming language written in C, currently in early stage of developpement.

# Goals
Provide a simple to use and fast programming language to perform some specific functions.

Deeper understanding of how programming languages work.

# Preview

## Code

```sft
var aLittle = 4;
var aLot;

if aLittle < 2
    aLot = 1000
else
    aLot = 2000

fn average(a, b)
{
    return (a + b)/2
}

print(average(aLittle, aLot));
```
## Tokens

```
[Line: 2		Type: 2		Id: 74		Symbol: var		]
[Line: 2		Type: 4		Id: 100		Symbol: aLittle		]
[Line: 2		Type: 0		Id: 61		Symbol: =		]
[Line: 2		Type: 3		Id: 97		Symbol: 4		]
[Line: 2		Type: 1		Id: 59		Symbol: ;		]
[Line: 3		Type: 2		Id: 74		Symbol: var		]
[Line: 3		Type: 4		Id: 100		Symbol: aLot		]
[Line: 3		Type: 1		Id: 59		Symbol: ;		]
[Line: 5		Type: 2		Id: 66		Symbol: if		]
[Line: 5		Type: 4		Id: 100		Symbol: aLittle		]
[Line: 5		Type: 0		Id: 60		Symbol: <		]
[Line: 5		Type: 3		Id: 97		Symbol: 2		]
[Line: 6		Type: 4		Id: 100		Symbol: aLot		]
[Line: 6		Type: 0		Id: 61		Symbol: =		]
[Line: 6		Type: 3		Id: 97		Symbol: 1000		]
[Line: 7		Type: 2		Id: 68		Symbol: else		]
[Line: 8		Type: 4		Id: 100		Symbol: aLot		]
[Line: 8		Type: 0		Id: 61		Symbol: =		]
[Line: 8		Type: 3		Id: 97		Symbol: 2000		]
[Line: 10		Type: 2		Id: 73		Symbol: fn		]
[Line: 10		Type: 4		Id: 100		Symbol: average		]
[Line: 10		Type: 1		Id: 40		Symbol: (		]
[Line: 10		Type: 4		Id: 100		Symbol: a		]
[Line: 10		Type: 1		Id: 44		Symbol: ,		]
[Line: 10		Type: 4		Id: 100		Symbol: b		]
[Line: 10		Type: 1		Id: 41		Symbol: )		]
[Line: 11		Type: 1		Id: 123		Symbol: {		]
[Line: 12		Type: 2		Id: 70		Symbol: return		]
[Line: 12		Type: 1		Id: 40		Symbol: (		]
[Line: 12		Type: 4		Id: 100		Symbol: a		]
[Line: 12		Type: 0		Id: 43		Symbol: +		]
[Line: 12		Type: 4		Id: 100		Symbol: b		]
[Line: 12		Type: 1		Id: 41		Symbol: )		]
[Line: 12		Type: 0		Id: 47		Symbol: /		]
[Line: 12		Type: 3		Id: 97		Symbol: 2		]
[Line: 13		Type: 1		Id: 125		Symbol: }		]
[Line: 15		Type: 4		Id: 100		Symbol: print		]
[Line: 15		Type: 1		Id: 40		Symbol: (		]
[Line: 15		Type: 4		Id: 100		Symbol: average		]
[Line: 15		Type: 1		Id: 40		Symbol: (		]
[Line: 15		Type: 4		Id: 100		Symbol: aLittle		]
[Line: 15		Type: 1		Id: 44		Symbol: ,		]
[Line: 15		Type: 4		Id: 100		Symbol: aLot		]
[Line: 15		Type: 1		Id: 41		Symbol: )		]
[Line: 15		Type: 1		Id: 41		Symbol: )		]
[Line: 15		Type: 1		Id: 59		Symbol: ;		]
```

## Abstract Syntax Tree

```
├─var┐
|    └─=
|      ├─L aLittle
|      └─R 4
├─var┐
|    └─aLot
├─if┐
| | └─<
| |   ├─L aLittle
| |   └─R 2
| └─expr┐
|       └─=
|         ├─L aLot
|         └─R 1000
├─else
| └─expr┐
|       └─=
|         ├─L aLot
|         └─R 2000
├─average┐
| |      ├─a
| |      └─b
| └─return┐
|         └─/
|           ├─L +
|           | ├─L a
|           | └─R b
|           └─R 2
└─expr┐
      └─print(
         └┐
          └─average(
             └┐
              ├─aLittle
              └─aLot
              )
          )
```
