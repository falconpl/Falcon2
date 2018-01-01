# Falcon2

The Falcon Programming Language is a simple, fast and powerful scripting language, with features oriented both to fast applicaiton prototyping and complex project development.

The language is powered by a "Organic Engine" (OE) which understands and runs directly syntax constructs, or in other words, interpreets dynamically created and genetically evolved symbolic programs.

The engine is built around the idea of embeddability, or "playing nice" and offer added value to modern distributed, multithreaded applications that are not entierly dedicated to run around the scripting language.

Falcon2 is a deep refactoring of the original project, using updated modern C++ and unrolls some adaptation that were made in the previous code as afterthoughts and extensions down at the deepest core of the engine.

At his stage of development, this readme contains an extended version of the "cheatsheet" for Falcon 1.0, as a reference for language developers and users. In due time, this information will be moved in the documentation section, and the README will be used as a root documentation for developers only (with information about how to build the source and contribute to the project).

## The language in a nutshell (cheatsheets)

### Fastprint – passepartout to Falcon:
The `>` (fast print and new line) and `>>` (fast print) are used to perform basic output to the engine standard output. 
```
>> 'hello ', 'world' // one line
> "."                /* newline */
```
   
### toString() and describe()   
   Directly output an item is equivalent to using the toString() method, that is meant to transform the item into a minimal representation. Use describe to know more about its status and its internal (it is usually an excellent debug tool).
```
> [1,2,3]   
> [1,2,3].toString()
> [1,2,3].describe()
```

### Operators
* Mathematical operators: `+`, `-`, `*`, `/`, `%` (int modulo), `**` (power), `<<` (bitwise shitf), `>>`
* Auto-math: `+=`, `-=`, etc.
* Inc/dec: `++x`, `x++`, `--x`, `x--`
* Comparison: `==`, `===` (exactly), `!=`, `>`, `>=`, `<`, `<=`
* Logic: `and`, `or`, `not`
* Out-of-band: `^+` x (set), `^-` x (reset),
* `^%` x (swap), `^$` x (check)
* Bitwise: `^&` (and), `^|` (or), `^!` (not), `^^` (xor)

### Numbers
* Integers: `100`, `-1000`, `100_000`, `1_000_000`
* Floating pt: `10.03`, `4.02e16`
* Hex: `0x123ABC` `0x1def` (in strings: `"\xaDf0"`)
* Oct: `023` (dec: 19), Binary: `b1101` (dec: 13)

Number are considered as objects, and they can have properties and methods. For example:
```
foreach(printl) // prints 1..5
-5.foreach(printl) // -1..-5
```

### Strings
Strings beginning with `"` are parsed, special sequences starting with `\` are decoded. Strings starting with `'` are literal, and `\` is not interpreted.
```
x = "\t \"abc\"\n"      // parsed
y = 'unparsed \as is\'  
```

Use single quote twice to get a single quote in the literal string:
```
z = 'a literal ''x'' here.'
> "\x65,\0101" // 'A,A'
> 'abc'[0]     // 'a'
> 'abc'.len    // 3
> 'abc'[-1]    // 'c'
> 'ABC'[*0]    // 65
> 'ABC'[0:1]   // 'AB'
> 'ABC'[2:0]   // 'CAB'
> 'A'+"B"+1    // 'AB1'
> 'B'/1,'B'/-1 // 'AC'
> 'a'*3        // 'aaa'
> 'a'%65%66%67 // 'aABC'
z[1] = 'a'     // ERROR, immutable
```

### Multiline strings
Start a ML-string opening it and going next line immediately.
```
x = "
    Double quoted strings will
    trim newlines and leading
    spaces."
y = '
    Single quoted strings will
    preserve formatting as-is.'
```

### Mutable strings
```
y = x = m"abc" // or m'abc'
x[0] = 'A'     // m"Abc"
> y            // Abc as x!
x[0:0] = 'new' // m"newAbc"
x[3:4] = '-x-' // m"new-x-bc"
x[3:] = 65     // m"newA" A=65!
```

### International strings
```
VMProcess.current.setTT( [
  "i18n" => "International"
  "Another" => "Autre"
  ])
> i"i18n"    // International
> i"Another" // Autre
```

