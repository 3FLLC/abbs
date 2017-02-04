begin
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
      lineout(' ',ys);
      stringout := ' SYS:  ';
      if access > 94 then stringout := stringout + 'R)oom  ';
      if access > 95 then stringout := stringout + 'H)all  ';
      if access > 96 then stringout := stringout + 'M)essages  ';
      if access > 97 then stringout := stringout + 'U)ser  ';
      if access > 98 then stringout := stringout + 'V)ariables  ';
      stringout := stringout + 'Q)uit >';
      lineout(stringout,no);
      Inputchar(ys);
      q := Upcase(q);
      lineout(' ',ys);
      if not Resetnow then case q of
        'V' : if access > 98 then begin
                lineout(' ',ys);
                str(newaccess,stringout);
                stringout := ' New user access = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then newaccess := x;
                str(feedaccess,stringout);
                stringout := ' Feedback access = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then feedaccess := x;
                str(chataccess,stringout);
                stringout := ' Chat access     = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then chataccess := x;
                str(messaccess,stringout);
                stringout := ' Message access  = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then messaccess := x;
                str(mailaccess,stringout);
                stringout := ' Mail access     = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then mailaccess := x;
                str(fileaccess,stringout);
                stringout := ' Files access    = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if x in [0..99] then fileaccess := x;
                str(maxxusers,stringout);
                stringout := ' Maximum users   = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if (x > 0) and (x < 32001) then maxxusers := x;
                lineout(' ',ys);
                str(currrecs,stringout);
                stringout := ' Current records = ' + stringout;
                lineout(stringout,ys);
                str(FileSize(message),stringout);
                stringout := ' Current size    = ' + stringout;
                lineout(stringout,ys);
                str(messrecs,stringout);
                stringout := ' Maxrecs allowed = ' + stringout + '  > ';
                lineout(stringout,no);
                if not Resetnow then x := Intin;
                if (x > 30) and (x < 2096) then messrecs := x;
                str(trunc((messrecs/8)+1),stringout);
                stringout := ' Message file will grow to ' + stringout + 'K';
                lineout(stringout,ys);
              end;
        'U' : if access > 97 then begin
  OpenFile(Userlog,AddDrive(userdrv,'USERLOG.DAT'),SizeOf(User));
  OpenIndex(Usernam,AddDrive(userdrv,'USERNAM.DAT'),32,0);
  OpenIndex(Userpwd,AddDrive(userdrv,'USERPWD.DAT'),10,0);
  lineout(' Search for who?  >',no);
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
  if scratch = 'ALL' then scratch := '';
  stringin := scratch;
  if (length(stringin) > 0) and not Resetnow then begin
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
          stringout := ' Edit account of ' + User.Uname + ' ?';
          lineout(stringout,no);
          Inputchar(ys); q := Upcase(q);
        end;
      if (stringin = scratch) or (q = 'Y') then q := 'Y'
      else OK := no;
      if q = '~' then lineout(' Unknown user.',ys);
    end else lineout(' Unknown user.',ys);
    if OK and not Resetnow then repeat begin
      lineout('',ys);
      lineout('K)ill this account.',ys);
      lineout('Q)uit editing.',ys);
      lineout('1) Name      : ' + User.Uname,ys);
      str(User.Uaccess,stringout);
      lineout('2) Access    : ' + stringout,ys);
      lineout('   Phone     : ' + User.Uphone,ys);
      str(User.Uwidth,stringout);
      lineout('   Width     : ' + stringout,ys);
      str(User.Unulls,stringout);
      lineout('   Nulls     : ' + stringout,ys);
      if User.Ulinefeeds then lineout('   Linefeeds : ON',ys)
      else lineout('   Linefeeds : OFF',ys);
      if User.Ulowcase then lineout('   Lowercase : YES',ys)
      else lineout('   Lowercase : NO',ys);
      str(User.Uevent,stringout);
      lineout('   Event     : ' + stringout,ys);
      str(User.Ucalls,stringout);
      lineout('   Calls     : ' + stringout,ys);
      str(User.Uposts,stringout);
      lineout('   Posts     : ' + stringout,ys);
      str(User.Uploads,stringout);
      lineout('   Uploads   : ' + stringout,ys);
      str(User.Udloads,stringout);
      lineout('   Downloads : ' + stringout,ys);
      if User.Uexpert then lineout('   Expert    : ON',ys)
      else lineout('   Expert    : OFF',ys);
      lineout(' ',ys);
      lineout('USER: 1, 2, K, Q >',no);
      if not Resetnow then inputchar(ys);
      q := Upcase(q);  lineout('',ys);
      if not Resetnow then case q of
        'K' : begin
                stringin := User.Upwd;
                DeleteKey(Userpwd,Userec,stringin);
                if not OK then lineout(' * error deleting userkey',ys);
                stringin := LineUp(User.Uname);
                DeleteKey(Usernam,Userec,stringin);
                if not OK then lineout(' * error deleting name',ys);
                DeleteRec(Userlog,Userec);
                q := 'Q';
              end;
        '1' : begin
                lineout('Enter new name >',no);
                linein(31,ys,ys);
                if not Resetnow then begin
                  stringout := LineUp(User.Uname);
                  DeleteKey(Usernam,Userec,stringout);
                  if not OK then lineout(' * error deleting oldname',ys);
                  stringout := LineUp(stringin);
                  AddKey(Usernam,Userec,stringout);
                  if not OK then lineout(' * error adding newname',ys);
                  User.Uname := stringin;
                  PutRec(Userlog,Userec,User);
                end;
              end;
        '2' : begin
                lineout('Enter new access level >',no);
                x := Intin;
                if not Resetnow then begin
                  User.Uaccess := x;
                  PutRec(Userlog,Userec,User);
                end;
              end;
      end;
    end until (q = 'Q') or Resetnow;
  end;
  CloseFile(Userlog);
  CloseIndex(Userpwd);
  CloseIndex(Usernam);
              end;
        'H' : if access > 95 then begin
                lineout(' Enter hall number > ',no);
                inputchar(ys);
                if not Resetnow then begin
                  scratch := q;
                  stringin := scratch;
                  FindKey(Hallptr,CurrHall,scratch);
                  if OK then begin
                    GetRec(Hallnam,CurrHall,HallVar);
                    lineout(' ',ys);
                    stringout := ' Name   = ' + HallVar.Hallname + ' > ';
                    lineout(stringout,no);
                    linein(30,ys,ys);
                    if not Resetnow then begin
                      if length(stringin)>0 then HallVar.Hallname := stringin;
                      str(HallVar.HallAccess,stringout);
                      stringout := ' Access = ' + stringout + ' > ';
                      lineout(stringout,no);
                      x := Intin;
                      if not Resetnow then begin
                        if x in [0..99] then HallVar.HallAccess := x;
                        lineout(' Kill this hall [N] ? ',no);
                        Inputchar(ys);
                        lineout(' ',ys);
                        if q = cr then q := 'N' else q := Upcase(q);
                      end;
                    end;
                    if (q = 'Y') and not Resetnow then begin
                      DeleteRec(Hallnam,CurrHall);
                      DeleteKey(Hallptr,CurrHall,stringin);
                      if not OK then lineout(' Error on hall delete.',ys);
                    end else PutRec(Hallnam,CurrHall,HallVar);
                  end else begin
                    lineout(' Hall not found.  Create it [N] ? ',no);
                    inputchar(ys);
                    if not Resetnow then begin
                      lineout(' ',ys);
                      if q = cr then q := 'N' else q := Upcase(q);
                      if q = 'Y' then begin
                        HallVar.HallKey    := stringin;
                        HallVar.HallName   := 'Hall ' + stringin;
                        HallVar.HallAccess := 10;
                        RoomVar.RoomKey    := HallVar.HallKey + '01';
                        RoomVar.RoomName   := 'Room 1';
                        RoomVar.RoomAccess := 10;
                        RoomVar.RoomNumber := 1;
                        RoomVar.RoomHidden := no;
                        AddRec(Hallnam,CurrHall,HallVar);
                        AddKey(Hallptr,CurrHall,HallVar.HallKey);
                        if not OK then lineout('Error on hall index add.',ys);
                        AddRec(Roomnam,CurrRoom,RoomVar);
                        AddKey(Roomptr,CurrRoom,RoomVar.RoomKey);
                        if not OK then lineout('Error on room index add.',ys);
                      end;
                    end;
                  end;
                end;
              end;
        'R' : if access > 94 then begin
                lineout(' Enter room number > ',no);
                linein(3,ys,ys);
                if not Resetnow then begin
                 scratch := stringin;
                 SearchKey(Roomptr,CurrRoom,scratch);
                 if OK then repeat begin
                  GetRec(Roomnam,CurrRoom,RoomVar);
                  lineout(' ',ys);
                  repeat begin
                   stringin := RoomVar.RoomKey;
                   lineout(' # ' + stringin,ys);
                   lineout(' K) Kill this room',ys);
                   lineout(' Q) Quit room editor',ys);
                   lineout(' 0) Go to next room',ys);
                   lineout(' ',ys);
                   stringout := ' 1) Room   = ' + RoomVar.Roomname;
                   lineout(stringout,ys);
                   str(RoomVar.RoomAccess,stringout);
                   stringout := ' 2) Access = ' + stringout;
                   lineout(stringout,ys);
                   stringout := ' 3) Hidden = ';
                   if RoomVar.RoomHidden then stringout := stringout + 'Yes'
                   else stringout := stringout + 'No';
                   lineout(stringout,ys);
                   lineout(' ',ys);
                   lineout('>',no);
                   inputchar(ys);
                   lineout(' ',ys);
                   if not Resetnow then begin
                    q := Upcase(q);
                    case q of
                     'K' : begin
                            scratch := RoomVar.RoomKey + '00000';
                            SearchKey(Messptr,CurrMess,scratch);
                            if OK then GetRec(Messhdr,CurrMess,HdrVar);
                            if OK and (RoomVar.RoomKey = copy(HdrVar.HdrMsgKey,1,3)) then
                              lineout(' There are messages in this room.',ys)
                            else begin
                              scratch := RoomVar.RoomKey;
                              DeleteKey(Roomptr,CurrRoom,scratch);
                              if not OK then lineout(' Error on delete key.',ys)
                              else DeleteRec(Roomnam,CurrRoom);
                              SearchKey(Roomptr,CurrRoom,stringin);
                              if OK then GetRec(Roomnam,CurrRoom,RoomVar);
                            end;
                           end;
                     'Q' : OK := no;
                     '0' : NextKey(Roomptr,CurrRoom,scratch);
                     '1' : begin
                            lineout(' Enter new name >',no);
                            if not Resetnow then linein(22,ys,ys);
                            if not Resetnow then begin
                             RoomVar.Roomname := stringin;
                             PutRec(Roomnam,CurrRoom,RoomVar);
                            end;
                           end;
                     '2' : begin
                            lineout(' Enter new access >',no);
                            x := Intin;
                            if (x > 0) and (x <= 99) and not Resetnow then begin
                             RoomVar.RoomAccess := x;
                             PutRec(Roomnam,CurrRoom,RoomVar);
                            end else lineout(' Access not changed.',ys);
                           end;
                     '3' : begin
                            RoomVar.RoomHidden := not RoomVar.RoomHidden;
                            PutRec(Roomnam,CurrRoom,RoomVar);
                           end;
                    end;
                   end;
                  end until Resetnow or (q in ['0','Q']) or not OK;
                 end until not OK or Resetnow
                 else lineout(' No such room.',ys);
                end;
               end;
        'M' : if access > 96 then
               begin
                lineout(' Enter message # to search for >',no);
                linein(5,ys,ys);
                if not Resetnow then
                 begin
                   while length(stringin) < 5 do stringin := '0' + stringin;
                   scratch := stringin;
                   SearchKey(Omssptr,CurrMess,scratch);
                 end;
                while OK and not Resetnow do
                 begin
                   GetRec(Messhdr,CurrMess,HdrVar);
                   stringin := copy(HdrVar.HdrMsgKey,4,5);
                   DoMess;
                   if q = 'Y' then begin
                     scratch := stringin;
                     FindKey(Omssptr,CurrMess,scratch);
                     if OK then NextKey(Omssptr,CurrMess,scratch)
                     else SearchKey(Omssptr,CurrMess,stringin);
                   end
                   else OK := false;
                 end;
               end;
        'X' : if expert then expert := no else expert := ys;
        '?' : Fileout(AddDrive(helpdrv,'HELPUTIL'));
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
