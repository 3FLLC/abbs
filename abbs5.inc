{Following routine is machine dependant - it raises the ~RTS line long
enough to tell the modem to hit the cradle, give it up, go to sleep.}
Procedure Hangup;
begin
  if resetnow then lineout(' Timed out.  Disconnecting...',ys) else
    if q = 'T' then lineout(' Goodbye... *snurf*',ys);
  Port[00] := $10;   { Raise ~RTS line }
  Delay(500);        { for half a sec. }
  setbaud(baud);     { And drop it to ready modem.}
  Delay(1250);       { Escape code guard time.}
  charout('+'); charout('+'); charout('+');
  Delay(1250);       { Should give us the OK prompt}
  if chaready then q := charin;
  setbaud(2400);     { Now go for higher baud rate}
  charout('A'); charout('T'); charout('Z'); charout(cr);
  Delay(1000);
  if chaready then q := charin;
  q := 'T';
end;

{Following proc is modem dependant - disconnect, reset modem, and wait
for carrier.  Automatically adjust to incoming baud rate and reset DCD.}
Overlay Procedure Initmdmandwait;
begin
  setbaud(2400);   { 2400 8,N,1 }
  lineout('ATZ',ys);
  delay(1000);
  if chaready then q := charin;
  lineout('ATS2=0S10=12',ys);
  delay(1000);
  if chaready then q := charin;
  lineout('AT&C1&D3E0V0S0=1',ys);   { Numeric codes, no echo, AA }
  for x := 1 to 27 do write(lf);
  repeat begin
    q := #00;
    stringin := '';
    repeat begin
      if Keypressed then read(kbd,q);
      if chaready then q := charin;
    end until q <> #00;
    if not (q in [#00,cr]) then stringin := q;
    q := #00;
    tout := 0;
    repeat begin
      tout := tout + 1;
      if Keypressed then read(kbd,q);
      if chaready then q := charin;
    end until (q <> #00) or (tout > 250); {Experimental value}
    if not (q in [#00,cr]) then stringin := stringin + q;
    if stringin = '' then local := true;
    if stringin = '0' then writeln('.');
    if stringin = '2' then writeln('Phone ringing.');
    if stringin = '3' then writeln('Voice call or carrier lost.');
    if stringin = '1' then begin  baud := 300;  done := true; end;
    if stringin = '5' then begin  baud := 1200; done := true; end;
    if stringin = '10' then begin baud := 2400; done := true; end;
  end until done or local;
  if not local then begin
    setbaud(baud); writeln(baud,' baud.');
  end
  else begin
    writeln(' 9600 baud local.');
    Hangup;
  end;
  done := false; Resetnow := no;
end;
