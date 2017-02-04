{The following procedure (Not MD) either returns a character in q, or sets
Resetnow = ys, if nothing entered or carrier dropped.}
Procedure Inputchar(echo : Boolean);
begin
  tout := 0; q := #00;
  repeat
    begin
      tout := tout + 1;
      if chaready then begin q := charin; sysop := no; end;
      if Keypressed then begin read(kbd,q); sysop := ys; end;
    end
  until (q <> #00) or (tout > timeout) or (not carrier and not local);
  if q > #127 then q := Chr(Ord(q)-128);  {strip parity bit if set}
  if echo then
    if (q in [bs,cr,lf,^G]) or ((q > #31) and (q <= #127)) then begin
      write(q);
      if not local then charout(q);
    end;
  if q = #00 then Resetnow := ys;
end;

{Following proc (not MD) outputs an entire line through repeated calls to
Charout.  It echoes output to system console, and if linefeed is true, sends
a LF after CR to remote user.  If called with second parm false, will not
linefeed.  Also outputs nulls, if requested.}
Procedure Lineout(x : string80; y : boolean);
var i : integer; j : char;
begin
  if not local then begin
    j := chr(0);
    if nulls > 0 then for i := 1 to nulls do charout(j);
    i := 0;
    while (i < length(x)) and not (j in ['S','s','N','n']) do begin
      i := i + 1;
      charout(copy(x,i,1));
      if keypressed or chaready then begin
        if chaready then j := charin else read(kbd,j);
        if j > #127 then j := chr(ord(j)-128);
        if j in [^S,'P','p'] then inputchar(no);
      end;
    end;
    if y then begin
      charout(cr);
      if linefeeds then charout(lf);
    end;
    if not carrier and not carrier then Resetnow := ys;
    if j in ['S','s'] then q := 'S';
    if j in ['N','n'] then q := 'N';  {Added to facilitate Nexting}
  end;
  write(x);
  if y then write(cr,lf);
  if keypressed then begin
    read(kbd,j);
    if j in ['S','s'] then q := 'S';
  end;
end;

{The following procedure inputs a 255 char max line.}
{Maxlen is how many chars to accept, ~show for pswds, x for lf.}
Procedure Linein(maxlen : Integer; show, x : Boolean);
var z : Integer;
begin
  stringin := '';
  z := 0;
  repeat
    begin
      inputchar(no);
      if (q > #31) and (q < #127) then
        begin
          stringin := stringin + q;
          z := z + 1;
          if not show then lineout('*',no) else lineout(q,no);
        end
      else if ((q = bs) or (q = #127)) and (z > 0) then
        begin
          lineout(bs,no);
          delete(stringin,z,1);
          z := z - 1;
        end;
    end
  until Resetnow or (q = cr) or (z >= maxlen);
  if x then lineout(' ',ys);
end;

{This function returns a line in uppercase.}
Function LineUp(s : String80) : String80;
var y : String80;
begin
  y := '';
  if length(s) > 0 then for x := 1 to length(s) do
    y := y + Upcase(copy(s,x,1));
  LineUp := y;
end;

{This function returns an integer number from the user.}
Function Intin : Integer;
var x,y : Integer;
begin
  Linein(5,ys,ys);
  if length(stringin) = 0 then x := -1
  else
    begin
      Val(stringin,x,y);
      if y <> 0 then x := -1;
    end;
  Intin := x;
end;
