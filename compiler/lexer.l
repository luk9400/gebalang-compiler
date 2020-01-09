%option noyywrap
%option yylineno

%{
#include <string>
#include <iostream>

#include "symbol.hpp"
#include "parser.tab.hpp"

int yylex();
%}

pidentifier     [_a-z]+
digits           -?[0-9]+
specialChars    [\;\(\)\:\,]

%x comment

%%
"DECLARE"       { return DECLARE; }
"BEGIN"         { return TBEGIN; }
"END"           { return END; }
"ASSIGN"        { return ASSIGN; }

"FOR"           { return FOR; }
"FROM"          { return FROM; }
"DOWNTO"        { return DOWNTO; }
"TO"            { return TO; }
"ENDFOR"        { return ENDFOR; }

"WHILE"         { return WHILE; }
"DO"            { return DO; }
"ENDDO"         { return ENDDO; }
"ENDWHILE"      { return ENDWHILE; }

"READ"          { return READ; }
"WRITE"         { return WRITE; }

"PLUS"          { return PLUS; }
"MINUS"         { return MINUS; }
"TIMES"         { return TIMES; }
"DIV"           { return DIV; }
"MOD"           { return MOD; }

"IF"            { return IF; }
"THEN"          { return THEN; }
"ELSE"          { return ELSE; }
"ENDIF"         { return ENDIF; }

"EQ"            { return EQ; }
"NEQ"           { return NEQ; }
"LE"            { return LE; }
"GE"            { return GE; }
"LEQ"           { return LEQ; }
"GEQ"           { return GEQ; }

{pidentifier}   { yylval.pidentifier = new std::string(yytext); return pidentifier;}  
{digits}        { yylval.num = std::stoll(yytext); return num; }
{specialChars}  { return yytext[0]; }

"["             BEGIN(comment);
<comment>"]"    BEGIN(INITIAL);
<comment>.|\n   ;

[ \t\n\r]+      ;
.               { return ERROR; }
%%