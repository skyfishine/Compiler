## 项目说明

本项目为电子科技大学(UESTC)计算机科学与技术专业编译原理实验项目，主要实现了对一个pascal语言子集进行词法分析和语法分析。

词法分析通过状态转换实现。

语法分析通过递归下降方法实现。

## 编译运行

执行`make`编译，得到a.out可执行程序

执行`make run`可以直接编译运行

执行`make gdb`可以调试代码

## 语法分析

### 文法

完整文法如下所示

```xml
<程序> -> <分程序>
<分程序> -> begin <说明语句表>;<执行语句表> end
<说明语句表> -> <说明语句>│<说明语句表>;<说明语句>
<说明语句> -> <变量说明>│<函数说明>
<变量说明> -> integer <变量>
<变量> -> <标识符>
<标识符> -> <字母>│<标识符><字母>│ <标识符><数字>
<字母> -> a│b│c│d│e│f│g│h│i│j│k│l│m│n│o │p│q │r│s│t│u│v│w│x│y│z
<数字> -> 0│1│2│3│4│5│6│7│8│9
<函数说明> -> integer function <标识符>(<参数>);<函数体>
<参数> -> integer <变量>
<函数体> -> begin <说明语句表>;<执行语句表> end
<执行语句表> -> <执行语句>│<执行语句表>;<执行语句>
<执行语句> -> <读语句>│<写语句>│<赋值语句>│<条件语句>
<读语句> -> read(<变量>)
<写语句> -> write(<变量>)
<赋值语句> -> <变量>:=<算术表达式>
<算术表达式> -> <算术表达式>-<项>│<项>
<项> -> <项>*<因子>│<因子>
<因子> -> <变量>│<常数>│<函数调用>
<函数调用> -> <标识符>(<算术表达式>)
<常数> -> <无符号整数>
<无符号整数> -> <数字>│<无符号整数><数字>
<条件语句> -> if<条件表达式>then<执行语句>else <执行语句>
<条件表达式> -> <算术表达式><关系运算符><算术表达式>
<关系运算符>  -> <│<=│>│>=│=│<>
```

消除左递归：

```xml
<Program> -> <SubProgram>
<SubProgram> -> begin <ExplanStmtTlb>;<ExecStmtTlb> end
<ExplanStmtTlb> -> <ExplanStmt><ExplanStmtTlb_>
<ExplanStmtTlb_> -> ;<ExplanStmt><ExplanStmtTlb_>|ε

<ExplanStmt> -> <VarStmt>│<FuncStmt>
<VarStmt> -> integer <Var>
<Var> -> <Ident>
<Ident> -> <Letter><Ident_>
<Ident_> -> <Letter><Ident_>│<Number><Ident_>|ε

<Letter> -> a│b│c│d│e│f│g│h│i│j│k│l│m│n│o│p│q│r│s│t│u│v│w│x│y│z
<Number> -> 0│1│2│3│4│5│6│7│8│9
<FuncStmt> -> integer function <Ident>(<Param>);<FuncBody>
<Param> -> <Var>
<FuncBody> -> begin <ExplanStmtTlb>;<ExecStmtTlb> end
<ExecStmtTlb> -> <ExecStmt><ExecStmtTlb_>
<ExecStmtTlb_> -> ;<ExecStmt><ExecStmtTlb_>|ε

<ExecStmt> -> <ReadStmt>│<WriteStmt>│<AssignStmt>│<CondStmt>
<ReadStmt> -> read(<Var>)
<WriteStmt> -> write(<Var>)
<AssignStmt> -> <Var>:=<ArithExp>
<ArithExp> -> <Item><ArithExp_>
<ArithExp_> -> -<Item><ArithExp_> | +<Item><ArithExp_> |ε

<Item> ->  <Factor><Item_>
<Item_> -> *<Factor><Item_>|ε

<Factor> -> <Var>│<Const>│<FuncCall>
<Const> -> <UnsignedInt>
<UnsignedInt> -> <Number><UnsignedInt_>
<UnsignedInt_> -> <Number><UnsignedInt_>|ε

<FuncCall> -> <Ident>(<ArithExp>)

<CondStmt> -> if<CondExp>then<ExecStmt>else <ExecStmt>
<CondExp> -> <ArithExp><RelOp><ArithExp>
<RelOp>  -> <│<=│>│>=│=│<>
```

备注：具体实现上，即使消除了左递归，还是要考虑以下两种情况。

1. 两个终结符开头的文法：

```xml
<ExplanStmt> -> <VarStmt>│<FuncStmt>
<VarStmt> -> integer <Var>
<FuncStmt> -> integer function <Ident>(<Param>);<FuncBody>
```

2. 说明语句和执行语句的分隔符，说明语句和说明语句的分隔符，两者都是分号，分析的过程中可能存在冲突
    * 实现时采用简单的方法：提前查看分号后面的token，如果是说明语句的第一个token，那么按照说明语句递归，否则按照执行语句递归。
```xml
<函数体> -> begin <说明语句表>;<执行语句表> end
<说明语句表> -> <说明语句>│<说明语句表>;<说明语句>
    
<说明语句表> -> <说明语句> <说明语句表_>
<说明语句表_> -> ;<说明语句> <说明语句表_>
```

3. 注意：标识符和数字已经在词法分析过程中分析过了，语法分析无需再次实现！

### 输出结果

#### 变量名表

格式：
1. 变量名 vname: char(16)
2. 所属过程 vproc: char(16)
3. 分类 vkind: 0/1 (0表示变量，1表示形参)
4. 变量类型 vtype: types
5. 变量层次 vlev: int
6. 变量在变量表中的位置 vadr: int (相对于第一个变量)

#### 过程名表

格式：
1. 过程名 pname: char(16)
2. 过程类型 ptype: types
3. 过程层次 plev: int
4. 第一个变量在变量表中的位置 fadr: int
5. 最后一个变量在变量表中的位置 ladr: int

#### 示例结果

pascal代码：

```pascal
begin
	integer k; integer m; 
	integer function F(n);
    	begin
            integer n;
            if n<=0 then F:=1
            else F:=n*F(n-1)
        end
	;
	read(m);  k:=F(m);   write(k)
end
```

变量名表：

| 变量名 | 所属过程 | 分类 | 变量类型 | 变量层次 | 变量在变量表的位置 |
| ------ | -------- | ---- | -------- | -------- | ------------------ |
| k      | main     | 变量 | integer  | 0        | 0                  |
| m      | main     | 变量 | integer  | 0        | 1                  |
| n      | F        | 形参 | integer  | 1        | 2                  |

过程名表：

| 过程名 | 过程类型 | 过程层次 | 第一个变量位置 | 最后一个变量位置 |
| ------ | -------- | -------- | -------------- | ---------------- |
| main   | void     | 0        | 0              | 1                |
| F      | integer  | 1        | 2              | 2                |