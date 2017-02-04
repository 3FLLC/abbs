Overlay Procedure Games; {The sub-overlays create a .001 file kept on disk}

Overlay Procedure Nuclear;
Var     Distance : Real;
        Yield    : Real;
        Height   : Integer;
        Weather  : Real;
        ThermE   : Real;
        Shelter  : Boolean;
        Flux     : Real;
        Pulse    : Real;
        Rems     : Real;
begin
  lineout(' ',ys);
  lineout(' NUCLEAR EXPLOSION...',ys);
  lineout(' (a mathematical simulation)',ys);
  repeat begin
    lineout(' ',ys);
    lineout(' Enter YOUR distance in miles from the',ys);
    lineout(' detonation site [0-25] > ',no);
    Distance := Intin;
    if not ((Distance >= 0) and (Distance <= 25)) then
      lineout(' Enter a number from 0 to 25.',ys);
  end until Resetnow or ((Distance >= 0) and (Distance <= 25));
  if not Resetnow then repeat begin
    lineout(' ',ys);
    lineout(' Enter the size of the bomb in',ys);
    lineout(' megatons [1..30] > ',no);
    Yield := Intin;
    if not ((Yield >= 1) and (Yield <= 30)) then
      lineout(' Enter a number from 1 to 30.',ys);
  end until Resetnow or ((Yield >= 1) and (Yield <= 30));
  if not Resetnow then repeat begin
    lineout(' ',ys);
    lineout(' Enter the height in feet that the',ys);
    lineout(' bomb was detonated at [0..9999] > ',no);
    Height := Intin;
    if not ((Height >= 0) and (height <= 9999)) then
      lineout(' Enter a number from 0 to 9999.',ys);
    ThermE := 0.3  {??? This should be calculated.}
  end until Resetnow or ((Height >= 0) and (height <= 9999));
  if not Resetnow then repeat begin
    lineout(' ',ys);
    lineout(' 1) Overcast, drizzling',ys);
    lineout(' 2) Cloudy',ys);
    lineout(' 3) Patches of blue',ys);
    lineout(' 4) Beautiful sunny day!',ys);
    lineout(' What''s the weather? ',no);
    Inputchar(ys);
    case q of
     '1' : Weather := 0.25;
     '2' : Weather := 0.4;
     '3' : Weather := 0.6;
     '4' : Weather := 0.75;
    end;
  end until Resetnow or (q in ['1','2','3','4']);
  if not Resetnow then begin
    Shelter := no;
    lineout(' ',ys);
    lineout(' Are you in a bomb shelter [N] ? ',no);
    Inputchar(ys);
    if Upcase(q) = 'Y' then Shelter := ys;
  end;
  if not Resetnow then begin
    Flux := 3000. * ThermE * Weather * Yield / (Distance * Distance + 0.0001);
    Pulse := Sqrt(2. * Height * 3963 / 5280);
    Rems := 250. * 1000 * Yield / (16 * 3.1416 * Distance * Distance + 0.0001);
    lineout(' ',ys);
    lineout(' Thermal flux is the immediate temperature increase in the air',ys);
    lineout(' around you.  Shelters drastically reduce the dangers of flux.',ys);
    if Shelter then begin
      if flux > 15 then
        lineout(' Firestorms are raging outside of your shelter.',ys)
      else
        lineout(' The bomb shelter protects you.',ys);
    end else if flux < 1 then begin
      lineout(' You feel an abrupt warming of the air around you.',ys);
    end else if flux < 5 then begin
      lineout(' Your skin reddens, blistering in some places.',ys);
    end else if flux < 10 then begin
      lineout(' Third degree burns blacken your skin in exposed areas.',ys);
    end else begin
      lineout(' Thermal flux is enough to burn you to a crisp instantly.',ys);
      lineout(' Just to drag it out though, let''s pretend you survived...',ys);
    end;
    lineout(' ',ys);
    lineout(' The electromotive pulse is a "magnetic bubble" that grows',ys);
    lineout(' outward from an atomic explosion.  In this case, you were',ys);
    if Distance > Pulse then
      lineout(' out of range of the force destructive to electronic devices.',ys)
    else begin
      lineout(' within range.  Computers, radios, telephones, cars, and',ys);
      lineout(' almost everything else electrical ceases to operate.',ys);
    end;
    lineout(' ',ys);
    lineout(' Radiation is the emission of a variety of atomic particles',ys);
    lineout(' that have the annoying effect of "disassembling" molecular',ys);
    lineout(' structures - like your body''s fluids and organs!',ys);
    if Rems < 100 then begin
      lineout(' You have survived very low levels of radiation with no',ys);
      lineout(' apparant effects.',ys);
    end else if Rems < 200 then begin
      lineout(' Radiation exposure has made you sick - vomiting, headache,',ys);
      lineout(' some loss of white blood cells and dizziness.  This is good!',ys);
      lineout(' You will recover with only minor genetic damage.',ys);
    end else if Rems < 600 then begin
      lineout(' Your symptoms range from leukopenia to hair loss because of',ys);
      lineout(' radiation exposure.  Hospitalization is required and is',ys);
      lineout(' available to you because you have a reasonable chance to live.',ys);
    end else if Rems < 1000 then begin
      lineout(' You have very severe leukopenia and internal bleeding.',ys);
      lineout(' You have also developed ulcers and infection is likely',ys);
      lineout(' due to radiation exposure.  You need hospitalization, but',ys);
      lineout(' none is available because of the many others who have a',ys);
      lineout(' far better chance to survive than you.  Therefore, you have',ys);
      lineout(' less than a month to live.',ys);
    end else if Rems < 5000 then begin
      lineout(' You have diarrhia, fever, and a general disturbance of',ys);
      lineout(' body chemistry due to high level radiation exposure.',ys);
      lineout(' Treatment may stave off the pain, but will not save you.',ys);
      lineout(' You''ll die looking something like Fred Kreuger in 2 weeks.',ys);
    end else begin
      lineout(' You are experiencing convulsions, tremors, and ataxia',ys);
      lineout(' because of lethal levels of radiation exposure.  No',ys);
      lineout(' treatment will help you.  Nothing will help you.  The',ys);
      lineout(' pain and hallucinations are a once-in-a-lifetime experience',ys);
      lineout(' though.  Too bad you can''t tell your grandkids about it.',ys);
    end;
    lineout(' ',ys);
    lineout(' Now returning you to the relative safety of Gyroscope...',ys);
    lineout(' ',ys);
  end;
