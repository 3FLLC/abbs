{Following proc is not machine dependant - it returns true if the two
bytes contain at least one pair of equal-significance bits that are 1.
So, 10000000 (128 decimal) and the unknown pattern would return true if
the far left bit in the unknown pattern was a 1.}
Function Testbit(x,y : byte): Boolean;
begin
  testbit := false;
  if (x and y) > 0 then testbit := true;
end;

{Following proc is machine dependant - mask ~DCD of the modem status port.}
Function Carrier: Boolean;
begin
  Port[04] := $00;
  carrier := not testbit(Port[04],4) and not testbit(Port[04],4);
end;

{Following proc is machine dependant - read the modem status port and
mask bit that says "a char is ready".  This is basically the same thing
as the Turbo value "KeyPressed", except it's for the modem instead of CON:
Figuring out the necessity of STAT0 reset was a bear.}
Function Chaready: Boolean;
begin
  Port[04] := $00;                     {Port 04 needs this to speed up the}
  Chaready := testbit(Port[04],128);   {setting of RDRF bit in STAT0.     }
end;

{Following proc is machine dependant - read a char from the modem input
port (resets char-ready status port; you may need to do so "manually")
The chaready port should have been read true right before hitting this,
so that it's ok to reset 'tout' at this time.}
Function Charin: Char;
begin  Charin := chr(Port[08]);  end;  {Pull that character in!}

{Following proc is machine dependant - test status port over and over
until it's ok to write a character to modem output port.  Count up to
500 waiting for the output port to free up... then give it up.}
Procedure Charout(x : char);
begin
  tout := 0;
  repeat
    begin
      tout := tout + 1;
      Port[04] := $00;
    end
  until testbit(Port[04],2) or (tout > 500);
  Port[06] := ord(x);
end;

{The following routine is definitely machine dependant.  You'll have to
write to whatever register(s) to set your protocol and baud rate.}
Procedure Setbaud(x : integer); { Sets 8,N,1 & baudrate }
begin
  Port[00] := $6C;              {Sets protocol 8,N,1 in CNTLA0 register}
  case x of                     {Sets baudrate selected in CNTLB0 reg.}
     150 : Port[02] := $0E;
     300 : Port[02] := $0D;
     600 : Port[02] := $0C;
    1200 : Port[02] := $0B;
    2400 : Port[02] := $0A;
  end;
end;

{Following routine is machine dependant - it gets the time and date
from whatever peripheral hardware/software is out there.  The format
of the timestamp is whatever you want it to be, as the string will simply
be written into the message buffer as the first line of the message.
This routine returns a 16 character timestamp, and also sets:
minute, day, date, month - integer values. }
Function Time : String16;
var     hh : byte absolute $EB02;
        mm : byte absolute $EB01;
        stringin, stringout : String14;
begin
  repeat begin
    x := hh; y := mm; minute := x * 60 + y;
  end until minute = hh * 60 + mm;
  if minute < oldminute then begin
    day := day + 1; if day > 7 then day := 1;
    {bump date & month here too}
  end;
  oldminute := minute;
  str(x,stringin); str(y,stringout);
  if length(stringin) < 2 then stringin := '0' + stringin;
  if length(stringout) < 2 then stringout := '0' + stringout;
  stringin  := stringin + ':' + stringout + ' ';
  stringout := Copy('SunMonTueWedThuFriSat',day*3-2,3);
{ stringin  := stringin + stringout; }
{ stringout := Copy('JanFebMarAprMayJunJlyAugSepOctNovDec',month*3-2,3); }
  Time      := stringin + stringout;
end;
