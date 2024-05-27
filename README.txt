DOCUMENTATION
How to run:
Input your test case in input.txt.
Run the following commands in terminal.
$ ./sh/build.sh
$ ./sh/run.sh

Description:
Use CNF form like (A||B||C||...)&&(D||E||...)&&()...
Inside the source code comments, "Clause" refers to the sequence of OR'd arguments.
Each thing inside the Clause is refered to as a Literal.

Each line of the input.txt file is a clause. Use one space to separate tokens.
The following are the allowed tokens:

"z0" variable name is reserved. Use it in place for any time you need to have a one argument literal.
i.e. x <= 0 -> x - z0 <= 0 in the input.txt.
Never use z0 as the first argument in a literal otherwise idk its 3am and I didn't test it. asjdfahljdfl

Terminals: <token>
NOT: !
OR: ||
VAR: {all strings without spaces}\{z0}
ZERO: z0
OP: {<,>,<=,=>,=,!=}
CONST: Any integer
MINUS: -

Rules for each line parsing:
Clause ::= Literal | Literal OR Literal
Literal ::= Proposition | NOT Proposition
Proposition ::= VAR MINUS VAR OP CONST

(EXAMPLE 1)
input.txt:
! x - y >= 5 || ! y - x > -20
! y - z >= 4
z - x < 6
x - z0 = 5
Output:
y = -15
z = -20
x = 5
z0 = 0

(EXAMPLE 2)
input.txt:
! x - y >= 5 || ! y - x > -20
! y - z >= 4
z - x < 6
x - z0 = 5
y - z0 = 0
Output:
Unsatisfiable
