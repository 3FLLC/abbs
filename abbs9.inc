{CurrRoom points to data record in Roomnam.}
Procedure DoRoom;
begin
  GetRec(Roomnam,CurrRoom,RoomVar);
  if (access >= RoomVar.RoomAccess) and
     (HallVar.HallKey = copy(RoomVar.RoomKey,1,1)) then
   repeat begin
      lineout(' ',ys);
      stringout := ' ' + RoomVar.Roomname + '>';
      lineout(stringout,no);
      inputchar(ys);
      if not Resetnow then
        begin
          q := Upcase(q);
          case q of
            'J' : begin
                   lineout(bs+'Jump to room >',no);
                   linein(22,ys,ys);
                   stringout := RoomVar.RoomKey;
                   scratch := HallVar.HallKey + '00';
                   SearchKey(RoomPtr,CurrRoom,scratch);
                   if OK then begin
                     repeat begin
                       GetRec(Roomnam,CurrRoom,RoomVar);
                       if OK then begin
                         if LineUp(RoomVar.Roomname) = LineUp(stringin) then begin
                            if access >= RoomVar.RoomAccess then
                              done := ys else OK := no;
                           end
                         else begin
                           NextKey(Roomptr,CurrRoom,scratch);
                           if (copy(scratch,1,1) <> HallVar.HallKey) then
                             OK := no;
                         end;
                       end;
                     end until done or not OK;
                     if not done or not OK then begin
                       SearchKey(Roomptr,CurrRoom,stringout);
                       GetRec(Roomnam,CurrRoom,RoomVar);
                       lineout(' No such room in hall.',ys);
                     end;
                     done := false;
                   end;
                  end;
            'K' : begin
                    lineout(bs+'Known rooms:',ys);
                    stringin := RoomVar.RoomKey;
                    scratch := HallVar.HallKey + '00';
                    SearchKey(Roomptr,CurrRoom,scratch);
                    z := 0;
                    if OK then repeat begin
                      GetRec(Roomnam,CurrRoom,RoomVar);
                      if (access >= RoomVar.RoomAccess) and
                         not RoomVar.RoomHidden then begin
                        stringout := ' ' + RoomVar.Roomname + '>';
                        if (z + length(stringout)) > (width - 1) then
                          begin z := 0; lineout('',ys); end;
                        z := z + length(stringout);
                        lineout(stringout,no);
                      end;
                      NextKey(Roomptr,CurrRoom,scratch);
                    end until not OK or (Copy(scratch,1,1)<>HallVar.HallKey);
                    lineout('',ys);
                    SearchKey(Roomptr,CurrRoom,stringin);
                    if OK then GetRec(Roomnam,CurrRoom,RoomVar)
                    else begin
                      lineout('Error 21',ys);
                      q := 'Q';
                    end;
                  end;
            'E' : if access >= mailaccess then begin
                    lineout(bs+'Enter message',ys);
                    writeln(':',name);
                    lineout(' To who?  (Return for ALL) >',no);
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
                    HdrVar.HdrExcl    := false;
                    HdrVar.HdrMsgTo   := '';
                    if (length(stringin) > 0) and not Resetnow then begin
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
                        if q = '~' then lineout(' Unknown user.',ys);
                      end else lineout(' Unknown user.',ys);
                      CloseFile(Userlog);
                      CloseIndex(Usernam);
                    end;
                    if OK and not Resetnow then
                      begin
                        p := 0;  {Clear buffer.}
                        EditBuff;
                        if (q = 'S') and not Resetnow then
                         begin
                           if HdrVar.HdrExcl then lineout(' Private # ',no)
                           else lineout(' Public # ',no);
                           event := event + 1;
                           posts := posts + 1;
                           GetEventStr(event,5);
                           scratch := RoomVar.RoomKey + scratch;
                           HdrVar.HdrMsgKey  := scratch;
                           HdrVar.HdrMsgFrom := name;
                           HdrVar.HdrMsgPtr  := AddMsg;
                           AddRec(Messhdr,CurrMess,HdrVar);
                           AddKey(Messptr,CurrMess,HdrVar.HdrMsgKey);
                           if not OK then lineout('Error 1',ys);
                           if HdrVar.HdrExcl then begin
                             AddKey(Emssptr,CurrMess,HdrVar.HdrMsgTo);
                             if not OK then lineout('Error 2',ys);
                           end;
                           scratch := Copy(HdrVar.HdrMsgKey,4,5);
                           AddKey(Omssptr,CurrMess,scratch);
                           if not OK then lineout('Error 3',ys);
                           lineout(scratch,no);
                           while OK and ((currrecs+14)>messrecs) do Scroll;
                           if not OK then lineout(' scrolled!!',ys)
                           else lineout(' saved.',ys);
                         end else lineout(' Message tossed.',ys);
                      end;
                    q := 'E';
                  end else lineout(bs+'Enter locked - leave F)eedback.',ys);
            'D' : begin
                    lineout(cr+lf+' Delete message number >',no);
                    x := Intin;
                    GetEventStr(x,5);
                    FindKey(Omssptr,CurrMess,scratch);
                    if OK and not Resetnow then begin
                      GetRec(MessHdr,CurrMess,HdrVar);
                      if (name=HdrVar.HdrMsgFrom) or (access > 98) then KillMsg
                      else lineout(' Only sender may delete.',ys);
                    end else lineout(' Not found.',ys);
                  end;
            '?' : Fileout(AddDrive(helpdrv,'HELPMESS'));
            'H' : Fileout(AddDrive(helpdrv,'HELPMORE'));
            else if q in ['R','O'] then
                  begin
                    if q = 'O' then begin
                      lineout(bs+'Old messages',ys);
                      GetEventStr(lastevent,5);
                    end
                    else begin
                      lineout(bs+'Reverse read',ys);
                      scratch := '99999';
                    end;
                    if not expert then
                      lineout(' N)ext  S)top  or P)ause...',ys);
                    scratch := RoomVar.RoomKey + scratch;
                    SearchKey(Messptr,CurrMess,scratch);
                    if not OK then ClearKey(Messptr);
                    repeat begin
                      PrevKey(Messptr,CurrMess,scratch);
                      if OK then begin
                        GetRec(Messhdr,CurrMess,HdrVar);
                        if OK and not HdrVar.HdrExcl and
                        (copy(HdrVar.HdrMsgKey,1,3) = RoomVar.RoomKey) then
                          begin
                            if q = 'N' then q := 'R';
                            lineout(' ',ys);
                            if not (q in ['S','N']) then begin
                              GetMsg(HdrVar.HdrMsgPtr);
                              stringout := Copy(HdrVar.HdrMsgKey,4,5);
                              stringout := ' From '+HdrVar.HdrMsgFrom+' # '+stringout;
                              if width > 45 then lineout(stringout,no)
                              else lineout(stringout,ys);
                              if not (q in ['S','N']) then OutBuff;
                            end;
                          end;
                      end;
                    end until not OK or (q in ['S','s']) or Resetnow or
                              (copy(scratch,1,3) <> RoomVar.RoomKey);
                  end
             else if q in ['N','F'] then   {N)ew or F)orward}
                  begin
                    if q = 'N' then begin
                      lineout(bs+'New messages',ys);
                      GetEventStr(lastevent,5);
                    end
                    else begin
                      lineout(bs+'Forward read',ys);
                      scratch := '00000';
                    end;
                    if not expert then
                      lineout(' N)ext  S)top  or P)ause...',ys);
                    scratch := RoomVar.RoomKey + scratch;
                    SearchKey(Messptr,CurrMess,scratch);
                    if OK then
                     repeat begin
                       GetRec(Messhdr,CurrMess,HdrVar);
                       if OK and not HdrVar.HdrExcl and
                       (copy(HdrVar.HdrMsgKey,1,3) = RoomVar.RoomKey) then
                         begin
                          if q = 'N' then q := 'F';
                          lineout(' ',ys);
                          if not (q in ['S','N']) then begin
                            GetMsg(HdrVar.HdrMsgPtr);
                            stringout := Copy(HdrVar.HdrMsgKey,4,5);
                            stringout := ' From '+HdrVar.HdrMsgFrom+' # '+stringout;
                            if width > 45 then lineout(stringout,no)
                            else lineout(stringout,ys);
                            if not (q in ['S','N']) then OutBuff;
                          end;
                         end;
                       NextKey(Messptr,CurrMess,scratch);
                     end until not OK or (q in ['S','s']) or Resetnow or
                         (copy(scratch,1,3) <> RoomVar.RoomKey);
                  end;
          end; {of case}
          OK := true;
        end
      else q := 'T';
   end until (q in ['+','-','G','M','<','>','Q','T']) or Resetnow;
  case q of
    '+' : begin
           lineout(' Next room',no);
           repeat begin
            NextKey(Roomptr,CurrRoom,scratch);
            if not OK or (copy(scratch,1,1) <> HallVar.HallKey) then
             begin
              scratch := HallVar.HallKey + '00';
              SearchKey(Roomptr,CurrRoom,scratch);
             end;
            GetRec(Roomnam,CurrRoom,RoomVar);
            if not RoomVar.RoomHidden then done := ys;
           end until done or not OK;
           done := no;
          end;
    '-' : begin
           lineout(' Previous room',no);
           repeat begin
            PrevKey(Roomptr,CurrRoom,scratch);
            if not OK or (copy(scratch,1,1) <> HallVar.HallKey) then
             begin
              scratch := HallVar.HallKey + '99';
              SearchKey(Roomptr,CurrRoom,scratch);
              if not OK then ClearKey(Roomptr);
              PrevKey(Roomptr,CurrRoom,scratch);
             end;
            GetRec(Roomnam,CurrRoom,RoomVar);
            if not RoomVar.RoomHidden then done := ys;
           end until done or not OK;
           done := no;
          end;
    'G' : begin
            lineout(bs+'Goto new messages',ys);
            stringin := RoomVar.RoomKey;
            repeat begin
              NextKey(Roomptr,CurrRoom,stringout);
              if OK then GetRec(Roomnam,CurrRoom,RoomVar);
              if (copy(stringout,1,1) <> HallVar.HallKey) or not OK then
                OK := false
              else if RoomVar.RoomHidden = no then begin
                GetEventStr(lastevent,5);
                scratch := stringout + scratch;
                SearchKey(Messptr,CurrMess,scratch);
                if OK then begin
                  GetRec(Messhdr,CurrMess,HdrVar);
                  {stringout is the room we're testing in}
                  if OK then with HdrVar do
                   if copy(HdrMsgKey,1,3) = stringout then done := true;
                end;
              end;
            end until done or not OK;
            if not done or not OK then begin
              SearchKey(Roomptr,CurrRoom,stringin);
              lineout(' No more new messages in this hall.',ys);
            end;
            done := false;
          end;
    'M' : begin
            lineout(bs+'Make room',ys);
            scratch  := HallVar.HallKey + '01';
            x        := 1;
            SearchKey(Roomptr,CurrRoom,scratch);
            if not OK then ClearKey(Roomptr);
            PrevKey(Roomptr,CurrRoom,scratch);
            repeat begin
              NextKey(Roomptr,CurrRoom,scratch); {First room in hall.}
              Val(Copy(scratch,2,2),z,y);
              if (z <> x) or (copy(scratch,1,1) <> HallVar.HallKey) or
                not OK then done := true
              else begin
                Val(Copy(scratch,2,2),x,y);
                x := x + 1;
              end;
            end until done or (x > 99);
            str(access,scratch);
            if done and (x < 100) and (copy(scratch,length(scratch),1) <> '0')
             then begin
              GetEventStr(x,2);
              scratch := HallVar.HallKey + scratch;
              OK      := true;
              lineout(' Enter room name (Hit return to quit) >',no);
              linein(22,ys,ys);
              lineout(' Searching...',no);
              if length(stringin) < 3 then
                lineout(' Room name must be at least 3 chars long.',ys)
              else begin
                stringout := '000';
                SearchKey(Roomptr,CurrRoom,stringout);
                if OK then repeat begin
                  GetRec(Roomnam,CurrRoom,RoomVar);
                  if LineUp(stringin) = LineUp(RoomVar.Roomname) then
                    stringin := ' '
                  else NextKey(Roomptr,CurrRoom,stringout);
                end until not OK or (stringin = ' ');
                if stringin = ' ' then lineout('name reserved.',ys)
                else lineout('not found.',ys);
              end;
              if not Resetnow and (length(stringin) > 2) then begin
                lineout(' Make it a hidden room (Y/N) ?',no);
                Inputchar(ys);
                q := Upcase(q);
                RoomVar.RoomKey    := scratch;
                RoomVar.Roomname   := stringin;
                RoomVar.RoomAccess := 10;
                RoomVar.RoomNumber := x;
                RoomVar.RoomHidden := (q = 'Y');
                lineout(' ',ys);
                stringout := ' Create ' + stringin + '> (Y/N) ?';
                lineout(stringout,no);
                if not Resetnow then Inputchar(ys);
                q := Upcase(q);
                if not Resetnow and (q = 'Y') then begin
                  stringin := scratch;
                  AddRec(Roomnam,CurrRoom,RoomVar);
                  AddKey(Roomptr,CurrRoom,stringin);
                  if not OK then lineout(' Error 22',ys);
                  SearchKey(Roomptr,CurrRoom,scratch);
                end
                else if not Resetnow then begin
                  lineout(' Room not created.',ys);
                  scratch := HallVar.HallKey + '00';
                  SearchKey(Roomptr,CurrRoom,scratch);
                end;
              end
              else begin
                scratch := HallVar.HallKey + '00';
                SearchKey(Roomptr,CurrRoom,scratch);
              end;
            end
            else begin
              lineout(' Cannot make room.',ys);
              scratch := HallVar.HallKey + '00';  {Get first room}
              SearchKey(Roomptr,CurrRoom,scratch);
            end;
            done := false;
          end;
    '>' : begin CurrRoom := 0; lineout(' Next hall',no); end;
    '<' : begin CurrRoom := 0; lineout(' Previous hall',no); end;
    'Q' : begin CurrRoom := 0; lineout(bs+'Quit',no); end;
    'T' : begin CurrRoom := 0; lineout(bs+'Terminate',no); end;
  end;
  if not OK then begin
    lineout('Error 23',ys);
    CurrRoom := 0;
  end;
end;