### Regular expressions
```
D = r'([0-9]*)-([0-9]*)-([0-9]*)'

// * is perfect match, / is partial match:

> D * '1999-12-01'            // true
> D * 'Date: 1999-12-01'      // false
> D / 'Date: 1999-12-01'      // true
y,m,d = D % '1999-12-01' 
> y +'/'+m+'/'+d              // 1999/12/01
mch = D ** 'Date: 1999-12-01'
> mch                         // 1999/12/01
> D.replace('1999-12-01', 'Year:\1,Month:\2,Day:\3' )     //Year:1999,Month:12,Day:01
```

Options after r'': i case insensitive, m multiline (stop at \n), n never match newline, l larger match first.
```
re = r'a.*c'il         //ci + large
> re * 'AbC'           // true
> re ** 'xabcbcd'      // abcbc
> r'a.*c' ** 'xabcbcd' // abc
```

Regex can be created also as class instances, dynamically, via RE( pattern, opts ) String expansion operator @
```
x = 'value'
v = @"X is: $x"     //simple
> @"${'new'+x}"     //eval expr
> @"$({'new'+x}5r)" //with format
```

For more about formats, see the `Format` class. 

Useful string methods:
```
";".merge('a','b')  // 'a;b'
":".join(['a','b']) // 'a:b'
"abc".find("b",1)   // 1
m"abc".insert(1,"b")// abbc
"a,b".split(',')    // ['a','b']
"a,,b".split(',')   //['a','','b']
"a,,b".splittr(',') // ['a','b']
" a ".trim()        // returns "a"
x=m" a "; x.atrim() // x <= "a"
Arrays
a = [1,2,3]     // create
> a[0]          // access
x,y,z = a       // distribute
a = .[1 2 3]    // .[ spares ,
a = Array(1,2,3)// a <= [1,2,3]
> a             // just "array[3]"
> a.describe()  // more details
> a.len         // 3
> a.empty       // false
a += 4          // adds an element
a <<= .[5 6]    // adds 2 elements
a -= 1          // removes 1
a >>= .[2 3]    // removes 2 and 3
x=[1]+[2,3]     // x <= [1,[2,3]]
x=[1]<<[2,3]    // x <= [1,2,3]
for v in x; > x; end // iterate
x.foreach(printl)    // cool iter
y=x[1:3]        // elements 1,2
x[1:1] = 1      // inserts at 1
x[1:3] = 'new'  // changes 1,2
> x[-1]         // last element
y=x[-1:0]       // reverse all
Useful array methods
x.push(1,2,3)  // adds at end
x.pop()        // gets from end
x.unshift(1,2) // adds at front
x.shift()      // removes at front
x=Array.buffer(10)   // 10 NILs
x=Array.buffer(10,0) // 10 zeros
x.resize(2)          // cuts to 2
x = .['a' 'b' 'c']
> x.find('b')           // 1
> x.scan({n=> n=='c'})  // 2
x.insert(2,'x','y')// x,y before c
x.remove(2,3)   // remove at 2,3,4
x.erase('a')    // like -=
```

### Dictionaries
x=['a'=>0,'b'=>1]   // or...
y=.['a'=>0 'b'=>1]  // without ','
> x['a']            // 1
for k, v in x       // iterate
   > @ "$k = $v"    // sorted keys
end
> x.len,' ',x.empty // 2,false
z = x.pseudo        // keys become
> z.a,',',z.b       //  properties
x<<=['a'=>9,'c'=>1] // merge
y >>=['a','b']      // remove keys

### Basic statements
```
if /*expr*/
  /* do things */
elif /* expr */
  /* do other things */
else
  /* do more things */
end

while <expr>; /* ops */; end
for elem in set; /* ops */; end
for i = m to n, step ; end

switch value
  case 0
     /* do things */
  case 1, 2 to 5, "a" to "z"
    /* do more things */
  case r'A.C', "some value"
    /* do more things */
  default
    /* do final things */
end

try
   /* things that might fail */   
catch SomeType in variable
   > variable
catch in variable   // any error
   > variable
finally
   /* cleanup stuff */
end

### More on statements
Block statements can use ':' to declare singlestatement sub-blocks:
```
if a == 0: printl("a is 0")
```
Same goes for while and switch case.

For statements support forfirst, formiddle and forlast blocks.
```
for i = 1 to 5
   forfirst: >> "Sequence: "
   >> i
   formiddle: >> ", "
   forlast: > "."  
end
```

The raise statement can generate an exception and/or create an item that must be caught by some catch clause:
```
try
   raise "Hello world"
