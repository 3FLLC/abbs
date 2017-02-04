Program ABBSINIT;

var     ys,no     : Boolean;
        done      : Boolean;
        linefeeds : Boolean;
        lowcase   : Boolean;
        width     : Integer;
        nulls     : Integer;
        calls     : Integer;
        posts     : Integer;
        ploads    : Integer;
        dloads    : Integer;
        expert    : Boolean;
        access    : Integer;
        logons    : Integer;
        event     : Integer;
        messrecs  : Integer;
        currrecs  : Integer;
        chataccess: Integer;
        messaccess: Integer;
        feedaccess: Integer;
        newaccess : Integer;
        fileaccess: Integer;
        menudrv   : Integer;
        helpdrv   : Integer;
        messdrv   : Integer;
        namedrv   : Integer;
        keyfdrv   : Integer;
        userdrv   : Integer;
        filedrv   : Integer;
        bulldrv   : Integer;
        name      : String[31];
        phone     : String[12];
        initials  : String[3];
        pwd       : String[6];
        userkey   : String[9];  {Initials + password.}
        cr,lf,bs  : Char;
        q         : Char;
        Stringin  : String[80];
        scratch   : string[80];
        x,y,z     : Integer;

const   MaxDataRecSize = 65;
        MaxHeight = 3;
        MaxKeyLen = 32;
        PageSize = 24;
        Order = 12;
        PageStackSize = 3;

