# OwlInterpreter
 An AST walking interpreter for Owl

   program 'param';
   begin
       let x: int := 7;
       let y: int := 13;
       let t: int := 0;
       func add(a: int, b: int) begin
           return a + b;
       end;
       t := add(x, y);
       print t;
   end