catch String in variable
   > variable   // Hello world
end
```
The `in` clause is optional.

When catching an error instance, using as in place of in puts automatically the stack trace in the caught error.

### Functions
Toplevel functions can declared as
```
function func(p0, p1, pn)
   return p0+p1+p2
end
```

Anonymous functions can be declared as:
```
func = function(p0, p1, pn)
   /* do stuff */
end
func = {p0,p1,pn=>/* do stuff */}
```
When using, {=>expr}, if expr is a single expression, it's value is automatically returned; else, you need to use the return statement.

There are many ways to invoke a function; they are all called _evaluation_.
```
func('a',2,3)     // evaluate
func('a',2)       // pn = nil
func('a',2,3,4)   // 4 is ignored
func(p1|'a')      // named pars
x=.[func 1 2 3]; x() // cache call
.[func 1 2](3)    // some cache
pars = ^( 1,2,3 ) // EvalPars expr
func # pars       // invoke op.tor
pars[1] = 'a'; func#pars // again
Function methods and properties
The keyword fself refers to the function
currently evaluated.
function testMe(a,b)
   > fself.fullName
   > fself.location // source line
   > fself.parameter(0) //a
   > fself.parameter(2) //1 past b
   > fself.pcount   // params
end
> testMe.name       // 'testMe'
testMe()            // pcount = 2
testMe(1,2,3,4)     // pcount = 4
```

### Classes, objects, prototypes
Class define a rigidly structured OOP hierarcy. Objects describe class instances, and can be used to create singleton, link-time instances. Prototypes are flexible objects whose structure can be changed runtime.
```
x = p{a=2}          // proto decl
y = p{b=1;_base=[x]}
> @"${y.a},${y.b}"  // 2,1
x.c = 3             // change in x
> y.c               // seen in y
```

When a function is extracted from an object, it becomes a method; self keyword refers to the method object:
```
x.func = {v => self.b+v}
> y.func(3)      // 3+1=4
z = y.func       // caching a mth
> z(3)           // again, 3+1=4
```

Classes are more structured:
```
class xyz(p0,p1)
  prop0= 10+p0    // assign expr
  p1=p1           // same name OK
  p2=nil
  init            // constructor
    > "Got params ",p0,',',p1 
    self.p2 = 'Init here'
  end
  function mth0(a,b)
    > self.p0+a+b
  end
end

x = xyz(1,2)      // eval = create
y = [xyz,1,2]()   // another eval

m = y.mth
mm(2,3)           // ===y.mth(2,3)
Inheritance is done via from:
class child(a,b) from \
parent0(a+10),
parent1(b+20)
/* . . . */
end
a = child(1,2)
a.prop          // prop in child
a.parent0.prop  // prop in parent0
```

Objects are singletons created at link time; since they are not "called", they can't have parameters:
```
object xyz
  a=0
end

object abc from parent("init data")
  b=1
end
> xyz.a, ',' , abc.b  // 0,1
```

### Accumulator expression 
The accumulator expression is an expression integrating iteration over multiple sets, combination and filtering of each entry and generation of a new set out of the results.
```
^[g0, g1... gn] filter => target
```
All parts of the expression past `^[...]` are optional.

This expression iterates over `g0`...`gn`, eventually invoking `filter`, and eventually invoking the `+=` operator over the `target`. 
The `filter` function receive a different permuation from the values in `g0` ... `gn` sets at each invocation, with the first value from g0, g1, g2 etc., then the second value from g0, and the first from the others. If the filter function returns true, the values are added to the target (a single value if there is only one generator, an array of values with multiple starting sets).

The expression evaluates as the target, if given, or as `nil` if not.
```
^[5] printl                                               // prints 1 to 5
x= ^[3,3] => []                                           // x = [[1,1] [2,1] ... [3,3]
y= ^[['a','b','c'],['a','b','c']] {(g0,g1) g0!=g1} => []  // y = [['a', 'b'] ['a', 'c'], ['b','a'], ['b', 'c'] etc.
```
If the filter fuction reutns an _out of band_ value instead of true or false, the returned value is added to the target.
```
// putting 1..5 square in z
z= ^[5]{(v) ^+(v**2)} => []     // z = [1 4 9 16 25]
```

Ranges `[start: end: step]` can be used in any iterable sequence (notice they iterate up to end-1 as [n:n] means empty).
```
z=^[ [1:6:2] ]=>[]       // z=[1,3,5]
```

# Generators
Instead of sets, any iteration (e.g. for/in or the Accumulator expression) can be provided with generators.

Generators are simply functions returning _with doubt_  (`return?` statement) if there are more items to be returned. The last item in the iteration is returned _without doubt_.

```
function makeGen( array )
   idx = 0      // close the index
