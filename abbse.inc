{File header & category separator:  File key:  ##FILENAMEEXT 14 bytes.
## is the file category.}

Overlay Procedure Files;
Const   soh = $01; stx = $02; eot = $04; ack = $06;
        nak = $15; can = $18; ltrC = $43;
Type    FileNamRec = Record
                FileKey    : String[13];  {The key (room filename)}
                FileDate   : String[6];   {Date entry was made}
                FileDrive  : Char;        {Which drive file resides on}
                FileDesc   : String[40];  {Description}
                FileSize   : Integer;     {Num of records in file}
                FilePass   : String[6];   {Password to get file}
                FileXfers  : Integer;     {Number of downloads made}
                FileHidden : Boolean;     {Don't show in directory}
                FileAcc300 : Integer;     {\                       }
                FileAcc1200: Integer;     { >--- Access levels required}
                FileAcc2400: Integer;     {/                       }
        end;
        TypeNamRec = Record
                TypeKey    : String[2];   {The key (Typecode)}
                TypeName   : String[20];  {Name of category}
                TypeAccess : Integer;     {Access level required}
        end;
        FileKey = string[12];
Var     Filenam : DataFile;              {Points to files}
        Fileptr : IndexFile;             {Points into Filenam}
        Typenam : DataFile;              {Names of file "rooms"}
        Typeptr : IndexFile;             {Points into Typenam}
        FileVar : FileNamRec;            {FILENAM.DAT recl}
        TypeVar : TypeNamRec;            {TYPENAM.DAT recl}
        scratch : String[32];            {Scratch key}
        CurrType: Integer;
        CurrFile: Integer;
        Diskfile: File;
        blocknum: Integer;
        redos   : Integer;
        crc     : Integer;
        checksum: Byte;
        indata  : Byte;
        intries : Byte;                  {Counts attempts to get indata}
        infail  : Boolean;               {Set if one Inchar timeout occurs}
        doingcrc: Boolean;               {Set if CRC protocol requested}
        Buffer  : Array[0..127] of byte;

Procedure GetEventStr(e, l : Integer);
begin
  str(e,scratch);
  while length(scratch) < l do scratch := '0' + scratch;
end;

{Following function verifies access to a file}
Function FileAccess : Boolean;
begin
  if ((access > 90) or
  ((baud=300)  and (access >= FileVar.FileAcc300)) or
  ((baud=1200) and (access >= FileVar.FileAcc1200)) or
  ((baud=2400) and (access >= FileVar.FileAcc2400))) then
    if (length(FileVar.Filepass) = 0) or (access > 90) then FileAccess := true
    else begin
      lineout(' File password >',no);
      if not Resetnow then linein(6,ys,ys);
      if stringin = FileVar.Filepass then FileAccess := true
      else begin
        lineout(' * BAD PASSWORD *',ys);
        FileAccess := false;
      end;
    end
  else FileAccess := false;
end;

{Turn a bozo's filename into a useable filename}
Function EditFileName(xs : FileKey) : FileKey;
begin
  done := no;
  scratch := '';
  x := 0;
  if length(xs) > 0 then repeat begin
    x := x + 1;
    q := copy(xs,x,1);
    if q in ['A'..'Z','0','1'..'9','a'..'z','.',' '] then begin
      if (q in ['.',' ']) and not done then begin
        done := ys; q := ' ';
        scratch := scratch + '.';
      end else if not (q in ['.',' ']) then scratch := scratch + Upcase(q);
    end;
  end until (x = length(xs)) or (done and (q = '.'));
  q := ' ';
  x := Pos('.',scratch);
  if x = 0 then begin
    if length(scratch) < 9 then xs := scratch + '.'
    else xs := Copy(scratch,1,8) + '.' + Copy(scratch,9,3);
  end else xs := Copy(scratch,1,x) + Copy(scratch,x+1,3);
  EditFileName := xs;
  done := no;
end;

{Turn a legal filename into a key}
Function MakeFileKey(xs : FileKey) : FileKey;
begin
  scratch := copy(xs,1,(Pos('.',xs)-1));
  while length(scratch) < 8 do scratch := scratch + ' ';
  scratch := scratch + copy(xs,(Pos('.',xs)+1),(length(xs)-Pos('.',xs)));
  while length(scratch) < 11 do scratch := scratch + ' ';
  MakeFileKey := scratch;
end;

Procedure GetFileName;
begin
  lineout(' Name of file >',no);
  if not Resetnow then linein(12,ys,ys);
  stringin := TypeVar.TypeKey + MakeFileKey(EditFileName(stringin));
  scratch  := stringin;
end;

{*** The following are character I/O routines specific to binary transfer ***}

Function Inbyte : Byte;  {Gets a data byte from the input port or set infail}
begin
  intries := intries + 1;
  tout    := 0;
  infail  := no;
  repeat begin
    tout := tout + 1;
    if chaready then begin Inbyte := ord(charin); tout := 0; end;
  end until (tout = 0) or (tout > intimout) or not carrier;
  if not carrier or (tout > intimout) then infail := ys;
end;

{Here's the archaic crc routine...to be replaced by table!}
Function CalcCRC(indata : byte; crc : integer) : Integer;
var carry : boolean;
begin
  for y := 0 to 7 do begin
    carry := (crc and $8000) <> 0;
    crc := crc shl 1;
    if (indata and $80) <> 0 then crc := crc or $0001;
    if carry then crc := crc xor $1021;
    indata := lo(indata shl 1);
  end;
  CalcCRC := crc;
end;

{*** The following routines EditFileEntry, Asciixfer, Xmodemsend should all
     be overlays, which would allow room for the addition of other overlayed
     protocol servers. ***}

Procedure EditFileEntry;
begin
  with FileVar do begin
    lineout(' ',ys);
    if access > 98 then begin
      stringout := ' Disk drive       = ' + FileDrive + ' >';
      lineout(stringout,no);
      if not Resetnow then linein(1,ys,ys);
      if length(stringin) = 1 then FileDrive := Upcase(Copy(stringin,1,1));
    end;
    if access > 98 then begin
      stringout := ' Hidden           = ';
      if FileHidden then stringout := stringout + 'Yes >'
      else stringout := stringout + 'No >';
      lineout(stringout,no);
      if not Resetnow then Inputchar(ys);
      lineout(' ',ys);
      if q in ['N','n'] then FileHidden := no;
      if q in ['Y','y'] then FileHidden := ys;
    end;
    if access > 98 then begin
      str(FileAcc300,stringout);
      stringout := ' 300 baud access  = ' + stringout + ' >';
      lineout(stringout,no);
      if not Resetnow then x := Intin;
      if x in [0..99] then FileAcc300 := x;
      str(FileAcc1200,stringout);
      stringout := ' 1200 baud access = ' + stringout + ' >';
      lineout(stringout,no);
      if not Resetnow then x := Intin;
      if x in [0..99] then FileAcc1200 := x;
      str(FileAcc2400,stringout);
      stringout := ' 2400 baud access = ' + stringout + ' >';
      lineout(stringout,no);
      if not Resetnow then x := Intin;
      if x in [0..99] then FileAcc2400 := x;
    end;
    if access > 90 then begin
      stringout := ' Password         = ' + FilePass + ' >';
      lineout(stringout,no);
      if not Resetnow then linein(6,ys,ys);
      if length(stringin) > 0 then FilePass := stringin;
    end;
    if access > 90 then begin
      stringout := ' Upload date      = ' + FileDate + ' >';
      lineout(stringout,no);
      if not Resetnow then linein(6,ys,ys);
      if length(stringin) = 6 then FileDate := stringin;
    end;
    if FileAccess then begin
      lineout('File description :',ys);
      lineout(FileDesc,ys);
      if not Resetnow then linein(40,ys,ys);
      if length(stringin) > 0 then FileDesc := stringin;
    end;
  end;
  scratch := FileVar.FileDrive+':'+EditFileName(Copy(FileVar.FileKey,3,11));
  assign(Diskfile,scratch);
  {$I-} Reset(Diskfile) {$I+};
  if IOresult = 0 then FileVar.FileSize := FileSize(Diskfile) else
  if access > 90 then begin
    lineout(' File not found, manual entry required.',ys);
    str(FileVar.FileSize,stringout);
    stringout := ' File size        = ' + stringout + ' >';
    lineout(stringout,no);
    if not Resetnow then x := Intin;
    if (x > 0) and (x < 32001) then FileVar.FileSize := x;
  end;
end;

{Following proc outputs a file, assumes ascii.}
Procedure Asciixfer(x : string14);
begin
  assign(FilVar,x);
  {$I-} reset(FilVar) {$I+};
  if IOresult <> 0 then lineout(' Cannot find file on disk.',ys) else begin
    q := '?'; lineout(' ',ys);
    while not Eof(Filvar) and not (q in ['S','s']) and not Resetnow do begin
      readln(FilVar,Stringin);
      lineout(Stringin,ys);
    end;
    close(FilVar);
  end;
end;

{Following procedure performs an Xmodem download given the filename.}
Procedure Xmodemsend(xs : string14);
begin
  assign(Diskfile,xs);
  {$I-} reset(Diskfile) {$I+};
  if IOresult <> 0 then lineout(' Cannot find file on disk.',ys) else begin
    str(FileVar.FileSize,stringout);
    lineout(' XMODEMing you ' + stringout + ' records.',ys);
    lineout(' Hit ctrl-X repeatedly to cancel.',ys);
    doingcrc := false;
    blocknum := 0;
    redos    := 0;      {Number of times a block is retransmitted}
    intries  := 0;      {Number of times infail is set}
    {Initial handshake; establish whether CRC CCITT or checksum protocol}
    repeat begin
      indata := Inbyte;
      if infail then writeln(' Timeout',intries:2);
    end until ((indata in [nak,ltrC,can]) and not infail) or (intries > 4);
    if infail then writeln(' Receiver timed out.') else
    if indata = ltrC then begin
      doingcrc := true;
      writeln(' CRC CCITT protocol.');
    end else writeln(' Checksum protocol.');
    if indata <> can then indata := ack;
    if not infail then
    repeat begin
      if indata = ack then
        if Eof(Diskfile) then done := true else begin
          redos := 0;
          blocknum := blocknum + 1;
          write(cr + ' Sending #',blocknum:4);
          {$I-} BlockRead(Diskfile,Buffer[0],1) {$I+};
        end;
      if (indata <> can) and not done and not Resetnow then begin
        crc := 0; checksum := 0;        {Clear error checking stuff}
        {Send Xmodem header}
        charout(chr(soh));              {beginning of data block}
        charout(chr(lo(blocknum)));     {Lower byte of block number}
        charout(chr(255-lo(blocknum))); {Complement of lower byte of blocknum}
        {Send the 128 bytes of data}
        for x := 0 to 127 do begin
          charout(chr(Buffer[x]));
          checksum := lo(checksum + Buffer[x]);
          crc := CalcCRC(Buffer[x],crc);
        end;
        crc := CalcCRC(0,crc); crc := CalcCRC(0,crc); {Why is this needed???}
        {Send the error check stuff}
        if not doingcrc then charout(chr(checksum))
        else begin
          charout(chr(hi(crc))); charout(chr(lo(crc)));
        end;
        {Get ack, nak, or can in response to packet just sent}
        intries := 0;
        repeat begin
          indata := Inbyte;
          if infail then write(cr+lf+' Timeout ',intries:2);
        end until ((indata in [ack,nak,can]) and not infail) or (intries > 3);
        redos := redos + 1;
        if (redos > 5) and doingcrc then begin
          write(cr+lf+' Reverting to checksum.');
          doingcrc := false;
        end;
        if indata = nak then write(cr+lf+' Bad block ',redos:2);
      end;
    end until (indata = can) or (redos > 8) or done or infail;
    Close(Diskfile);
    delay(200);
    if done then begin
      charout(chr(eot));
      writeln(cr+lf+' Sent end of transmission.');
      indata := Inbyte;
      if infail or (indata <> ack) then begin
        charout(chr(eot));
        writeln(' No response.  Sent 2nd EOT.');
      end;
    end else begin
      charout(chr(can)); delay(50); charout(chr(can));
      writeln(cr+lf+' Sent cancel.');
    end;
    done := false;
    delay(1500); {1.5 seconds to allow their terminal to finish xmodem}
  end;
end;
