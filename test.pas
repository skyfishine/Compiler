begin
  integer k;
  integer m;
  integer function f(integer n);
    begin
      if n<=0 then f:=1
      else f:=n*f(n-1)
    end;
  read(m;
  k:=f(m);
  write(k)
end