return {=> 
   if idx+1==array.len //last?
      return array[idx]
   else // no, there's more...
      return? array[idx++]
   end }
end
gen = makeGen(.['a' 'b' 'c'])
for value in gen: > value
```

Blocks can yield values with doubt using the `^?` operator. Generators can break immediately the iteration, without returning a non-doubt element, using the return break statement.

### Namespaces
A namespace is declared through the namespace directive, and accessed via `.`, or via `..` to force namespace parsing.
```
namespace abc
abc.var = 1    // the compiler knows "abc" as a namespace
xyz..var = 2   // "xyz" becomes a namespace on the fly
```
Namespaces can also be implicitly declared via the import directive.

```
import Syn.*              // Syn is now a namespace
// moving a,b,c in NS xyz:
import a,b,c from MyModule in xyz
```

### Load and export
The load and export directives are used to create a complete program out of separate modules. All the modules involved in load are merged into a single, complete entity, and they all see the variables put in common via export.
```
// file: modA.fal
v1 = "Hello"; export v1
// file: modB.fal
namespace myNS
myNS.v2 = "world"; export myNS.v2
//file: main.fal
load modA          // logical name
load "path/to/modB.fal"
> v1, " ", myNS..v2 // Hello world
```

### Import
The import directive brings one or more symbols declared elsewhere in the current module. It has many forms with different meanings:
```
// Force to import this ones:
import v1, v2, vN
v1 = 5  // Not re­declared here!
// import from another module
import x,y,NS.z from MyMod
// import and change name
import someVar from mod as newName
// import in NS
import a,b,NS.c from mod in MyNS
> MyNS.a    // was a in mod
> MyNS.c    // was NS.c in mod
// Import a namespace from global
import Syn.*   
x = Syn.While(...) 
// Declare generic providers
import from mod1
import from "path/to/mod2.fal"
// this must be in mod1 or mod2:
> someVar 
// declare providers for a NS
import NS.* from modx
> someVar   // NS.someVar in modx
// Change NS 
import NS.* from mody in MyNS
> MyNS.y   // NS.y in mody
```

### Aspect oriented programming – summoning
_Summoning_ is calling an object method via a “message”, which can be redirected by the receiver. It's done via
```
<obj>::prop     //property get
<obj>::prop[x]  //property set
<obj>::method[] //mth summon
```
example:
```
class Target
   prop = 0
   function mth(p0,p1)
      > "called: ", p0, ",", p1
   end
end
tg = Target()  // new instance
> tg::prop     // summon property
> tg::prop[1]  // set new
> tg::prop     // changed value
tg::mth["first", "second"]
```
Commas between parameters are optional: ```tg::mth["first" "second"]``` is a valid summon.

### Optional summoning
Using :? in place of :: summoning becomes optional: no action is taken if the required method or property is not present.
```
tg::unknown[]  // throws error
tg:?unknown[]  // call if possible, otherwise nothing is done.
```
### Indirect summoning
A property/method that is unknown when the program is writen can be summoned indirectly using the summon or vsummon special messages:
```
x::summon[symbol p0 … pn]
```
where symbol is either a symbol declared as `&x` or a string.
The vsummon mesage uses an iterable sequence to provide parameters: 
```
param_array = .['a' 'b' 'c']
x::vsummon[&x param_array]
```

Notice that summon and vsummon messages can be overridden as any other message.
```
x = p{
  summon = {(a) > 'Summoned ', a}
}
x::summon['me']     // prints "Summoned me"
```

### Check responding to messages
The respondTo special message returns true if the entity wants to accept the given summon.
```
tg = Target()
> tg::respondsTo['prop']  // true
> tg::respondsTo[&msg]    // true
> tg::respondsTo['abc']   // false
```

The respondsTo message can be overridden to declare that an object is willing to accept messages other than its own properties and methods. See below.

### Intercepting unknown summons
If the target object exposes an __unknownSummon method, that gets called when a message that is not found as property or method is invoked. This can be used in combination to respondsTo override do provide flexible summoning.
```
class VarSummon
   function mth(p0,p1)
      > "called: ", p0, ",", p1
   end
   function respondsTo(msg)
      if r'call.*' / msg
         return true
      end
      return msg in self
   end
   function __unknownSummon(msg)
      if r'call.*' / msg
        > 'called: ', msg, '/',
            passvp().describe()
        return
      end
      raise AccessError()
   end