end;

Overlay Procedure Abuse;
Type    Phrase   = String[18];
Var     Number   : Integer;
        Phrases  : File of Phrase;
        scratch  : Phrase;
        type1s   : Integer;
        type2s   : Integer;
        type3s   : Integer;
        type4s   : Integer;
begin
  lineout(' ',ys);
  lineout(' Welcome to ABUSE, you masochist...',ys);
  Assign(Phrases,AddDrive(datadrv,'PHRASES.DAT'));
  Reset(Phrases);
  Read(Phrases,scratch); Val(scratch,type1s,x);
  Read(Phrases,scratch); Val(scratch,type2s,x);
  Read(Phrases,scratch); Val(scratch,type3s,x);
  Read(Phrases,scratch); Val(scratch,type4s,x);
  repeat begin
    lineout(' ',ys);
    lineout(' Enter how much abuse you would',ys);
    lineout(' like today [1-20] > ',no);
    Number := Intin;
    if not ((Number >= 1) and (Number <= 20)) then
      lineout(' Idiot!  Try a number from 1 to 20!',ys);
  end until Resetnow or ((Number >= 1) and (Number <= 20));
  if Number > 3 then lineout(' S)top or P)ause output,',ys);
  lineout(' ',ys);
  if not Resetnow then repeat begin
    Number := Number - 1;
    Seek(Phrases,(4 + Random(Type1s)));
    Read(Phrases,scratch);
    stringout := ' You ' + scratch + ' ';
    Seek(Phrases,(4 + Type1s + Random(Type2s)));
    Read(Phrases,scratch);
    stringout := stringout + scratch + ' of ';
    Seek(Phrases,(4 + Type1s + Type2s + Random(Type3s)));
    Read(Phrases,scratch);
    stringout := stringout + scratch + ' ';
    Seek(Phrases,(4 + Type1s + Type2s + Type3s + Random(Type4s)));
    Read(Phrases,scratch);
    Stringout := stringout + scratch + '.';
    lineout(stringout,ys);
  end until Resetnow or (Number = 0) or (q = 'S');
  Close(Phrases);
  lineout(' ',ys);
end;

begin
  OvrDrive(gamedrv);
  repeat begin
    if not expert then Fileout(AddDrive(menudrv,'MENUGAME'))
    else lineout(' GAME: A N ? X Q ',no);
    lineout('>',no);
    inputchar(ys);
    q := Upcase(q);
    lineout(' ',ys);
    if not Resetnow then
    case q of
      'A' : Abuse;
      'N' : Nuclear;
      '?' : Fileout(AddDrive(helpdrv,'HELPGAME'));
      'X' : expert := not expert;
    end;
  end until Resetnow or (q = 'Q');
  OvrDrive(0);  {Set back to default}
end;
