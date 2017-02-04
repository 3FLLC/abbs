Overlay Procedure UserParms;
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
        HdrVar  : HdrRec;                {MESSNAM.DAT recl}
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

Procedure Convertmail;
begin
  x := 0;
  repeat
    begin
      scratch := userkey;
      FindKey(Emssptr,CurrMess,scratch);
      if OK then
        begin
          GetRec(Messhdr,CurrMess,HdrVar);
          x := x + 1;
          HdrVar.HdrMsgTo := initials + pwd;
          PutRec(Messhdr,CurrMess,HdrVar);
          DeleteKey(Emssptr,CurrMess,scratch);
          if not OK then lineout(' Mail delivery source error!',ys);
          scratch := initials + pwd;
          AddKey(Emssptr,CurrMess,scratch);
          if not OK then lineout(' Mail delivery destination error!',ys);
        end;
    end
  until not OK;
  str(x,stringout);
  stringout := ' Rerouted ' + stringout + ' mail message(s) to new password.';
  if x > 0 then lineout(stringout,ys);
end;

begin
  InitIndex;
  OpenFile(Hallnam,AddDrive(keyfdrv,'HALLNAM.DAT'),SizeOf(HallVar));
  OpenFile(Roomnam,AddDrive(keyfdrv,'ROOMNAM.DAT'),SizeOf(RoomVar));
  OpenFile(Messhdr,AddDrive(namedrv,'MESSNAM.DAT'),SizeOf(HdrVar));
  OpenIndex(Hallptr,AddDrive(keyfdrv,'HALLPTR.DAT'),2,0);
  OpenIndex(Roomptr,AddDrive(keyfdrv,'ROOMPTR.DAT'),4,0);
  OpenIndex(Messptr,AddDrive(keyfdrv,'MESSPTR.DAT'),9,0);
  OpenIndex(Emssptr,AddDrive(keyfdrv,'MESSEPTR.DAT'),10,1);
  OpenIndex(Omssptr,AddDrive(keyfdrv,'MESSOPTR.DAT'),6,0);
  Assign(Message,AddDrive(messdrv,'MESSAGE.DAT'));
  Reset(Message);
  repeat
    begin
      if not expert then Fileout(AddDrive(menudrv,'MENUUTIL'))
      else lineout(' UTIL: B L N P S U W I X ? Q ',no);
      lineout('>',no);
      Inputchar(ys);
      q := Upcase(q);
      lineout(' ',ys);
      if not Resetnow then case q of
        'I' : begin
              str(access,scratch);
              q := copy(scratch,length(scratch),1);
              if ((q <> '0') and (access > 10) and (access < 90)) or
               (access > 98) then
              begin
               lineout(' ',ys);
               lineout(' Do not take this command lightly. It will raise the',ys);
               lineout(' access level of the user you name to equal your own.',ys);
               lineout(' ',ys);
               lineout(' Name of user to invite (return to abort) >',no);
               linein(31,ys,ys);
               while (length(stringin) > 0) and not done do
                if copy(stringin,1,1) = ' ' then delete(stringin,1,1)
                else done := ys;
               done := no;
               while (length(stringin) > 0) and not done do
                if copy(stringin,length(stringin),1) = ' ' then
                 delete(stringin,length(stringin),1) else done := ys;
               done := no; scratch := '';
               if length(stringin) > 0 then
                for x := 1 to length(stringin) do
                 scratch := scratch + Upcase(copy(stringin,x,1));
               stringin := scratch;
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
                      stringout := ' Invite ' + User.Uname + ' ?';
                      lineout(stringout,no);
                      Inputchar(ys); q := Upcase(q);
                      lineout(' ',ys);
                    end;
                  if (stringin = scratch) or (q = 'Y') then begin
                    if access > User.Uaccess then begin
                      if access > 98 then begin
                        str(User.Uaccess,stringout);
                        stringout := ' Current access = ' + stringout;
                        lineout(stringout,ys);
                        lineout(' Enter replacement access >',no);
                        x := Intin;
                        if x > 0 then User.Uaccess := x;
                        str(x,stringout);
                        stringout := ' New access = ' + stringout;
                        lineout(stringout,ys);
                      end else User.Uaccess := access;
                      PutRec(Userlog,Userec,User);
                    end;
                    lineout(' Done!',ys);
                    q := 'Y';
                  end else OK := no;
                  if q = '~' then lineout(' Unknown user.',ys);
                 end else lineout(' Unknown user.',ys);
                 CloseFile(Userlog);
                 CloseIndex(Usernam);
                end;
              end else lineout(' Sysop must authorize.',ys);
              end;
        'P' : begin
                OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
                OpenIndex(Userpwd,AddDrive(userdrv,'USERPWD.DAT'),10,0);
                scratch := userkey;
                FindKey(Userpwd,Userec,userkey);
                if OK then begin
                  GetRec(Userlog,Userec,User);
                  lineout(' ',ys);
                  lineout(' Enter your new password >',no);
                  linein(6,no,ys);
                  pwd := LineUp(stringin);
                  scratch := initials + pwd;
                  FindKey(Userpwd,Userec,scratch);
                  if not OK and not Resetnow then begin
                      lineout(' Re-enter password >',no);
                      linein(6,no,ys);
                      stringin  := LineUp(stringin);
                      if not Resetnow then begin
                        if stringin = pwd then begin
                          Convertmail;
                          scratch         := initials + pwd;
                          User.Uaccess    := access;
                          User.Upwd       := scratch;
                          User.Uname      := name;
                          User.Uphone     := phone;
                          User.Ulinefeeds := linefeeds;
                          User.Uwidth     := width;
                          User.Unulls     := nulls;
                          User.Ulowcase   := lowcase;
                          User.Uevent     := lastevent;
                          User.Ucalls     := calls;
                          User.Uposts     := posts;
                          User.Uploads    := ploads;
                          User.Udloads    := dloads;
                          User.Uexpert    := expert;
                          PutRec(Userlog,Userec,User);
                          DeleteKey(Userpwd,Userec,userkey);
                          userkey         := scratch;
                          AddKey(Userpwd,Userec,scratch);
                          if OK then lineout(' Done!',ys)
                          else lineout(' Error in update.',ys);
                        end
                        else lineout(' Did not match - try again.',ys);
                      end;
                  end
                  else lineout(' Password is taken.  Try another.',ys);
                end
                else lineout(' Error in user password file.',ys);
                CloseFile(Userlog);
                CloseIndex(Userpwd);
              end;
        'L' : begin
                if linefeeds then
                  begin
                    linefeeds := no;
                    lineout(' Linefeeds now off.',ys);
                  end
                else
                  begin
                    linefeeds := ys;
                    lineout(lf + ' Linefeeds now on.',ys);
                  end;
              end;
        'N' : begin
                str(nulls,stringout);
                stringout := ' Nulls = ' + stringout;
                lineout(stringout,ys);
                lineout(' Enter nulls needed (0..9) >',no);
                x := Intin;
                if (x >= 0) and (x < 10) then nulls := x
                else lineout(' Nulls not changed.',ys);
              end;
        'W' : begin
                str(width,stringout);
                stringout := ' Width = ' + stringout;
                lineout(stringout,ys);
                lineout(' Enter your screen width (20..132) >',no);
                x := Intin;
                if (x > 19) and (x < 133) then width := x
                else lineout(' Width not changed.',ys);
              end;
        'S' : begin
                lineout(' ',ys);
                stringout := ' Status of ' + name + ':';
                lineout(stringout,ys);
                stringout := ' Linefeeds off.';
                if linefeeds then stringout := ' Linefeeds on.';
                lineout(stringout,ys);
                str(width,stringout);
                stringout := ' Width = ' + stringout;
                lineout(stringout,ys);
                str(nulls,stringout);
                stringout := ' Nulls = ' + stringout;
                lineout(stringout,ys);
                stringout := ' Expert mode off.';
                if expert then stringout := ' Expert mode on.';
                lineout(stringout,ys);
                str(calls,stringout);
                stringout := ' Calls = ' + stringout;
                lineout(stringout,ys);
                str(posts,stringout);
                stringout := ' Posts = ' + stringout;
                lineout(stringout,ys);
              end;
        'B' : begin
               lineout(' ',ys);
               ClearKey(Hallptr);
               repeat
                begin
                 NextKey(Hallptr,CurrHall,scratch);
                 if OK then
                  begin
                   GetRec(Hallnam,CurrHall,HallVar);
                   if access >= HallVar.Hallaccess then
                    begin
                     with HallVar do
                       stringout := ' '+HallKey+'  '+Hallname + ' hall...';
                     if access > 90 then
                       begin
                         while length(stringout) < 40 do
                           stringout := stringout + ' ';
                         str(HallVar.Hallaccess,stringin);
                         stringout := stringout + ' = ' + stringin;
                       end;
                     lineout(stringout,ys);
                     scratch2 := scratch + '00';
                     SearchKey(Roomptr,CurrRoom,scratch2);
                     PrevKey(Roomptr,CurrRoom,scratch2);
                     NextKey(Roomptr,CurrRoom,scratch2);
                     while OK and (Copy(scratch2,1,1) = scratch) and (q <> 'S') do
                       begin
                         GetRec(Roomnam,CurrRoom,RoomVar);
                         if (access >= RoomVar.Roomaccess) and
                            (not RoomVar.RoomHidden or (access > 90))  then
                           begin
                             with RoomVar do
                               stringout := '    '+RoomKey+'  '+Roomname;
                             if access > 90 then
                               begin
                                 while length(stringout) < 40 do
                                   stringout := stringout + ' ';
                                 str(RoomVar.Roomaccess,stringin);
                                 stringout := stringout + ' = ' + stringin;
                               end;
                             lineout(stringout,ys);
                           end;
                         NextKey(Roomptr,CurrRoom,scratch2);
                       end;
                    end;
                   OK := true;
                  end;
                end
               until not OK or (q = 'S');
              end;
        'U' : begin
                OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
                OpenIndex(Usernam,AddDrive(userdrv,'USERNAM.DAT'),32,0);
                if OK then
                 begin
                  lineout(' ',ys);
                  lineout(' S)top or P)ause output...',ys);
                  lineout(' ',ys);
                  lineout(' Calls  Posts  Last#  Username',ys);
                  ClearKey(Usernam);
                  repeat
                   begin
                    NextKey(Usernam,Userec,stringin);
                    if OK then
                     begin
                      GetRec(Userlog,Userec,User);
                      str(User.Ucalls,stringin);
                      while Length(stringin) < 5 do stringin := ' '+stringin;
                      stringout := stringin;
                      str(User.Uposts,stringin);
                      while Length(stringin) < 7 do stringin := ' '+stringin;
                      stringout := stringout + stringin;
                      str(User.Uevent,stringin);
                      while Length(stringin) < 7 do stringin := ' '+stringin;
                      stringout := stringout + stringin + '   ' + User.Uname;
                      if access > 98 then
                        begin
                          str(User.Uaccess,stringin);
                          stringout := stringout + ' = ' + stringin;
                        end;
                      lineout(stringout,ys);
                     end;
                   end
                  until not OK or (q = 'S') or Resetnow;
                 end
                else lineout('error on Userlog open!',ys);
                CloseFile(Userlog);
                CloseIndex(Usernam);
              end;
        '?' : Fileout(AddDrive(helpdrv,'HELPUSER'));
        'X' : if expert then expert := no else expert := ys;
        'Q' : done := true;
      end;
    end;
  until done or Resetnow;
  done := false;
  CloseFile(Hallnam);
  CloseFile(Roomnam);
  CloseFile(Messhdr);
  CloseIndex(Hallptr);
  CloseIndex(Roomptr);
  CloseIndex(Messptr);
  CloseIndex(Emssptr);
  CloseIndex(Omssptr);
  Close(Message);
end;