end
vs = VarSummon()
> vs::respondsTo['msg']   // true
> vs::respondsTo['callMe']// true
> vs::respondsTo['xyz']   // false
> vs::msg['a' 'b']        // ok
> vs::callMe['x' 'y']     // ok
> vs::xyz[]               // error
```
### Delegation
Delegation redirects some or all the summons to another target object.
```
x::delegate[tg] // dlg all to tg
// delegate some messages
x::delegate[tg 'msg0' … &msgN]
x::delegate[]   // clears all dlg
```
Delegation is incremental:
```
x::delegate[tg 'msg0']
x::delegate[tg 'msg1']
```
is equivalent to
```
x::delegate[tg 'msg0' 'msg1']
```
The delegated messages don't need to be present in the original object. Example:
```
class Responder
   function doGood(p0,p1)
      > "called: ", p0, ",", p1
   end
end
x = "Something"
resp = Responder()
x::delegate[resp "doGood"]
x::doGood['a' 'b']
```

### Rules
Rules try all the alternatives until success. Function return values are divided into 2 categories: deterministic and non-deterministic
(also called doubtful). 
Any function can return a doubtful result using the `return?` Statement. Any evaluation can yield a doubtful result using the `^?` expression.
Rules set a branch point each time they cross a doubtful result. When an evaluation or function return yields 'false'. The built-in function advance returns a doubtful result taken from an iterable
sequence.
```
rule
   a = advance(.[1 2 3])
   b = advance(.['a' 'b' 'c'])
   // ret true if a and b are ok 
   check(a,b)   
   or
   declareFailure()
end
```
Using another statement in rules abandons the rule context; evaluation of expressions turns back to normal:
rule
   a = advance(.[1 2 3])
   if a > 2
      // can return false and the evaluation will go on
      check(a)
   end
   check(a) // if false here, the rule will fail
end
```
Rules can be nested, and other rules can be declared inside nested statements.
```
rule
   a = advance(.[1 2 3])
   rule
     check(a) // is a ok?
     or
     a = a * 2 // no? - double it
   end
   if a > 2
      b = work_with(a)
      rule
        check(b) // is b ok?
      or
        b = b * 2 // no? Dbl it
      end
   end
end
```
The cut ! statement discards non-determinism that was previusly detected. It forces the rule to be ok with the first result it got from a doubtful return.
rule
   a = random(10) // doubtful val
   !              // ok with that
   a < 5; wereLuky() 
   or
   weLost()
end
```

The result yield by a rule is either true, if every expression in the rule context evaluated to nonfalse,
or false if the rule 'fails'. In this case, all the variables that were changed in the rule context are rolled back to their previous value.
```
a = -1
result = rule
    a = random(1,10); !
    a < 5
end
// Will be true:6..10 or false:-1
> result, ':', a
```

### Code blocks, Literal code and quoting
The `{[] ...}` expression returns a tree of code as-is.
Evaluating a `{[] ...}` code is the same as having it verbatim substituted in place. Variables scope in `{[] ...}` is the same as the caller.
For example:
```
x = {[] while a > 0; > --a; end }
> x[0].render()     // prints "while a > 0 ... end"
> x[0][0].render()  // prints "> --a"
a = 5
x()               // literal eval, "a" is whatever was in this context.
y = $a            // symbol 'a'
> y,'=',y()       // a=0
```
Tilde-symbol is used for names that shall be generated dynamically:
```
x = Symbol( "abc" )
x(5)      // assign 5 to "abc" dynamically
> ~abc    // "> abc" without tilde would cause an import error
```
The `{() ...}` expression returns a new tree each
time it is parsed (not evaluated), and closes the
variable scope.
The `^=` operator substitutes the literal expression with value. For example
```
x = {(v1,v2) a=v1+v2; >a; ^=a+1}    // evaluating the expression yields v1+v2+1
m=x(1,2)                            // print 3, returns 4
> m                                 // m is 4
```

The ^=& operator returns an expression and evaluates it the caller's context.
```
// eval AFTER ret
x={(v1) ^=& (&m()**v1)}  
m=2
x(3)     // evaluates (&m()**3), and &m() is "2" in this context
```
The `{[p0..pN] ... }` expression will do an ETA evaluation of the parameters: they get passed UNPARSED in the code. For example, 
```
my_if={[expr, ifok, ifno] 
    expr() ? ifok() : ifno() 
}

