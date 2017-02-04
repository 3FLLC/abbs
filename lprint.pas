Program Lprint(input, output);

var
   prtfile : text;

begin

   assign(prtfile,'PRN:');
   reset(prtfile);
   writeln(prtfile,'Hello World');
   
end.

