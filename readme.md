## 待办清单

### 功能函数

- [x] print
- [ ] int
- [ ] double
- [ ] str
- [ ] bool

### 表达式解析部分

- [x] `file_input: (NEWLINE | stmt)* EOF;`
  
- [x] `funcdef: 'def' NAME parameters ':' suite;`

- [x] `parameters: '(' typedargslist? ')';`
  
- [x] `typedargslist: (tfpdef ('=' test)? (',' tfpdef ('=' test)?)*);`
  
- [x] `tfpdef: NAME ;`

- [x] `stmt: simple_stmt | compound_stmt;`
  
- [x] `simple_stmt: small_stmt  NEWLINE;`
  
- [x] `small_stmt: expr_stmt | flow_stmt;`
  
- [x] `expr_stmt: testlist ( (augassign testlist) | ('=' testlist)*);  // 连等 加等/减等/...`
  
- [x] `augassign: ('+=' | '-=' | '*=' | '/=' | '//=' | '%=' );`
  
- [x] `flow_stmt: break_stmt | continue_stmt | return_stmt;`
  
- [x] `break_stmt: 'break';`

- [x] `continue_stmt: 'continue';`
  
- [x] `return_stmt: 'return' (testlist)?;`
  
- [x] `compound_stmt: if_stmt | while_stmt | funcdef ;`
  
- [x] `if_stmt: 'if' test ':' suite ('elif' test ':' suite)* ('else' ':' suite)?;`
  
- [x] `while_stmt: 'while' test ':' suite;`

- [x] `suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT;`
  
- [x] `test: or_test ;`
  
- [x] `or_test: and_test ('or' and_test)*;`
  
- [x] `and_test: not_test ('and' not_test)*;`
  
- [x] `not_test: 'not' not_test | comparison;`
  
- [x] `comparison: arith_expr (comp_op arith_expr)*;`
  
- [x] `comp_op: '<'|'>'|'=='|'>='|'<=' | '!=';`
  
- [x] `arith_expr: term (addorsub_op term)*;`
  
- [x] `addorsub_op: '+'|'-';`
  
- [x] `term: factor (muldivmod_op factor)*;`
  
- [x] `muldivmod_op: '*'|'/'|'//'|'%';`
  
- [x] `factor: ('+'|'-') factor | atom_expr;`
  
- [x] `atom_expr: atom trailer?;`

- [x] `trailer: '(' (arglist)? ')' ;`
  
- [x] `atom: (NAME | NUMBER | STRING+| 'None' | 'True' | 'False' | ('(' test ')'));`
  
- [x] `testlist: test (',' test)* (',')?;  // 算式  eg： a,b  a  a+b`
  
- [x] `arglist: argument (',' argument)*  (',')?;`
  
- [x] `argument: ( test | test '=' test );`
