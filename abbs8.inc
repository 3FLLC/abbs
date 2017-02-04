Overlay Procedure Message;
Type    MsgRec  = Record
                MsgPtr : Integer;
                Msgchars : Array[1..126] of char;
        end;
        HdrRec  = Record                 {53 bytes}
                HdrMsgKey : String[8];   {The key (hall room event)}
                HdrMsgPtr : Integer;     {Points to the message}
                HdrExcl   : Boolean;     {Is this E-mail? (has 2nd index)}
                HdrMsgTo  : String[9];   {To UserKey - 2nd index}
                HdrMsgFrom: String[31];  {From UserName}
        end;
        RoomRec = Record                 {24 bytes}
                RoomKey   : String[3];   {Room key (hall room#)}
                Roomname  : String[22];
                RoomNumber: Integer;     {2 digit room number}
                RoomAccess: Integer;     {Access required to enter room}
                RoomHidden: Boolean;     {Don't list in hall}
        end;
        HallRec = Record                 {35 bytes}
                HallKey   : String[1];   {only 1-9 halls}
                Hallname  : String[30];
                Hallaccess: Integer;
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
        Roomnam : DataFile;              {Contains room data}
        Hallnam : DataFile;              {Contains hall data}
        Messptr : IndexFile;             {Points into Messhdr}
        Emssptr : IndexFile;             {Points into Messhdr}
        Omssptr : IndexFile;             {Points into Messhdr}
        Roomptr : IndexFile;             {Points into Roomnam}
        Hallptr : IndexFile;             {Points into Hallnam}
        MsgVar  : MsgRec;                {MESSAGE.DAT recl}
        HdrVar  : HdrRec;                {MESSHDR.DAT recl}
        RoomVar : RoomRec;               {ROOMNAM.DAT recl}
        HallVar : HallRec;               {HALLNAM.DAT recl}
        CurrHall: Integer;               {Data record number of hall}
        CurrRoom: Integer;               {                      room}
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

{Procedure GetMsg - takes the starting record number of a message and loads it
into MsgBuff.}
Procedure GetMsg(var themsg : Integer);
begin
  p := 0;
  repeat
    begin
      Seek(Message,themsg);
      BlockRead(Message,MsgVar,1);
      themsg := MsgVar.Msgptr;
      x := 0;
      repeat
        begin
          p := p + 1;
          x := x + 1;
          MsgBuff[p] := MsgVar.MsgChars[x];
        end
      until (MsgBuff[p] = #$00) or (x = 126) or (p=1600);
    end
  until (themsg = 0) or (p=1600);
  if MsgBuff[p] = #$00 then p := p - 1;
end;

{Prior to calling KillMsg, the Messhdr record must have been read}
Procedure KillMsg;
begin
  z := HdrVar.HdrMsgPtr;
  DelMsg(z);
  DeleteRec(Messhdr,CurrMess);
  DeleteKey(Messptr,CurrMess,HdrVar.HdrMsgKey);
  if not OK then lineout('Error 4',ys);
  if HdrVar.HdrExcl then begin
    scratch := HdrVar.HdrMsgTo;
    DeleteKey(Emssptr,CurrMess,scratch);
    if not OK then lineout('Error 5',ys);
  end;
  scratch := Copy(HdrVar.HdrMsgKey,4,5);
  DeleteKey(Omssptr,CurrMess,scratch);
  if not OK then lineout('Error 6',ys);
end;

Procedure Scroll;
begin
  scratch := '00000';
  SearchKey(Omssptr,CurrMess,scratch);
  if OK then begin
    GetRec(Messhdr,CurrMess,HdrVar);
    KillMsg;
  end;
end;

Procedure GetEventStr(e, l : Integer);
begin
  str(e,scratch);
  while length(scratch) < l do scratch := '0' + scratch;
end;