{$I ACCESS.BOX}
{$I GETKEY.BOX}
{$I ADDKEY.BOX}
{$I DELKEY.BOX}
Type    UserRec = record
                Uaccess    : Integer;       {+  2   2 }
                Upwd       : String[9];     {+ 10  12 }
                Uname      : String[31];    {+ 32  44 }
                Uphone     : String[12];    {+ 13  57 }
                Ulinefeeds : Boolean;       {+  1  58 }
                Uwidth     : Integer;       {+  2  60 }
                Unulls     : Integer;       {+  2  62 }
                Ulowcase   : Boolean;       {+  1  63 }
                Uevent     : Integer;       {+  2  65 }
                Ucalls     : Integer;
                Uposts     : Integer;
                Uploads    : Integer;
                Udloads    : Integer;
                Uexpert    : Boolean;       {+  1  74 }
        end;
        MsgRec  = Record
                MsgPtr    : Integer;
                Msgchars  : Array[1..126] of char;
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
                Roomname  : String[22];  {22 char room name (Duh!)}
                RoomNumber: Integer;     {2 digit room number}
                RoomAccess: Integer;     {Access required to enter room}
                RoomHidden: Boolean;     {Don't list in hall}
        end;
        HallRec = Record                 {35 bytes}
                HallKey   : String[1];   {only 1-9 halls}
                Hallname  : String[30];
                Hallaccess: Integer;
        end;
        SysVars = Integer;
        FileNamRec = Record
                FileKey    : String[13];  {The key (type filename)}
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

Var     Message : file;                  {Contains raw message data}
        Messhdr : DataFile;              {Points into Message}
        Roomnam : DataFile;              {Contains room data}
        Hallnam : DataFile;              {Contains hall data}
        Messptr : IndexFile;             {Points into Messhdr}
        Emssptr : IndexFile;             {Points into Messhdr}
        Omssptr : IndexFile;             {Points into Messhdr}
        Roomptr : IndexFile;             {Points into Roomnam}
        Hallptr : IndexFile;             {Points into Hallnam}
        SysFil  : File of SysVars;       {ABBSSYS.DAT defn}
        MsgVar  : MsgRec;                {MESSAGE.DAT recl}
        HdrVar  : HdrRec;                {MESSHDR.DAT recl}
        RoomVar : RoomRec;               {ROOMNAM.DAT recl}
        HallVar : HallRec;               {HALLNAM.DAT recl}
        Userlog : DataFile;
        Userpwd : IndexFile;
        Usernam : IndexFile;
        User    : UserRec;
        Userec  : Integer;          {Data record number of user}
        Halrec  : Integer;          {                      hall}
        Romrec  : Integer;          {                      room}
        Mesrec  : Integer;          {                      mess}
        Filenam : DataFile;              {Points to files}
        Fileptr : IndexFile;             {Points into Filenam}
        Typenam : DataFile;              {Names of file "rooms"}
        Typeptr : IndexFile;             {Points into Typenam}
        FileVar : FileNamRec;            {FILENAM.DAT recl}
        TypeVar : TypeNamRec;            {TYPENAM.DAT recl}
        CurrType: Integer;
        CurrFile: Integer;

begin
  writeln('Gyroscope data file initialization.');
  InitIndex;
  MakeFile(Messhdr,'MESSNAM.DAT',SizeOf(HdrVar));
  MakeIndex(Messptr,'MESSPTR.DAT',9,0);
  MakeIndex(Emssptr,'MESSEPTR.DAT',10,1);
  MakeIndex(Omssptr,'MESSOPTR.DAT',6,0);
  logons    := 0;
  event     := 0;
  messrecs  := 63;
  currrecs  := 0;
  chataccess:= 10;
  messaccess:= 10;
  feedaccess:= 10;
  newaccess := 10;
  fileaccess:= 10;
  menudrv   := 0;
  helpdrv   := 0;
  messdrv   := 0;
  namedrv   := 0;
  keyfdrv   := 0;
  userdrv   := 0;
  filedrv   := 0;
  bulldrv   := 0;
  Assign(SysFil,'ABBSSYS.DAT');
  Rewrite(SysFil);
  Write(SysFil,logons,event,messrecs,currrecs,chataccess,messaccess);
  Write(SysFil,feedaccess,newaccess,fileaccess,menudrv,helpdrv,messdrv);
  Write(SysFil,namedrv,keyfdrv,userdrv,filedrv,bulldrv);
  Close(SysFil);
  Assign(Message,'MESSAGE.DAT');
  Rewrite(Message);

  MakeFile(Hallnam,'HALLNAM.DAT',SizeOf(HallVar));
  MakeIndex(Hallptr,'HALLPTR.DAT',2,0);
  with HallVar do
        begin
          HallKey    := '1';
          Hallname   := 'Open';
          Hallaccess := 10;
          stringin   := HallKey;
          writeln('Initializing hall ' + HallKey + ': ' + Hallname);
        end;
  AddRec(Hallnam,HalRec,HallVar);
  AddKey(Hallptr,HalRec,stringin);
  CloseFile(Hallnam);
  CloseIndex(Hallptr);
  MakeFile(Roomnam,'ROOMNAM.DAT',SizeOf(RoomVar));
  MakeIndex(Roomptr,'ROOMPTR.DAT',4,0);
  with RoomVar do
        begin
          RoomKey    := '101';
          Roomname   := 'Lobby';
          RoomNumber := 1;
          RoomAccess := 10;
          RoomHidden := false;
          stringin   := RoomKey;
          writeln('Initializing room ' + RoomKey + ': ' + Roomname);
        end;
  AddRec(Roomnam,RomRec,RoomVar);
  AddKey(Roomptr,RomRec,stringin);
  CloseFile(Roomnam);
  CloseIndex(Roomptr);

{ Initialize the message data file with a zero nextempty}
  MsgVar.msgptr := 0;
  Seek(Message,0);
  for x := 1 to 126 do MsgVar.MsgChars[x] := ' ';
  BlockWrite(Message,MsgVar,1);
  Close(Message);
  CloseFile(Messhdr);
  CloseIndex(Messptr);
  CloseIndex(Emssptr);
  CloseIndex(Omssptr);
{ Initialize the userlog.}
  writeln('Creating USERLOG system files.');
  MakeFile(Userlog,'USERLOG.DAT',Sizeof(User));
  MakeIndex(Userpwd,'USERPWD.DAT',10,0);
  MakeIndex(Usernam,'USERNAM.DAT',32,0);
  writeln(' ');
  write(' Enter sysop name >');
  readln(stringin);
  name := stringin;
  write('Initials  : ');
  readln(stringin);
  initials := '';
  for x := 1 to length(stringin) do
    initials := initials + Upcase(copy(stringin,x,1));
  write('Password  : ');
  readln(pwd);
  write('Access    : 99');
  access := 99;
  write('Phone     : ');
  readln(phone);
  write('Linefeeds ? ');
  readln(q);
  linefeeds := true;
  if (q = 'N') or (q = 'n') then linefeeds := false;
  write('Width     : ');
  readln(width);
  write('Lowercase ? ');
  readln(q);
  write('Nulls     : ');
  readln(nulls);
  lowcase := true;
  if (q = 'N') or (q = 'n') then lowcase := false;
  event := 0;
  calls := 0;
  posts := 0;
  ploads := 0;
  dloads := 0;
  expert := false;
  userkey := initials + pwd;
  writeln(' ');
  writeln('Access    : ',access);
  writeln('Pwdkey    : ',userkey);
  writeln('Name      : ',name);
  writeln('Phone     : ',phone);
  writeln('Linefeeds : ',linefeeds);
  writeln('Width     : ',width);
  writeln('Nulls     : ',nulls);
  writeln('Lowercase : ',lowcase);
  writeln('Calls     : ',calls);
  writeln('Posts     : ',posts);
  writeln('Uploads   : ',ploads);
  writeln('Downloads : ',dloads);
  writeln('Expert    : ',expert);
  writeln('Event     : ',event);
  writeln(' ');
  writeln('Writing userlog entry [',name,'] [',userkey,']');
  User.Uaccess    := access;
  User.Upwd       := userkey;
  User.Uname      := name;
  User.Uphone     := phone;
  User.Ulinefeeds := linefeeds;
  User.Uwidth     := width;
  User.Unulls     := nulls;
  User.Ulowcase   := lowcase;
  User.Uevent     := event;
  User.Ucalls     := calls;
  User.Uposts     := posts;
  User.Uploads    := ploads;
  User.Udloads    := dloads;
  User.Uexpert    := expert;
  AddRec(Userlog,Userec,User);
  AddKey(Userpwd,Userec,userkey);
  if not OK then writeln('* error writing password.');
  stringin := '';
  for x := 1 to length(name) do
    stringin := stringin + Upcase(copy(name,x,1));
  AddKey(Usernam,Userec,stringin);
  if not OK then writeln('* error writing name.');
  CloseFile(Userlog);
  CloseIndex(Userpwd);
  CloseIndex(Usernam);
{ Initialize FILES stuff}
  writeln('Creating FILE & TYPE files...');
  MakeFile(Filenam,'FILENAM.DAT',SizeOf(FileVar));
  MakeFile(Typenam,'TYPENAM.DAT',SizeOf(TypeVar));
  MakeIndex(Fileptr,'FILEPTR.DAT',14,0);
  MakeIndex(Typeptr,'TYPEPTR.DAT',3,0);
  scratch := '01';
  with TypeVar do begin
    TypeKey    := scratch;
    TypeName   := 'General Text Files';
    TypeAccess := 0;
  end;
  AddRec(Typenam,CurrType,TypeVar);
  if not OK then writeln('Error creating first record!') else begin
   AddKey(Typeptr,CurrType,TypeVar.TypeKey);
   if not OK then writeln('Error writing first key!');
  end;
  CloseFile(Filenam);
  CloseFile(Typenam);
  CloseIndex(Fileptr);
  CloseIndex(Typeptr);
  writeln('End of job.');
end.
