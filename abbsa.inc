{CurrHall points to data record in Hallnam.}
Procedure DoHall;
begin
  GetRec(Hallnam,CurrHall,HallVar);
  if access >= HallVar.Hallaccess then begin
    lineout(' ',ys);
    stringout := ' ' + HallVar.Hallname + '...';
    lineout(stringout,no);
    scratch := HallVar.HallKey + '00';  {Get first room in hall}
    SearchKey(Roomptr,CurrRoom,scratch);
    if OK then repeat DoRoom until (CurrRoom = 0) or Resetnow
    else if CurrHall = 1 then q := 'Q' else q := '>';
  end;
  case q of    {We've left this hall; '>', '<', 'Q', or 'T'.}
    '>' : begin
            NextKey(Hallptr,CurrHall,scratch);
            if not OK then NextKey(Hallptr,CurrHall,scratch);
          end;
    '<' : begin
            PrevKey(Hallptr,CurrHall,scratch);
            if not OK then PrevKey(Hallptr,CurrHall,scratch);
          end;
    'Q' : CurrHall := 0;
    'T' : CurrHall := 0;
  end;
  if not OK then begin
    lineout('Error 24',ys);
    CurrHall := 0;
  end;
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
    scratch := Userkey;
    FindKey(Emssptr,CurrMess,scratch);
    if OK then begin
      lineout(' You have mail...',ys);
      Emess := 0;   {Number of E-mail messages found}
      repeat begin
        Emess := Emess + 1;
        GetRec(Messhdr,CurrMess,HdrVar);
        stringout := Copy(HdrVar.HdrMsgKey,4,5);
        stringout := ' From '+HdrVar.HdrMsgFrom+' # '+stringout;
        lineout(stringout,no);
        z := HdrVar.HdrMsgPtr;
        GetMsg(z);
        OutBuff;
        lineout(' ',ys);
        lineout(' Erase the above message ?',no);
        Inputchar(ys);
        q := Upcase(q);
        lineout(' ',ys);
        if (q = 'Y') and not Resetnow then begin
          Emess := Emess - 1; {One less kept by user}
          z := HdrVar.HdrMsgPtr;
          DelMsg(z);
          DeleteRec(Messhdr,CurrMess);
          stringout := HdrVar.HdrMsgKey;
          DeleteKey(Messptr,CurrMess,stringout);
          if not OK then lineout('Error 7',ys);
          DeleteKey(Emssptr,CurrMess,scratch);
          if not OK then lineout('Error 8',ys);
          stringout := Copy(HdrVar.HdrMsgKey,4,5);
          DeleteKey(Omssptr,CurrMess,stringout);
          if not OK then lineout('Error 9',ys);
          if OK then lineout(' Message erased.',ys)
          else lineout(' Error in message deletion.',ys);
          y := 1; {Seek an unread message}
          FindKey(Emssptr,CurrMess,scratch);
          while OK and (y <= Emess) do
            begin y := y + 1; NextKey(Emssptr,CurrMess,scratch); end;
        end
        else NextKey(Emssptr,CurrMess,scratch);
      end until Resetnow or not OK or (scratch <> Userkey);
    end
    else lineout(' You have no mail...try sending some!',ys);
    if not Resetnow then begin
      scratch  := '1';       {Always enter the first hall.}
      SearchKey(Hallptr,CurrHall,scratch);
      if OK then repeat DoHall until (CurrHall = 0) or Resetnow
      else lineout('Error 25',ys);
      lineout(' ',ys);
    end;
  Close(Message);
  CloseFile(Hallnam);
  CloseFile(Roomnam);
  CloseFile(Messhdr);
  CloseIndex(Hallptr);
  CloseIndex(Roomptr);
  CloseIndex(Messptr);
  CloseIndex(Emssptr);
  CloseIndex(Omssptr);
  UpAbbssys; {if the bbs dies OUTSIDE of message system, the MESS files are ok}
end;
