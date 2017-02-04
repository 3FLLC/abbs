{Before this program can be recompiled and take over an existing message
base, you must verify that
 1) the ABBSSYS.DAT layout in Procedure UpAbbssys; matches existing ABBSSYS,
 2) any values not yet implemented in ABBSSYS are initialized to their
    proper values}

Program ABBS;
 {Ver 0.01 6/5   Added xmodem checksum/CRC-CCITT downloads}
 {Ver 0.01 5/28  Timestamps, access level expansion}
 {Ver 0.00       started keeping track of mods}

const   MaxDataRecSize = 65;
        MaxHeight = 3;
        MaxKeyLen = 32;
        PageSize = 24;
        Order = 12;
        PageStackSize = 3;
        MsgBuffSize = 1600;  {This needs to be implemented in the code!}

{$I ACCESS.BOX}
{$I ADDKEY.BOX}
{$I DELKEY.BOX}
{$I GETKEY.BOX}

type    String255 = string[255];
        String80  = string[80];
        String16  = string[16]; {Used for timestamping}
        String14  = string[14]; {Used for filenames}
        SysVars   = Integer;    {Used for ABBSSYS.DAT}

var     FilVar    : text;       {Text file names.}
        SysFil    : file of SysVars;
        ys,no     : Boolean;    {Real important!}
        done      : Boolean;    {Used to terminate the program (et al)}
        Local     : Boolean;    {So sysop can fake carrier and logon}
        Sysop     : Boolean;    {Set to true if last inputchar was by sysop}
        linefeeds : Boolean;    {User linefeeds?}
        lowcase   : Boolean;    {Display lowercase?}
        expert    : Boolean;    {Disable menues?}
        calls     : Integer;    {Number of calls for a user}
        posts     : Integer;    {Messages posted by user}
        ploads    : Integer;    {Files uploaded by user}
        dloads    : Integer;    {Files downloaded by user}
        width     : Integer;    {User's screen width}
        nulls     : Integer;    {How many nulls needed after cr}
        lastevent : Integer;    {Last time current user called}
        access    : Integer;    {User's access level}
        event     : Integer;    {This event number}
        newaccess : Integer;    {Access to assign a new user}
        chataccess: Integer;    {Minimum access for Chat}
        feedaccess: Integer;    {Minimum access for Feedback}
        messaccess: Integer;    {Minimum access for Messages}
        fileaccess: Integer;    {Minimum access for Files}
        mailaccess: Integer;    {Minimum access to send mail}
        messrecs  : Integer;    {Maximum MESSAGE.DAT records allowed}
        currrecs  : Integer;    {Current MESSAGE.DAT active records}
        maxxusers : Integer;    {Maximum users before system scrolls them}
        currusers : Integer;    {Current number of users}
        logons    : Integer;    {Total number of system logons}
        menudrv   : Integer;    {Drive containing MENU text files}
        helpdrv   : Integer;    {Drive containing HELP text files}
        messdrv   : Integer;    {Drive containing MESSAGE.DAT}
        namedrv   : Integer;    {Drive containing MESSNAM.DAT}
        keyfdrv   : Integer;    {Drive containing ROOM, HALL, & other MESS}
        userdrv   : Integer;    {Drive containing USER files}
        filedrv   : Integer;    {Drive containing FILE hall, type, & name}
        bulldrv   : Integer;    {Drive containing bulletin text files}
        gamedrv   : Integer;    {Drive containing game overlay files}
        datadrv   : Integer;    {Drive containing game data files}
        baud      : Integer;    {Caller's baud rate}
        callminute: Integer;    {minute that a caller connected}
        oldminute : Integer;    {last known minute of the day}
        minute    : Integer;    {0-1439 - minute of the day}
        day       : Integer;    {1-7    - day of the week}
        date      : Integer;    {1-31   - day of the month}
        month     : Integer;    {1-12   - month of the year}
        initials  : String[3];
        pwd       : String[6];
        userkey   : String[9];  {Initials + password.}
        name      : String[31];
        phone     : String[12];
        lastcalr  : String[31];
        SysopName : String[31]; {Who receives Feedback??}
        cr,lf,bs  : Char;
        q         : Char;
        p         : Integer;    {Length of a message}
        Stringout : String80;
        Stringin  : String255;
        x,y,z     : Integer;
        Filename  : String14;
        Tempstr   : String14;
        Resetnow  : Boolean;
        Tout      : Real;       {Turbo's pretty fast, so this'll get big}
        Timeout   : Real;       {A big number that determines timeout time}
        Intimout  : Real;       {The timeout value for file xfer (~10 seconds)}
        MsgBuff   : Array[1..MsgBuffSize] of char;  {The message buffer}

Procedure UpAbbssys;
begin
  Assign(SysFil,'ABBSSYS.DAT');
  Rewrite(SysFil);
  Write(SysFil,logons,event,messrecs,currrecs,chataccess,messaccess);
  Write(SysFil,feedaccess,newaccess,fileaccess,menudrv,helpdrv,messdrv);
  Write(SysFil,namedrv,keyfdrv,userdrv,filedrv,bulldrv);
  Close(SysFil);
end;

{$I ABBS1.INC} { MACHINE DEPENDENT - Charin, Charout, Status testing.}
{$I ABBS2.INC} { Procedures        - Character, Line, Integer I/O}
{$I ABBS3.INC} { Procedure Fileout - Outputs help or information text files }
{$I ABBS4.INC} { Procedures Outbuff and Editbuff - User/Msgbuffer interface }
{$I ABBS5.INC} { MACHINE DEPENDENT - Hangup & Reset modem (Overlay) }
{$I ABBS6.INC} { Overlay Procedure - Chat, Feedback, and Bulletins }
{$I ABBS7.INC} { Overlay Procedure - Logon & Logoff }
{$I ABBS8.INC} { Overlay Procedure - 1st part of message subsystem }
{$I ABBS9.INC} {                     2nd part of message subsystem }
{$I ABBSA.INC} {                     3rd part of message subsystem }
{$I ABBSB.INC} { Overlay Procedure - User utilities }
{$I ABBSC.INC} { Overlay Procedure - 1st part of sysop utilities }
{$I ABBSD.INC} {                     2nd part of sysop utilities }
{$I ABBSE.INC} { Overlay Procedure - 1st part of file transfer subsystem }
{$I ABBSF.INC} {                     2nd part of file transfer subsystem }
{$I ABBSG.INC} { Overlay Procedure - Games section }

Begin
  InitIndex;
  ys := true;
  no := false;
  cr := #13;
  lf := #10;
  bs := #08;
  CalcUsers;         {Gives currusers}
  oldminute  := 0;   {keep oldminute here though}
  mailaccess := 10;  {ADD THESE GUYS TO ABBSSYS, along with day, date, month}
  maxxusers  := 500; {sure... right... add this to V)ariables too}
  gamedrv    := 1;   {1 = drive A}
  datadrv    := 2;   {2 = drive B}
  Assign(SysFil,'ABBSSYS.DAT');  {Note - ABBSSYS MUST be on default drive}
  Reset(SysFil);
  Read(SysFil,logons,event,messrecs,currrecs,chataccess,messaccess);
  Read(SysFil,feedaccess,newaccess,fileaccess,menudrv,helpdrv,messdrv);
  Read(SysFil,namedrv,keyfdrv,userdrv,filedrv,bulldrv);
  Close(SysFil);
  lastcalr := 'System Boot';
  SysopName := 'Gyro Gearloose';
  write(' 1=Sun, 2=Mon, etc.. what day is it? ');
  readln(day);
  stringout := ' Booted ' + Time;
  writeln(stringout);
  repeat begin
    Timeout := 175000.;    {A number you'll just have to experiment with.}
    Intimout:= 15000.;     {Another one; xmodem timeout value.  s/b ~10 sec}
    Local := false;        {Set when sysop hits a key to sign on locally.}
    Resetnow := false;     {Set when a timeout has occurred.}
    done := false;         {Used as a flag by various loops.}
    writeln('Log:',logons,' Posts:',event,' Max:',messrecs,' Curr:',currrecs);
    Initmdmandwait;
    {Put default stuff to be reset for each caller here.}
    access    := 0;
    width     := 79;
    nulls     := 0;
    linefeeds := true;
    lowcase   := ys;
    expert    := no;
    Fileout(AddDrive(bulldrv,'SIGNON'));
    stringout := ' Last caller: ' + lastcalr + '.';
    lineout(Stringout,ys);
    Str(event:5,stringout);
    stringout := ' Last message #' + stringout;
    lineout(stringout,ys);
    lineout(' ',ys);
    repeat
      begin
        if access = 0 then lineout(' MENU:  B)ulletins  L)ogon ',no)
        else if not expert then Fileout(AddDrive(menudrv,'MENUMAIN'))
        else lineout(' MAIN: B C D F G L M U X ? T ',no);
        lineout('>',no);
        inputchar(ys);
        q := Upcase(q);
        lineout(' ',ys);
        if not Resetnow then
        case q of
          'B' : Bulletins;
          'C' : if (access >= chataccess) or sysop then Chat else
                if access = 0 then lineout(' Logon first.',ys) else
                lineout(' Out to lunch.  Try again later.',ys);
          'D' : if access >= fileaccess then Files else
                if access = 0 then lineout(' Logon first.',ys) else
                lineout(' Not authorized - leave F)eedback to sysop.',ys);
          'F' : if access >= feedaccess then Feedback else
                if access = 0 then lineout(' Logon first.',ys) else
                lineout(' Not authorized - try C)hat.',ys);
          'G' : if access > 0 then Games else lineout(' Logon first.',ys);
          'L' : if access = 0 then Logon else lineout(' Logged on.',ys);
          'M' : if access >= messaccess then Message else
                if access = 0 then lineout(' Logon first.',ys) else
                lineout(' Not authorized - leave F)eedback to sysop.',ys);
          'S' : if access > 90 then Utility;
          'U' : if access > 0 then UserParms else lineout(' Logon first.',ys);
          'X' : if access > 0 then
                  if expert then expert := no else expert := ys;
          'Y' : if local then Resetnow := ys;
          'Z' : if local then begin q := 'T'; done := ys; end;
          '?' : Fileout(AddDrive(helpdrv,'HELPMAIN'));
        end;
        if (q = 'T') and not Resetnow then begin
          if not sysop then begin
            lineout(' Confirm logoff [Y] >',no);
            inputchar(ys);
            lineout(' ',ys);
            if q in ['Y','y',#13] then begin
              q := 'T';
              stringout := ' Logging ' + name + ' off.';
              lineout(stringout,ys);
            end else q := ' ';
          end else lineout(' Throwing you out.',ys);
        end;
      end
    until Resetnow or (q = 'T');
    if not local then Hangup;
    if (q = 'T') and (access > 0) and not Resetnow then Logoff;
    UpAbbssys;
    name := 'Somebody';
  end until done;
  writeln(' End of Job');
end.
