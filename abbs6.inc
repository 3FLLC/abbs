{And here's the good old C)hat routine.}
Overlay Procedure Chat;
var z : Integer;
    echo, show : Boolean;
begin
  stringout := ' calling...(Ringing 5 times)';
  if not local then stringout := ' ' + name + stringout;
  lineout(Stringout,ys);
  echo := ys;
  z := 1;
  repeat
    begin
      z := z + 1;
      lineout(#07 + ' Ring...  ',no);
      delay(1000);
    end
  until (z > 5) or Keypressed;
  lineout(cr+lf+' Terminate with ESCape or ctrl-C if nobody answers.',ys);
  lineout(' ',ys);
  if local then begin
    show := ys; echo := no;
    write('Go 1200 baud ? ');
    read(q); q := Upcase(q);
    if q = 'Y' then begin setbaud(1200); writeln(lf,cr,'1200 baud...'); end
    else writeln(lf,cr,'300 baud...');
    writeln('Control-D toggles local echo.');
  end;
  repeat
    begin
      inputchar(echo);
      if local then
        if sysop then
          begin
            if q = ^D then show := not show
            else charout(q);
            if show then write(q);
          end
        else write(q)
      else if q = cr then
        begin
          write(lf);
          if linefeeds then charout(lf);
        end;
    end
  until (q = #27) or (q = ^C) or Resetnow;
  lineout(' ',ys);
end;

{This is the Feedback routine.  It accepts a message and sends it to sysop.}
Overlay Procedure Feedback;
Type    MsgRec  = Record
                MsgPtr : Integer;
                Msgchars : Array[1..126] of char;
        end;
        HdrRec  = Record                 {53 bytes}
                HdrMsgKey : String[8];   {The key (hall room ('101') event)}
                HdrMsgPtr : Integer;     {Points to the message}
                HdrExcl   : Boolean;     {Is this E-mail? (has 2nd index)}
                HdrMsgTo  : String[9];   {To UserKey - 2nd index}
                HdrMsgFrom: String[31];  {From UserName}
        end;
        UserRec = record
                Uaccess    : Integer;       {+  2   2 }
                Upwd       : String[9];     {+ 10  12 }
                Uname      : String[31];    {+ 32  44 }
                Uphone     : String[12];    {+ 13  57 }
                Ulinefeeds : Boolean;       {+  1  58 }
                Uwidth     : Integer;       {+  2  60 }
                Unulls     : Integer;       {+  2  62 }
                Ulowcase   : Boolean;       {+  1  63 }
                Uevent     : Integer;       {+  2  65 }
                Ucalls     : Integer;       {+  2  67 }
                Uposts     : Integer;       {+  2  69 }
                Uploads    : Integer;       {+  2  71 }
                Udloads    : Integer;       {+  2  73 }
                Uexpert    : Boolean;       {+  1  74 }
        end;
Var     Message : file;              {Contains raw message data}
        Messhdr : DataFile;              {Points into Message}
        Messptr : IndexFile;             {Points into Messhdr}
        Emssptr : IndexFile;             {Points into Messhdr}
        Omssptr : IndexFile;             {Points into Messhdr}
        MsgVar  : MsgRec;                {MESSAGE.DAT recl}
        HdrVar  : HdrRec;                {MESSHDR.DAT recl}
        CurrMess: Integer;               {                      mess}
        Emess   : Integer;               {Total excl messages kept.}
        scratch : String[32];            {Scratch key}
        Userlog : DataFile;
        Userpwd : IndexFile;
        Usernam : IndexFile;
        User    : UserRec;
        Userec  : Integer;          {Data record number of user}

{Function AddMsg - takes p & MsgBuff, and returns the beginning record of the
message in Message.}
Function AddMsg : Integer;
var n, i, r, thisrec, nextempty : Integer;
begin
  n := 0;
  Seek(Message,0);
  BlockRead(Message,MsgVar,1);
  nextempty := MsgVar.Msgptr;
  if nextempty = 0 then AddMsg := FileSize(Message) else AddMsg := nextempty;
  y := 0;
  r := trunc((p+125)/126);
  if r = 0 then r := 1;
  currrecs := currrecs + r;
  for z := 1 to r do
    begin
      x := y + 1;
      y := x + 125;
      if p < y then y := p;
      if nextempty = 0 then
        begin
          MsgVar.Msgptr := 0;
          thisrec := FileSize(Message);
          MsgVar.MsgPtr := thisrec + 1;
        end
      else
        begin
          Seek(Message,nextempty);
          BlockRead(Message,MsgVar,1);
          thisrec := nextempty;
          nextempty := MsgVar.MsgPtr;
          if nextempty = 0 then MsgVar.Msgptr := FileSize(Message);
        end;
      if z = r then MsgVar.Msgptr := 0;
      i := 0;
      for n := x to y do
        begin
          i := i + 1;
          with MsgVar do MsgChars[i] := MsgBuff[n];
        end;
      if i < 126 then with MsgVar do MsgChars[i+1] := #$00;
      Seek(Message,thisrec);
      BlockWrite(Message,MsgVar,1);
    end;
  MsgVar.Msgptr := nextempty;
  Seek(Message,0);
  BlockWrite(Message,MsgVar,1);
end;

{Procedure DelMsg - takes the starting record number of a message and places
all of the records comprising that message on the empty record list.}
Procedure DelMsg(var delrec : Integer);
var n, nextempty : Integer;
begin
  if delrec = 0 then lineout('From delrec: Delete record 0?',ys);
  n := 0;
  Seek(Message,n);
  BlockRead(Message,MsgVar,1);
  nextempty := MsgVar.Msgptr;
  while delrec > 0 do
    begin
      currrecs := currrecs - 1;
      Seek(Message,delrec);
      BlockRead(Message,MsgVar,1);
      n := MsgVar.Msgptr;
      MsgVar.Msgptr := nextempty;
      nextempty := delrec;
      Seek(Message,delrec);
      BlockWrite(Message,MsgVar,1);
      delrec := n;
    end;
  MsgVar.Msgptr := nextempty;
  Seek(Message,n);
  BlockWrite(Message,MsgVar,1);
end;

{Prior to calling KillMsg, the Messhdr record must have been read}
Procedure KillMsg;
begin
  z := HdrVar.HdrMsgPtr;
  DelMsg(z);
  DeleteRec(Messhdr,CurrMess);
  DeleteKey(Messptr,CurrMess,HdrVar.HdrMsgKey);
  if not OK then lineout(' Error on msg index scroll.',ys);
  if HdrVar.HdrExcl then begin
    scratch := HdrVar.HdrMsgTo;
    DeleteKey(Emssptr,CurrMess,scratch);
    if not OK then lineout(' Error on emsg index scroll.',ys);
  end;
  scratch := Copy(HdrVar.HdrMsgKey,4,5);
  DeleteKey(Omssptr,CurrMess,scratch);
  if not OK then lineout(' Error on omsg index scroll.',ys);
end;

Procedure Scroll;
begin
  scratch := '00000';
  SearchKey(Omssptr,CurrMess,scratch);
  if OK then
    begin
      GetRec(Messhdr,CurrMess,HdrVar);
      KillMsg;
    end;
end;

Procedure GetEventStr(e, l : Integer);
begin
  str(e,scratch);
  while length(scratch) < l do scratch := '0' + scratch;
end;

begin
  InitIndex;
  OpenFile(Messhdr,AddDrive(namedrv,'MESSNAM.DAT'),SizeOf(HdrVar));
  OpenIndex(Messptr,AddDrive(keyfdrv,'MESSPTR.DAT'),9,0);
  OpenIndex(Emssptr,AddDrive(keyfdrv,'MESSEPTR.DAT'),10,1);
  OpenIndex(Omssptr,AddDrive(keyfdrv,'MESSOPTR.DAT'),6,0);
  Assign(Message,AddDrive(messdrv,'MESSAGE.DAT'));
  Reset(Message);
  lineout(' Sending private message to system operator.',ys);
  writeln(':',name);
  stringin := SysopName;
  scratch := '';
  if length(stringin) > 0 then
   for x := 1 to length(stringin) do
    scratch := scratch + Upcase(copy(stringin,x,1));
  if scratch = 'ALL' then scratch := '';
  stringin := scratch;
  HdrVar.HdrExcl    := false;
  HdrVar.HdrMsgTo   := '';
  if (length(stringin) > 0) and not Resetnow then
    begin
      OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
      OpenIndex(Usernam,AddDrive(userdrv,'USERNAM.DAT'),32,0);
      scratch := stringin;
      SearchKey(Usernam,Userec,scratch);
      if OK then begin
        GetRec(Userlog,Userec,User);
        scratch := '';
        for x := 1 to length(User.Uname) do
          scratch := scratch + Upcase(copy(User.Uname,x,1));
        q := '~';
        if length(stringin) < length(scratch) then
          if stringin = copy(scratch,1,length(stringin)) then begin
            stringout := ' Send to ' + User.Uname + ' ?';
            lineout(stringout,no);
            Inputchar(ys); q := Upcase(q);
          end;
        if (stringin = scratch) or (q = 'Y') then begin
          HdrVar.HdrExcl  := true;
          HdrVar.HdrMsgTo := User.Upwd;
          q := 'Y';
        end else OK := no;
        if q = '~' then lineout(' Sysop not in userlog.',ys);
      end else lineout(' Sysop not in userlog.',ys);
      CloseFile(Userlog);
      CloseIndex(Usernam);
    end;
  if OK and not Resetnow then
    begin
      p := 0;  {Clear buffer.}
      EditBuff;
      if (q = 'S') and not Resetnow then
        begin
          event := event + 1;
          posts := posts + 1;
          GetEventStr(event,5);
          scratch := '101' + scratch;
          HdrVar.HdrMsgKey  := scratch;
          HdrVar.HdrMsgFrom := name;
          HdrVar.HdrMsgPtr  := AddMsg;
          AddRec(Messhdr,CurrMess,HdrVar);
          AddKey(Messptr,CurrMess,HdrVar.HdrMsgKey);
          if HdrVar.HdrExcl then
            AddKey(Emssptr,CurrMess,HdrVar.HdrMsgTo);
          scratch := Copy(HdrVar.HdrMsgKey,4,5);
          AddKey(Omssptr,CurrMess,scratch);
          while OK and ((currrecs+14)>messrecs) do Scroll;
          if not OK then
            lineout(' All public messages scrolled!',ys)
          else lineout(' Message saved.',ys);
        end
      else lineout(' Message tossed.',ys);
    end;
  q := 'F';
  Close(Message);
  CloseFile(Messhdr);
  CloseIndex(Messptr);
  CloseIndex(Emssptr);
  CloseIndex(Omssptr);
end;

{This procedure gives a menu of text files available for output.}
Overlay Procedure Bulletins;
begin
  repeat
    begin
      if not expert then Fileout(AddDrive(menudrv,'MENUBULL'))
      else lineout(' BLTN: B H I F N P W ? X Q ',no);
      lineout('>',no);
      inputchar(ys);
      q := Upcase(q);
      lineout(' ',ys);
      if not Resetnow then
      case q of
        'B' : Fileout(AddDrive(bulldrv,'BULLETIN'));
        'H' : Fileout(AddDrive(helpdrv,'HELP'));
        'I' : Fileout(AddDrive(bulldrv,'INFO'));
        'F' : Fileout(AddDrive(bulldrv,'FUNNIES'));
        'N' : Fileout(AddDrive(bulldrv,'NUMBERS'));
        'P' : Fileout(AddDrive(bulldrv,'POLICY'));
        'W' : Fileout(AddDrive(bulldrv,'WELCOME'));
        '?' : Fileout(AddDrive(helpdrv,'HELPBULL'));
        'X' : expert := not expert;
      end;
    end
  until Resetnow or (q = 'Q');
end;
