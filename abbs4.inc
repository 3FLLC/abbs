{This routine outputs the contents of the message buffer formatted.}
Procedure Outbuff;
var l, x, y, z : Integer;
begin
  lineout(' ',no);
  l := 2;  y := 1;  q := ' ';
  while (y < p) and not Resetnow do
    begin
      x := y;
      repeat
        begin
          y := y + 1;
          l := l + 1;
        end
      until (Msgbuff[y] = #32) or (l = width) or (y = p);
      if l = width then
        begin
          write(cr,lf);
          l := 2 + (y-x);
          if not local then
            begin
              charout(cr);
              if linefeeds then charout(lf);
            end;
        end;
      z := x;
      repeat
        begin
          write(Msgbuff[z]);
          if not local then charout(Msgbuff[z]);
          if Msgbuff[z] = #13 then
            begin
              write(lf);
              l := 2;
              if not local and linefeeds then charout(lf);
            end;
          z := z + 1;
          if chaready or keypressed then
            begin
              if chaready then q := charin else read(kbd,q);
              if q in [^S,'P','p'] then inputchar(no);
            end;
        end;
      until (z >= y) or Resetnow;
      if q in ['N','n','S','s'] then y := p;
    end;
  lineout(' ',ys);
end;

{This routine accepts/appends characters into msgbuff.}
Procedure Editbuff;
var l, t, x, y, z : Integer;  {Line position, total lines.}
begin
 done := false;
 t := 0;
 if p = 0 then begin   {Add the timestamp}
   stringout := Time;
   p := length(stringout);
   for x := 1 to p do MsgBuff[x] := Copy(stringout,x,1);
   p := p + 1; MsgBuff[p] := cr; p := p + 1; MsgBuff[p] := ' ';
 end;
 repeat begin
  lineout(' ',ys);
  lineout(' Buffer open...', ys);
  if not expert then
   begin
    lineout(' Messages are formatted to callers screen width.  To defeat formatting,',ys);
    if not (q in ['S','s']) then
    lineout(' begin line with a space.  End message entry with two RETURNs.',ys);
   end;
  lineout(' ',ys);
  repeat begin
    if p < 1520 then Stringout := '>' else Stringout := 'E>';
    lineout(Stringout,no);
    l := 0;
    repeat begin
      inputchar(ys);
      if ((q = bs) or (q = #127)) then
       if (l <= 0) then
        begin
          if not local then charout('>');
          write('>');
        end
       else
        begin
          l := l - 1;
          if (l = 0) and (Msgbuff[p] <> #32) then Msgbuff[p-1] := #32;
          p := p - 1;
          if q = #127 then
            begin
              if not local then charout(bs);
              write(bs);
            end;
        end
      else if (q = #13) or ((q > #31) and (q < #126)) then
        begin
          l := l + 1;
          if (l = 1) and (q = #32) then Msgbuff[p] := #13;
          p := p + 1;
          Msgbuff[p] := q;
        end;
    end until (q = #13) or (p > 1599) or Resetnow;
    write(lf);  if not local and linefeeds then charout(lf);
    if (l = 1) or (p > 1599) then done := true else Msgbuff[p] := #32;
  end until done or Resetnow;
  done := false;
  if not Resetnow then
  repeat begin
    str(p,stringout);
    stringout := ' (' + stringout + '/1600 chars in buffer)';
    if p > 1000 then lineout(Stringout,ys);
    if not expert then Fileout(AddDrive(menudrv,'MENUEDIT'))
    else lineout(' EDIT: A C L X ? Q S ',no);
    lineout('>',no);
    inputchar(ys);
    q := Upcase(q);
    lineout(' ',ys);
    if not Resetnow then
      case q of
        'X' : expert := not expert;
        'A' : begin
               lineout(' ',ys);
               lineout(' Enter string to search for >',no);
               linein(80,ys,ys);
               if not Resetnow then begin
                x := 0;
                if Length(stringin) > 0 then repeat
                  begin
                    y := 0;
                    z := x;
                    x := x + 1;      {Scanning MsgBuff}
                    repeat
                      begin
                        y := y + 1;
                        z := z + 1;
                        q := Copy(stringin,y,1);
                      end
                    until (q <> MsgBuff[z]) or (y >= Length(stringin));
                    if q <> MsgBuff[z] then z := 0;
                  end
                until (z > 0) or (x > (p - Length(stringin)));
                if z < 1 then x := 0;
                if x > 0 then
                  begin
                    y := Length(stringin);
                    p := p - y;
                    for z := x to p do MsgBuff[z] := MsgBuff[z + y];
                    lineout(' Enter replacement string ',no);
                    y := 1600 - p;
                    if y > 255 then y := 255;
                    if y < 255 then lineout('E',no);
                    lineout('>',no);
                    linein(y,ys,ys);
                    y := Length(stringin);
                    if (y > 0) and not Resetnow then
                      begin
                        for z := p downto x do MsgBuff[z + y] := MsgBuff[z];
                        p := p + y;
                        for z := 0 to (y-1) do
                          MsgBuff[x + z] := Copy(stringin,(z+1),1);
                      end;
                  end
                else lineout(' String not found.',ys);
               end;
               q := 'A';
              end;
        'C' : begin
                lineout(' Continuing entry...',ys);
                p := p - 1;  {Removes the CR that got us here.}
              end;
        'L' : Outbuff;
        'Q' : begin
                p := 0;  done := true;
              end;
        'S' : done := true;
        '?' : Fileout(AddDrive(helpdrv,'HELPEDIT'));
      end;
  end until (q = 'C') or (q = 'Q') or (q = 'S') or Resetnow;
 end until Resetnow or done;
 done := false;
end;
