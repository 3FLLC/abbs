Overlay Procedure Utility;   {Sysop maintenance utilities}
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
        RoomRec = Record                 {32 bytes}
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
        scratch : String[32];            {Scratch key}
        scratch2: String[32];            {Another}
        Userlog : DataFile;
        Userpwd : IndexFile;
        Usernam : IndexFile;
        User    : UserRec;
        Userec  : Integer;          {Data record number of user}

{Takes 'event' and puts it into 'scratch', right justified and zero padded.}
Procedure GetEventStr(e, l : Integer);
begin
  str(e,scratch);
  while length(scratch) < l do scratch := '0' + scratch;
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

{Procedure DelMsg - takes the starting record number of a message and places
all of the records comprising that message on the empty record list.}
Procedure DelMsg(var delrec : Integer);
var n, nextempty : Integer;
begin
  n := 0;
  if delrec = 0 then writeln('From delrec:  Delete record 0?');
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
  if not OK then lineout(' Error on msg index delete.',ys);
  if HdrVar.HdrExcl then begin
    scratch := HdrVar.HdrMsgTo;
    DeleteKey(Emssptr,CurrMess,scratch);
    if not OK then lineout(' Error on emsg index delete.',ys);
  end;
  scratch := Copy(HdrVar.HdrMsgKey,4,5);
  DeleteKey(Omssptr,CurrMess,scratch);
  if not OK then lineout(' Error on omsg index delete.',ys);
end;

{MsgHdr must have already been read.}
Procedure DoMess;
begin
  if HdrVar.HdrExcl then
   stringout := ' Private message from '
  else stringout := ' Public message from ';
  with HdrVar do
   stringout := stringout+HdrMsgFrom+' in room '+HdrMsgKey;
  lineout(stringout,ys);
  lineout(' View message [Y] ?',no);
  Inputchar(ys);
  if q = cr then q := 'Y' else q := Upcase(q);
  lineout(' ',ys);
  if not Resetnow and (q = 'Y') then
   begin
    z := HdrVar.HdrMsgPtr;
    GetMsg(z);
    OutBuff;
    lineout(' ',ys);
   end;
  if not Resetnow then
   begin
    lineout(' Kill message [N] ?',no);
    Inputchar(ys);
    if q = cr then q := 'N' else q := Upcase(q);
    lineout(' ',ys);
    if not Resetnow and (q = 'Y') then KillMsg;
   end;
  if not Resetnow then
   begin
    lineout(' Seek next message [Y] ?',no);
    Inputchar(ys);
    if q = cr then q := 'Y' else q := Upcase(q);
    lineout(' ',ys);
   end;
end;
