Program Runme;

type    String255 = string[255];
        String80  = string[80];
        String14  = string[14];
        SysVars   = Integer;

var     FilVar    : text;       {Text file names.}
        FilVar2   : text;
        SysFil    : file of SysVars;
        event     : Integer;    {This event number}
        newaccess : Integer;    {Access to assign a new user}
        chataccess: Integer;    {Minimum access for Chat}
        feedaccess: Integer;    {Minimum access for Feedback}
        messaccess: Integer;    {Minimum access for Messages}
        fileaccess: Integer;    {Minimum access for Files}
        messrecs  : Integer;    {Maximum MESSAGE.DAT records allowed}
        currrecs  : Integer;    {Current MESSAGE.DAT active records}
        logons    : Integer;    {Total number of system logons}
        menudrv   : Integer;    {Drive containing MENU text files}
        helpdrv   : Integer;    {Drive containing HELP text files}
        messdrv   : Integer;    {Drive containing MESSAGE.DAT}
        namedrv   : Integer;    {Drive containing MESSNAM.DAT}
        keyfdrv   : Integer;    {Drive containing ROOM, HALL, & other MESS}
        userdrv   : Integer;    {Drive containing USER files}
        filedrv   : Integer;    {Drive containing FILE hall, type, & name}
        bulldrv   : Integer;    {Drive containing bulletin text files}
        q         : Char;
        x,y,z     : Integer;

Begin
  Assign(SysFil,'ABBSSYS.DAT');  {Note - ABBSSYS MUST be on default drive}
  Reset(SysFil);
  Read(SysFil,logons,event,messrecs,currrecs,chataccess,messaccess);
  Read(SysFil,feedaccess,newaccess,fileaccess);
  Close(SysFil);
  writeln('This program is used to configure ABBS to search in the');
  writeln('correct spots for your files.  The default is to have all');
  writeln('data files on one drive - the same drive that ABBS is run');
  writeln('from.  For each question, enter 0 for default drive, 1 to');
  writeln('use A:, 2 for B:, 3 for C: etc...');
  writeln(' ');
  write('MENU text files are located on drive >');
  readln(menudrv);
  write('HELP text files are located on drive >');
  readln(helpdrv);
  write('MESSAGE.DAT is located on drive      >');
  readln(messdrv);
  write('MESSNAM.DAT is located on drive      >');
  readln(namedrv);
  write('ROOM, HALL, and other MESS files     >');
  readln(keyfdrv);
  write('USER log files                       >');
  readln(userdrv);
  write('FILE and TYPE data files             >');
  readln(filedrv);
  write('Other files; bulletins, signon, etc..>');
  readln(bulldrv);
  Assign(SysFil,'ABBSSYS.DAT');
  Rewrite(SysFil);
  Write(SysFil,logons,event,messrecs,currrecs,chataccess,messaccess);
  Write(SysFil,feedaccess,newaccess,fileaccess,menudrv,helpdrv,messdrv);
  Write(SysFil,namedrv,keyfdrv,userdrv,filedrv,bulldrv);
  Close(SysFil);
  writeln(' End of Job');
end.
