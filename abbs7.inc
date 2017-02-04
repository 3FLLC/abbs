{This procedure counts the number of users currently in the userlog.  It's
a funny way of 'saving' the variable currusers, but it saves the USERlog
data files if ABBSSYS.DAT ever gets trashed.  That's why I do it this way.}
Overlay Procedure CalcUsers;
Var     Userpwd : IndexFile;
        Userec  : Integer;          {Data record number of user}
begin
  OpenIndex(Userpwd,AddDrive(userdrv,'USERNAM.DAT'),32,0);
  if OK then begin
    write(' Tallying users...');
    currusers := 0;
    ClearKey(Userpwd);
    repeat begin
      NextKey(Userpwd,Userec,stringin);
      if OK then currusers := currusers + 1;
    end until not OK;
    writeln(currusers);
  end
  else writeln(' Error occured opening file!');
  CloseIndex(Userpwd);
end;

{This procedure gets initials and password and searches the USERLOG for such a
person.  If not found, will get name and other data and log them in.  If found,
it sets 'access' and passes it back to main program.  In fact, it passes the
whole mess of user-oriented variable information back, for use by the msg sys}
Overlay Procedure Logon;
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
                Ucalls     : Integer;       {+  2  67 }
                Uposts     : Integer;       {+  2  69 }
                Uploads    : Integer;       {+  2  71 }
                Udloads    : Integer;       {+  2  73 }
                Uexpert    : Boolean;       {+  1  74 }
        end;
Var     Userlog : DataFile;
        Userpwd : IndexFile;
        Usernam : IndexFile;
        Emssptr : IndexFile;
        User    : UserRec;
        Userec  : Integer;          {Data record number of user}

begin
  lineout(' ',ys);
  lineout(' Your initials >',no);
  linein(3,ys,ys);
  if stringin = '' then stringin := '?';
  initials := LineUp(stringin);
  lineout(' Password >',no);
  pwd := '';
  if not Resetnow then
    begin
      linein(6,no,ys);
      pwd := LineUp(stringin);
    end;
  userkey := initials + pwd;
{  InitIndex; }
  OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
  OpenIndex(Userpwd,AddDrive(userdrv,'USERPWD.DAT'),10,0);
  OpenIndex(Usernam,AddDrive(userdrv,'USERNAM.DAT'),32,0);
  Findkey(Userpwd,Userec,userkey);
  if not OK and not Resetnow then
    begin
      lineout(' Re-enter password >',no);
      linein(6,no,ys);
      stringin := LineUp(stringin);
      userkey := initials + stringin;
      Findkey(Userpwd,Userec,userkey);
      if not OK then
       begin
        if (pwd <> stringin) or (initials = '?') then
         begin
          lineout(' Passwords did not match - try again.',ys);
          OK := false;
         end
        else
         begin
          lineout(' I do not recognize you.  Would you like an account ? ',no);
          inputchar(ys);
          q := Upcase(q);
          if (q = 'Y') and not Resetnow then
            begin
              repeat
                begin
                 lineout(' ',ys);
                 lineout(' Registration:  JUST HIT RETURN if you',ys);
                 lineout(' are not sure what the question means.',ys);

{ The Author of this BBS program is called Gyro Gearloose.  My family calls  }
{ me that, and it's more descriptive of me than Aaron Logue.  If you find    }
{ pseudonyms unacceptable, please don't run this software.  There's nothing  }
{ stuffier and more alien to a new modem user than a cold, closed BBS.       }
{ The next line may be removed to clean up the messy registration stuff, but }
{ I am serious about not running this if nyms bother you. The entire premise }
{ of this software is to present an open, friendly atmosphere to people.     }

                 lineout(' Pseudonyms are acceptable.',ys);
                  repeat begin
                    lineout(' ',ys);
                    lineout(' Your name >',no);
                    linein(31,ys,ys);
                    if stringin = '' then stringin := '~';
                    while copy(stringin,1,1) = ' ' do delete(stringin,1,1);
                    while copy(stringin,length(stringin),1) = ' ' do
                      delete(stringin,length(stringin),1);
                    name := stringin;
                    stringin := LineUp(name);
                    Findkey(Usernam,Userec,stringin);
                    if OK then lineout(' Already taken - try another.',ys);
                  end until not OK or Resetnow;
                  if not Resetnow and (name <> '~') then begin
                    lineout(' -----',ys);
                    lineout('  ###-###-####  Your phone number',ys);
                    lineout(' >',no);
                    linein(12,ys,ys);
                    phone := stringin;
                  end;
                  if not Resetnow and (name <> '~') then begin
                    lineout(' -----',ys);
                    lineout(' Answer N to the next question only if',ys);
                    lineout(' everything has been double-spaced.',ys);
                    lineout(' Do you want linefeeds [Y] >',no);
                    inputchar(ys);
                    q := upcase(q);
                    if q = 'N' then linefeeds := no;
                    lineout(' ',ys);
                  end;
                  if not Resetnow and (name <> '~') then begin
                    lineout(' -----',ys);
                    lineout(' Your screen width [80] >',no);
                    width := Intin;
                    if (width < 20) or (width > 132) then width := 80;
                  end;
                  if not Resetnow and (name <> '~') then begin
                    lineout(' -----',ys);
                    lineout(' If you do not see 0123456789 on the',ys);
                    lineout(' next line, then you need as many nulls',ys);
                    lineout('0123456789 as there are digits missing.',ys);
                    lineout(' Nulls, if any [0] >',no);
                    nulls := Intin;
                    if (nulls < 0) or (nulls > 9) then nulls := 0;
                  end;
                  if not Resetnow and (name <> '~') then begin
                    lineout(' ',ys);
                    lineout(' Name  : ' + name,ys);
                    lineout(' Phone : ' + phone,ys);
                    if linefeeds then stringout:='on.' else stringout:='off.';
                    lineout(' Linefeeds ' + stringout,ys);
                    str(width,stringout);
                    stringout := ' Width : ' + stringout;
                    lineout(stringout,ys);
                    str(nulls,stringout);
                    stringout := ' Nulls : ' + stringout;
                    lineout(stringout,ys);
                    lineout(' Is the above data correct ? ',no);
                    inputchar(ys);
                    q := Upcase(q);
                    lineout(' ',ys);
                  end;
                  if (q = 'Y') and not Resetnow and (name <> '~') then
                    begin
                      lineout(' Adding ' + name + ' to userlog...',ys);
                      access          := newaccess;
                      User.Uaccess    := access;
                      User.Upwd       := initials + pwd;
                      User.Uname      := name;
                      User.Uphone     := phone;
                      User.Ulinefeeds := linefeeds;
                      User.Uwidth     := width;
                      User.Unulls     := nulls;
                      User.Ulowcase   := true;
                      User.Uevent     := event + 1;
                      User.Ucalls     := 0;
                      User.Uposts     := 0;
                      User.Uploads    := 0;
                      User.Udloads    := 0;
                      User.Uexpert    := expert;
                      AddRec(Userlog,Userec,User);
                      AddKey(Userpwd,Userec,userkey);
                      if not OK then lineout(' Userpwd error!',ys);
                      stringin := LineUp(name);
                      AddKey(Usernam,Userec,stringin);
                      if not OK then lineout(' Userlog add error NOK',ys);
                      Fileout(AddDrive(bulldrv,'WELCOME'));
                      done := true; OK := ys;
                    end
                  else
                    begin
                      done := true; {Or, we could ask if try again...}
                      OK := false;
                    end;
                end
              until done or Resetnow;
              done := false;
            end;
         end;
       end;
    end;
  if OK and not Resetnow then begin
    GetRec(Userlog,Userec,User);
    access    := User.Uaccess;
    userkey   := User.Upwd;
    name      := User.Uname;
    phone     := User.Uphone;
    linefeeds := User.Ulinefeeds;
    width     := User.Uwidth;
    nulls     := User.Unulls;
    lowcase   := User.Ulowcase;
    lastevent := User.Uevent;
    logons    := logons + 1;
    calls     := User.Ucalls + 1;
    posts     := User.Uposts;
    ploads    := User.Uploads;
    dloads    := User.Udloads;
    expert    := User.Uexpert;
    stringout := Time;
    callminute:= minute;
    lineout(' ' + name + ' logged in ' + stringout,ys);
    lastcalr := name;
    str((event-lastevent+1),stringout);
    stringout := ' ' + stringout + ' messages since last logoff.';
    lineout(stringout,ys);
    OpenIndex(Emssptr,AddDrive(keyfdrv,'MESSEPTR.DAT'),10,1);
    stringout := userkey;
    if OK then FindKey(Emssptr,x,stringout);
    if OK then lineout(' You have mail.',ys);
    CloseIndex(Emssptr);
  end else lineout(' NOT logged in.',ys);
  CloseFile(Userlog);
  CloseIndex(Userpwd);
  CloseIndex(Usernam);
end;

{This procedure T)erminates a user, resaving his/her parameters and updating
the old message numbers.}
Overlay Procedure Logoff;
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
                Ucalls     : Integer;       {+  2  67 }
                Uposts     : Integer;       {+  2  69 }
                Uploads    : Integer;       {+  2  71 }
                Udloads    : Integer;       {+  2  73 }
                Uexpert    : Boolean;       {+  1  74 }
        end;
Var     Userlog : DataFile;
        Userpwd : IndexFile;
        User    : UserRec;
        Userec  : Integer;          {Data record number of user}
begin
  OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
  OpenIndex(Userpwd,AddDrive(userdrv,'USERPWD.DAT'),10,0);
  Findkey(Userpwd,Userec,userkey);
  if OK then begin
    GetRec(Userlog,Userec,User);
    User.Uaccess    := access;
    User.Upwd       := userkey;
    User.Uname      := name;
    User.Uphone     := phone;
    User.Ulinefeeds := linefeeds;
    User.Uwidth     := width;
    User.Unulls     := nulls;
    User.Ulowcase   := lowcase;
    User.Uevent     := event + 1; {+1 so that their last message not incl.}
    User.Ucalls     := calls;
    User.Uposts     := posts;
    User.Uploads    := ploads;
    User.Udloads    := dloads;
    User.Uexpert    := expert;
    PutRec(Userlog,Userec,User);
    name            := 'somebody';
  end else writeln(' Error on Logoff!');
  CloseFile(Userlog);
  CloseIndex(Userpwd);
end;
