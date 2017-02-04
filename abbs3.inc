{This function takes a drive number (0 if use current drive) and adds
the appropriate drive letter and a colon to the supplied filename}
Function AddDrive(x : Integer; s : string14) : string14;
begin
  if (x > 0) and (x < 17) then
    AddDrive := Copy('ABCDEFGHIJKLMNOP',x,1) + ':' + s
  else AddDrive := s;
end;

{Following proc outputs a disk textfile - extension is forced to .TXT}
Procedure Fileout(x : string14);
begin
  assign(FilVar,x + '.TXT');
  {$I-} reset(FilVar) {$I+};
  OK := (IOresult = 0);
  if not OK then lineout(x + '.TXT was not found!',ys) else begin
    q := '?';
    lineout(' ',ys);
    while not Eof(Filvar) and not (q in ['S','s']) and not Resetnow do begin
      readln(FilVar,Stringin);
      lineout(Stringin,ys);
    end;
    close(FilVar);
  end;
end;
