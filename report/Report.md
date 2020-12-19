Report



设计思路

和project 1 一样的是需要构建一颗语法树,父节点里会有一个数组负责存储子节点的地址. 这一构建的过程发生在bison自底向上归约的过程中,在建立好语法树之后,才会开始进行检查. 检查的方式是自顶向下遍历节点,同时构建符号表. 通过符号表完成类型错误,定义错误等问题的检测.



符号表设计

符号表的搭建使用了c++ STL 的 multimap.

总共有两张表:

一张负责记录初始化的变量(基本类型,数组,结构体,函数) (表1),

另一张负责记录结构体的具体定义(成员变量) (表2)

key值是变量名.



表 1 里的 value 类都是Variable. Variable 类有四个成员变量,分别是

class_type : 指代 基本类型/数组/结构体变量/函数 

type : 基本类型 和 数组 则是其类型 ,函数指其返回的类型,结构体变量则是其结构体的名字

line: 在第几行初始化

scope: 该变量的作用域编号





int main(){

​	float i;

​	int j; 

}

| key  | Variabe(class_type) | Variabe(type) | Variabe(line) | Variabe(scope) |
| ---- | ------------------- | ------------- | ------------- | -------------- |
| i    | primitive           | float         | 2             | 1              |
| j    | array               | int           | 3             | 1              |
| main | function            | int           | 1             | 0              |





为了让数组可以记录其大小, 设计了类Array 继承 Variable, 增加 size 变量,记录大小.

为了让结构体变量可以记录其成员,设计了类Struct 继承 Variable, 增加 members变量, members 变量的类型是vector\<Variable*>.

为了让函数可以记录其形参,设计了类Function  继承 Variable, 增加 args变量, args 变量的类型是vector\<Variable*>.



表二的value则是结构体定义的成员,类型是vector\<Variable*>, 以及其作用域编号

struct apple{

​	float price;

​	int num;

}

struct banana{

​	float price;

​	int num;

}

| key    | memebers                     | scope |
| ------ | ---------------------------- | ----- |
| apple  | vector<Variable*>{price,num} | 0     |
| banana | vector<Variable*>{price,num} | 0     |





作用域机制

预设一个全局的作用域编号,即0,当进入内层作用域时,该编号加一,定义在该作用域里的变量,scope等于该编号,在退出该内层作用域时将符号表里scope等于该编号的变量删除,然后编号减一.

 