a=1
my_if(a==0, printl('zero'), printl('nz'))
```

here `a==0`, `printl('zero')` and `printl('nz')` are passed as-is to `my_if`, without being evaluated. `my_if` will evaluate the expressions on its own, when necessary.

The unquote expression ^~ resolves in the value of the expression in the context where the literal codeblock is defined:
```
s1 = 10
expr={(a) a+^~s1}
> expr.render()   // {(a) a+10} the s1 symbol was resolved when expr was assigned
```

To create an ETA function, use function&(...) or {p0..pn &=>...}. To evaluate after return from a function, use return&:
```
function& funcIf(expr,ifok,ifno)
   if expr(): return& ifok
   return& ifno
end
```

### Reflection
Reflection can also be generated using reflected statements and expressions. Each grammar token has a class counterpart. The following generate the same code:
```
import Syn.*
a = 0
loop1 = While( LT(&a,10),
 FastPrint( PreInc(&a)) )
loop2 = {[] while a < 10 
 > ++a; end}
```

Each grammar token can be accessed and manipulated via operators:
```
> loop1.render() // the whole loop
> loop1.selector.render() // a<10
> loop1[0].render()       // > ++a
> loop1[0] = {[] > a++ }
> loop1.render()          // > a++
```

### Load and import modspec
Load and import directives can use both a Organic Engine Virtual File System (__VFS__) path (full or relative), or a logical modspec. The modspec is calculated starting from the VFS path of the main module in a process, with the following rules:
#A name as-is is relative to the same location where the main module is found.
#A name preceded with a single dot is relative to the location where the requester is found.
#A name preceded by 'self.' is to be found in a subdirectory having the same name of the requester, relative to the location where the requester is found.
#Names separated by dots are considered VFS subdirectories.
```
// directory structure:
//   main.fal
//   mod1.fal
//   abc/mod2.fal
//   abc/mod3.fal
//   abc/mod2/X.fal
//   abc/sub/Y.fal
//   lib/Z.fal
//   main/mod4.fal

// Inside main.fal
load mod1  // mod1 is besides main
load .mod1 // same
load abc.mod2  // abc/mod2.fal
load self.mod4 // main/mod4.fal
...

// Inside abc/mod2.fal
load mod1    // besides main
load lib.Z   // lib/Z.fal
load .mod3   // abc/mod3
load .sub.Y  // abc/sub/Y.fal
load self.X  // abc/mod2/X.fal
...
```

# Dynamic compilation
The compile() function creates dynamic out of a source code string.

```
code = compile(
  "while a < 5; > ++a; end")
a = 0
> code.render()  // while a<5 ...
code()           // loops!
```

Dynamic inclusion
The `include()` function dynamically loads a module; it may be used to automatically run the main function of the required module or to access arbitrary symbols in the required module.
The prototype is:
```
include(URI,[path],[enc],[syms])
   URI – URI/name of the module
   path – search path
   enc – Source text encoding
   syms – Dictionary of desired symbols
```
If `syms` is set to a dictionary, on success the values of each entry will be the symbols found in the module.
```
// Inside mymod.fal
> "Hello world"
...

// Inside main.fal
// This will print "Hello world"
include("mymod.fal")
// ... equivalent to
dict = ["__main__"=>nil]
include("mymod.fal", nil, nil, dict)
dict["__main__"]()
```

### Parallel programming
The Parallel class is the interface towards parallel programming.
```
code1 = {=> printl("one")}
code2 = {=> printl("two")}
p = Parallel(code1, code2)
p.launch()
```
The caller code will wait till all the code branches (agents) in the parallel computations are completed.

### Attributes
Modules, function and classes can have attributes that specify some characteristics or configuration parameters that are evaluated at link time, before the actual program is executed.
```
:author => "Giancarlo Niccolai"
:version => 0x0103
:opts => ['a'=>true, 'b'=>false]
```

Attributes can be queried via the `attributes` property (will be a dictionary) and the `getAttribute`/`setAttribute` methods of the module class.
