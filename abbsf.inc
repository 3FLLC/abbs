{Must have CurrType}
Procedure DoType;
begin
  GetRec(Typenam,CurrType,TypeVar);
  if OK then begin
    if access >= TypeVar.TypeAccess then
      repeat begin
        lineout(' ',ys);
        if not expert then Fileout(AddDrive(menudrv,'MENUFILE'));
        stringout := ' ' + TypeVar.Typename + '>';
        lineout(stringout,no);
        inputchar(ys);
        if not Resetnow then begin
          q := Upcase(q);
          case q of
            'X' : expert := not expert;
            '?' : if expert then Fileout(AddDrive(menudrv,'MENUFILE'));
            'H' : Fileout(AddDrive(helpdrv,'HELPFILE'));
            'L' : begin
                    lineout(bs+'List files',ys);
                    lineout(' ',ys);
                    lineout('    File       Date  Size  Description',ys);
                    scratch := TypeVar.TypeKey + '           ';
                    SearchKey(Fileptr,CurrFile,scratch);
                    if OK then repeat begin
                      GetRec(Filenam,CurrFile,FileVar);
                      done := (Copy(FileVar.FileKey,1,2) <> TypeVar.TypeKey);
                      if not done and
                       (not FileVar.FileHidden or (access > 90)) then begin
                        if length(FileVar.FilePass) > 0 then begin
                          stringout := '*';
                          q := 'P';
                        end
                        else stringout := ' ';
                        stringout := stringout + Copy(FileVar.FileKey,3,11);
                        stringout := stringout + '  ' + FileVar.FileDate;
                        str(FileVar.FileSize,scratch);
                        stringout := stringout + '  ' + scratch;
                        while length(stringout) < 25 do
                          stringout := stringout + ' ';
                        stringout := stringout + '  ' + FileVar.FileDesc;
                        lineout(stringout,ys);
                      end;
                      NextKey(Fileptr,CurrFile,scratch);
                    end until done or (q = 'S') or not OK;
                    done := no;
                    if q = 'P' then lineout('* - Passworded file.',ys);
                  end;
            'K' : begin
                    lineout(bs+'Known rooms',ys);
                    stringin := TypeVar.TypeKey;
                    scratch := '00';
                    SearchKey(Typeptr,CurrType,scratch);
                    z := 0;
                    if OK then repeat begin
                      GetRec(Typenam,CurrType,TypeVar);
                      if (access >= TypeVar.TypeAccess) then begin
                        stringout := ' ' + TypeVar.Typename + '>';
                        if (z + length(stringout)) > (width + 2) then begin
                          z := 0; lineout('',ys);
                        end;
                        z := z + length(stringout);
                        lineout(stringout,no);
                      end;
                      NextKey(Typeptr,CurrType,scratch);
                    end until not OK;
                    lineout(' ',ys);
                    SearchKey(Typeptr,CurrType,stringin);
                    if OK then GetRec(Typenam,CurrType,TypeVar)
                    else begin
                      lineout('Error 40',ys);
                      q := 'Q';
                    end;
                  end;
            'A' : if access > 98 then begin
                    lineout(bs+'Add file entry',ys);
                    with FileVar do begin
                      FileKey    := '';
                      FileDate   := '010187';
                      FileDrive  := 'B';
                      FileDesc   := '--   40 character file description   --';
                      FileSize   := 0;
                      FilePass   := '';
                      FileXfers  := 0;
                      FileHidden := no;
                      FileAcc300 := 30;
                      FileAcc1200:= 20;
                      FileAcc2400:= 10;
                    end;
                    GetFileName;
                    if not Resetnow then begin
                     FileVar.FileKey := scratch;
                     FindKey(Fileptr,CurrFile,scratch);
                     if OK then lineout(' File already exists.',ys) else begin
                      EditFileEntry;
                      if (length(FileVar.FileKey)>0) and (FileVar.FileSize>0)
                       and not Resetnow then begin
                        AddRec(Filenam,CurrFile,FileVar);
                        AddKey(Fileptr,CurrFile,FileVar.FileKey);
                        if not OK then lineout('Error 41',ys);
                      end else lineout('Must have name and filesize.',ys);
                     end;
                    end;
                  end;
            'P' : if access > 90 then begin
                    lineout(bs+'Purge file entry',ys);
                    GetFileName;
                    if not Resetnow then begin
                      FindKey(Fileptr,CurrFile,scratch);
                      if not OK then
                        lineout(' File not found.',ys)
                      else begin
                        DeleteRec(Filenam,CurrFile);
                        DeleteKey(Fileptr,CurrFile,stringin);
                        if not OK then lineout('Error 42',ys);
                        lineout(' File entry killed.',ys);
                      end;
                    end;
                  end;
            'E' : begin
                    lineout(bs+'Edit file entry',ys);
                    GetFileName;
                    if not Resetnow then begin
                      FindKey(Fileptr,CurrFile,scratch);
                      if not OK then
                        lineout(' File not found.',ys)
                      else begin
                        GetRec(Filenam,CurrFile,FileVar);
                        EditFileEntry;
                        if (FileVar.FileSize > 0) and not Resetnow then
                          PutRec(Filenam,CurrFile,FileVar);
                      end;
                    end;
                  end;
            'S' : begin
                    lineout(bs+'Send file',ys);
                    GetFileName;
                    if not Resetnow then begin
                      stringin := Copy(scratch,3,11);
                      FindKey(Fileptr,CurrFile,scratch);
                      if not OK then
                        lineout(' "'+stringin+'" not found in L)ist.',ys)
                      else begin
                        GetRec(Filenam,CurrFile,FileVar);
                        scratch := EditFileName(Copy(FileVar.FileKey,3,11));
                        lineout(' Sending [' + scratch + ']',ys);
                        scratch := FileVar.FileDrive + ':' + scratch;
                        if Fileaccess then Asciixfer(scratch)
                        else lineout(' File locked.',ys);
                      end;
                    end;
                  end;
            'D' : begin
                    lineout(bs+'Download file',ys);
                    GetFileName;
                    if not Resetnow then begin
                      FindKey(Fileptr,CurrFile,scratch);
                      if not OK then
                        lineout(' File not found.',ys)
                      else begin
                        GetRec(Filenam,CurrFile,FileVar);
                        scratch := EditFileName(Copy(FileVar.FileKey,3,11));
                        lineout(' Sending [' + scratch + ']',ys);
                        scratch := FileVar.FileDrive + ':' + scratch;
                        if Fileaccess then Xmodemsend(scratch)
                        else lineout(' File locked.',ys);
                      end;
                    end;
                  end;
            'U' : begin
                    lineout(bs+'Upload file',ys);
                    lineout(' Upload under construction.',ys);
                  end;
          end;
        end;
        OK := ys;
      end until (q in ['+','-','M','R','Q','T']) or Resetnow;
    case q of
      '+' : begin
              if access >= TypeVar.TypeAccess then lineout(' Next room',no);
              NextKey(Typeptr,CurrType,scratch);
              if not OK then begin
                ClearKey(Typeptr);
                NextKey(Typeptr,CurrType,scratch);
              end;
            end;
      '-' : begin
              if access >= TypeVar.TypeAccess then lineout(' Previous room',no);
              PrevKey(Typeptr,CurrType,scratch);
              if not OK then begin
                ClearKey(Typeptr);
                PrevKey(Typeptr,CurrType,scratch);
              end;
            end;
      'M' : if access > 98 then begin
              lineout(bs+'Make room',ys);
              x := 1;
              ClearKey(Typeptr);
              repeat begin
                NextKey(Typeptr,CurrType,scratch); {First fileroom.}
                Val(scratch,z,y);
                if (z <> x) or not OK then done := true
                else begin
                  Val(scratch,x,y);
                  x := x + 1;
                end;
              end until done or (x > 99);
              if done and (x < 100) then
                begin
                  GetEventStr(x,2);
                  done    := false;
                  OK      := true;
                  lineout(' Enter room name (Hit return to quit) >',no);
                  if not Resetnow then linein(20,ys,ys);
                  if not Resetnow then begin
                    lineout(' Searching...',no);
                    if length(stringin) < 3 then
                      lineout(' Room name must be at least 3 chars long.',ys)
                    else begin
                      stringout := '00';
                      SearchKey(Typeptr,CurrType,stringout);
                      if OK then repeat begin
                        GetRec(Typenam,CurrType,TypeVar);
                        if LineUp(stringin) = LineUp(TypeVar.Typename) then
                          stringin := ' '
                        else NextKey(Typeptr,CurrType,stringout);
                      end until not OK or (stringin = ' ');
                      if stringin = ' ' then lineout('name reserved.',ys)
                      else lineout('not found.',ys);
                    end;
                  end;
                  if not Resetnow and (length(stringin) > 2) then begin
                    stringout := stringin;
                    lineout(' Enter room access >',no);
                    if not Resetnow then x := Intin;
                    TypeVar.TypeKey    := scratch;
                    TypeVar.Typename   := stringout;
                    TypeVar.TypeAccess := x;
                    lineout(' ',ys);
                    stringout := ' Create ' + stringout + '> (Y/N) ?';
                    lineout(stringout,no);
                    if not Resetnow then Inputchar(ys);
                    q := Upcase(q);
                    if not Resetnow and (q = 'Y') then begin
                      stringin := scratch;
                      AddRec(Typenam,CurrType,TypeVar);
                      AddKey(Typeptr,CurrType,stringin);
                      if not OK then lineout(' Error 43',ys);
                      SearchKey(Typeptr,CurrType,scratch);
                    end
                    else if not Resetnow then begin
                      lineout(' Room not created.',ys);
                      ClearKey(Typeptr);
                      NextKey(Typeptr,CurrType,scratch);
                    end;
                  end
                  else begin
                    ClearKey(Typeptr);
                    NextKey(Typeptr,CurrType,scratch);
                  end;
                end
              else begin
                lineout(' No space remaining in file hall.',ys);
                ClearKey(Typeptr);
                NextKey(Typeptr,CurrType,scratch);
              end;
            end;
      'R' : if access > 98 then begin
              lineout(bs+'Room editor',ys);
              lineout(' ',ys);
              stringout := ' Room   = ' + TypeVar.Typename + ' >';
              lineout(stringout,no);
              if not Resetnow then linein(20,ys,ys);
              if length(stringin) > 0 then TypeVar.Typename := stringin;
              str(TypeVar.TypeAccess,stringout);
              stringout := ' Access = ' + stringout + ' >';
              lineout(stringout,no);
              if not Resetnow then x := Intin;
              if x > 0 then TypeVar.TypeAccess := x;
              PutRec(Typenam,CurrType,TypeVar);
              lineout(' Remove this room? ',no);
              if not Resetnow then inputchar(ys);
              q := upcase(q);
              lineout(' ',ys);
              if not Resetnow and (q = 'Y') then begin
                scratch := TypeVar.TypeKey + '           ';
                SearchKey(Fileptr,CurrFile,scratch);
                if OK then GetRec(Filenam,CurrFile,FileVar);
                if OK and (TypeVar.TypeKey = Copy(FileVar.FileKey,1,2))
                  then lineout(' There are files in this room.',ys)
                else begin
                  scratch := TypeVar.TypeKey;
                  stringin := scratch;
                  DeleteKey(Typeptr,CurrType,scratch);
                  if not OK then lineout('Error 44',ys)
                  else DeleteRec(Typenam,CurrType);
                  SearchKey(Typeptr,CurrType,stringin);
                  if OK then GetRec(Typenam,CurrType,TypeVar) else begin
                    scratch := '00';
                    SearchKey(Typeptr,Currtype,scratch);
                  end;
                end;
              end;
            end;
      'Q' : begin CurrType := 0; lineout(bs+'Quit to MAIN',no); end;
      'T' : begin CurrType := 0; lineout(bs+'Terminate',no); end;
    end;
    if not OK then begin CurrType := 0; lineout('Error 45',ys); end;
  end else begin CurrType := 0; lineout('Error 46',ys); end;
end;

begin
  InitIndex;
  OpenFile(Filenam,AddDrive(filedrv,'FILENAM.DAT'),SizeOf(FileVar));
  OpenFile(Typenam,AddDrive(filedrv,'TYPENAM.DAT'),SizeOf(TypeVar));
  OpenIndex(Fileptr,AddDrive(filedrv,'FILEPTR.DAT'),14,0);
  OpenIndex(Typeptr,AddDrive(filedrv,'TYPEPTR.DAT'),3,0);
  scratch  := '00';   {Always enter the first room.}
  SearchKey(Typeptr,CurrType,scratch);
  if OK then repeat DoType until (CurrType = 0) or Resetnow
  else lineout(' File system not active.',ys);
  lineout(' ',ys);
  CloseFile(Filenam);
  CloseFile(Typenam);
  CloseIndex(Fileptr);
  CloseIndex(Typeptr);
end;
