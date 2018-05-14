
/**************** parser.cxx  Sommersemester 2018******************/

/*******   **************/

#ifndef GLOBAL_H

#include "global.h"

#endif


/**
 * lookahead enthält nächsten Eingabetoken
 */
int lookahead;

int exp();

int nextsymbol();


/******************  factor  **********************************************/
/* analysiert wird der korrekte Aufbau eines Faktors 

			

Schnittstelle: 

	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead
							



*/
int factor()
{
    int kind;
    st_entry *found;        // Zeiger auf Eintrag in ST
    int factor_typ;

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "	Faktor";
    }


    switch (lookahead)    // je nach nächstem Eingabesymbol in lookahead
    {
        case KLAUF:    /* Symbol '(' folgt --> (EXPRESSION) erwartet*/

            lookahead = nextsymbol();
            exp();
            if (lookahead == KLZU) {
                // korrekt ; nächstes Symbol lesen --> Ende
                lookahead = nextsymbol();
            }
            else {
                error(27);  // /*27*/   "kein Faktor: Name oder Konstante oder ( E) "
            } // kein Faktor
            break;


        case INTNUM:
            /* Int-Zahl (INTNUMBER) gefunden --> okay */
            lookahead = nextsymbol();

            break;


        case REALNUM:        /* Real-Zahl (REALNUMBER) gefunden --> okay */
            lookahead = nextsymbol();

            break;


        case ID:    /* Identifikator (ID) gefunden  */
            /* Suche Identifikator in Symboltabelle ;
                angewandtes Auftreten -->
                Deklaration muss vorhanden sein
                und also Eintrag in ST */

            found = lookup(idname);


            if (found == NULL) {
                /* nicht gefunden --> Fehler: Id nicht deklariert*/
                error(10);  // /*10*/   "Identifikator nicht deklariert"
            }

            else    // Id in ST gefunden ; Art prüfen

            {
                kind = found->token;    // Art des ST-Eintrags

                switch (kind)
                {
                    case KONST:    // Konstantenname --> okay

                        break;

                    case INTIDENT:// einfache Variable, Typ int --> okay

                        break;

                    case REALIDENT:// einfache Variable, Typ real --> okay

                        break;


                    case PROC:    // Name einer Prozedur in
                        // Factor nicht erlaubt
                        error(20);  // /*20*/   "Prozedurname in Ausdruck nicht erlaubt",
                        // break;

                } // endswitch (kind)

                // nächstes Symbol lesen

                lookahead = nextsymbol();
            }    // endif


            break;

        default:    // kein korrekter Faktor
            error(27);  // /*27*/   "kein Faktor: Name oder Konstante oder ( E) "
    }    // endswitch (lookahead)

    return (0);
}    // end factor









/******************  term ***************************************************/
/* analysiert wird der korrekte Aufbau eines Terms nach folgender Syntax:
			
			TERM	::=		FACTOR  { '*' FACTOR |  '/' FACTOR }*
			
Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead
							Typ des Terms ist Funktionswert

*/
int term()
{
    int ret;

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Term:";
    }


    ret = factor();
    // korrekter Factor

    while (lookahead == MULT || lookahead == DIV)
        // solange * oder / folgt, muss Factor kommen

    {// nächstes Symbol lesen
        lookahead = nextsymbol();
        ret = factor();


    }
    return (0);
}    // end term




/******************  exp ***************************************************/
/* analysiert wird der korrekte Aufbau eines Ausdrucks nach folgender Syntax:
			
			EXPRESSION	::=		TERM { '+' TERM |  '-' TERM}*
			
Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead
							Funktionswert ist Typ des Ausdrucks
*/
int exp()
{
    int typ_left, typ_right;
    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Ausdruck";
    }

    typ_left = term();
    // korrekter Term

    while (lookahead == PLUS || lookahead == MINUS)
        // solange + oder - folgt, muss Term kommen

    {// nächstes Symbol lesen
        lookahead = nextsymbol();
        // Term prüfen
        typ_right = term();
        // nach korrektem Ende wurde nächstes Symbol gelesen

    }
    return (0);
}    // end exp




/******************  condition ***************************************************/
/* analysiert wird der korrekte Aufbau einer Bedingung nach folgender Syntax:
			
			CONDITION	::=		EXPRESSION  RELOP  EXPRESSION
			
Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/
int condition()
{
    int typ_left, typ_right;


    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Condition";
    }


    typ_left = exp();
    // korrekter Ausdruck
    // relationaler Operator muss folgen

    switch (lookahead)
    {
        case EQ:
        case NE:
        case LT:
        case LE:
        case GT:
        case GE:// nächstes Symbol lesen
            lookahead = nextsymbol();
            // Ausdruck muss folgen
            typ_right = exp();

            break;

        default: // kein relationaler Operator
            error(19);  // /*19*/   "Vergleichsoperator erwartet",
    }
    if (typ_left != typ_right) {
        errortext("Typen der Operanden nicht kompatibel");
    }

    return (typ_left);
}  // end condition 




/****************** statement ***************************************************/
/* analysiert wird der korrekte Aufbau eines Statements nach folgender Syntax:
			
			STATEMENT 	::=		IDENT  ':=' EXPRESSION  
							|	call IDENT
							|	begin STATEMENT { ';' STATEMENT }* end
							|	if CONDITION then STATEMENT [else STATEMENT ] fi
							|	while CONDITION do STATEMENT



Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/

void statement()
{
    st_entry *found;        // Zeiger auf ST-Eintrag
    int typ_left, typ_right;

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Statement";
    }


    switch (lookahead)
    {

        case ID :
            // Procedure IDENT

            found = lookup(idname);

            // Check IDENT type (variable or const/proc)
            if (found->token == INTIDENT || found->token == REALIDENT) {

                /*
                 * IDENT is an INT or REAL variable -> Okay!
                 */


                // Parse ASS ":="
                lookahead = nextsymbol();
                if (lookahead == ASS) {

                    // Parse EXPRESSION
                    lookahead = nextsymbol();
                    exp();
                }
            }
            break;


        case CALL:

            lookahead = nextsymbol();
            found = lookup(idname);

            if (found->token == PROC) {
                lookahead = nextsymbol();
            }
            else {
                // lookahead != ID
                error(10);  // /*10*/   "Identifikator nicht deklariert"
            }

            break;


        case BEGIN:

            // Parse at least one STATEMENT
            do {
                lookahead = nextsymbol();
                statement();

                // If lookahead = ';' --> Parse next STATEMENT
            } while (lookahead == SEMICOLON);

            // Close BEGIN-STATEMENT
            if (lookahead == END) {
                lookahead = nextsymbol();
            }
            else {
                error(16);  // /*16*/   "end oder ';' erwartet "
            }
            break;


        case IF:
            // Parse an IF-THEN-ELSE-FI statement

            // Parse the condition
            lookahead = nextsymbol();
            condition();

            // Parse the required THEN-statement
            if (lookahead == THEN) {
                lookahead = nextsymbol();
                statement();
            }
            else {
                error(15);  // /*15*/   "then erwartet"
            }

            // Parse the (optional) ELSE-statement;
            if (lookahead == ELSE) {
                lookahead = nextsymbol();
                statement();
            }

            // Close the IF-statement with FI
            if (lookahead == FI) {
                // All right-> IF-THEN-[ELSE]-FI done
                lookahead = nextsymbol();
            }
            else {
                error(39);  // /*39*/  " fi fehlt"
            }
            break;

        case WHILE:

            lookahead = nextsymbol();
            condition();

            if (lookahead == DO) {
                lookahead = nextsymbol();
                statement();
            }
            else {
                error(17);  // /*17*/   "do erwartet "
            }
            break;

        default:
            error(30);  // /*30*/   "Statement erwartet"
            break;
    }


    return;    // end statement
}



/****************** procdecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Prozedurdeklaration 
nach folgender Syntax:
			
			PROCDECL 	::=		{procedure IDENT ';' BLOCK ';' }*



Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schlüsselwort procedure 
							nächstes Eingabesymbol befindet sich in lookahead 
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/
void procdecl()
{
    st_entry *neu;

    if (tracesw) {
        trace << "\n Zeile:" << lineno << "Procdeklaration:";
    }


    // PROCDECL 	::=		{procedure IDENT ';' BLOCK ';' }*

    do  {
        // Get next symbol
        lookahead = nextsymbol();

        // Assure next symbol is an ID and not defined twice.
        if (lookahead = ID && !lookup_in_actsym(idname)) {
            // All right -> create new procedure
            neu = insert(PROC);
            lookahead = nextsymbol();

            // Parse the BLOCK of the procedure
            if (lookahead == SEMICOLON) {

                lookahead = nextsymbol();
                block(neu->subsym);
            }
        }

        // Parse next procedure
    } while (lookahead == PROCEDURE);

    if (lookahead == SEMICOLON) {
        lookahead = nextsymbol();
    }


    return;   // end procdecl
}



/****************** vardecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Variablendeklaration 
nach folgender Syntax:
			
			VARDECL 	::=		  var IDENT ' : ' TYP  { ',' IDENT ' : ' TYP} *  ';' 			
			  
				
Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schlüsselwort var 
							nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/
void vardecl()
{

    if (tracesw) {
        trace << "\n Zeile:" << lineno << "Variablendeklaration:";
    }


    // Parse several variables
    do {
        // Assure next symbol is an ID and not defined twice
        lookahead = nextsymbol();
        if (lookahead == ID && !lookup_in_actsym(idname)) {

            // All right -> Parse COLON
            lookahead = nextsymbol();
            if (lookahead == COLON) {

                // All right -> Parse type of variable
                lookahead = nextsymbol();
                switch (lookahead)
                {
                    case INT:
                        insert(INTIDENT);
                        break;

                    case REAL:
                        insert(REALIDENT);
                        break;

                    default:
                        error(36);  // /*36*/   "Unzulässiger Typ",
                        break;
                }

            }
        }

        // Parsing variables done, expecting KOMMA (next variable) or SEMICOLON (end of vardecl).
        lookahead = nextsymbol();

    } while (lookahead == KOMMA);

    // Variable declarations must end with a SEMICOLON
    if (lookahead != SEMICOLON) {
        error(5);   // /* 5*/   " ';' oder ',' fehlt ",
    }

    lookahead = nextsymbol();

    return;    // end vardecl
}








/****************** constdecl ***************************************************/
/* analysiert wird der korrekte Aufbau einer Variablendeklaration 
nach folgender Syntax:
			
			CONSTDECL 	::=	 const IDENT '=' INTNUMBER {',' IDENT '=' INTNUMBER } * ';' 
			
			  
				
Schnittstelle: 
	bei Aufruf :			erkannt wurde das Schlüsselwort const 
							nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/
void constdecl()
{

    if (tracesw) {
        trace << "\n Zeile:" << lineno << "Konstantendeklaration:";
    }

    // Parse several constants
    do {
        // Assure next symbol is an ID and not defined twice
        lookahead = nextsymbol();
        if (lookahead == ID && !lookup_in_actsym(idname)) {

            // All right -> Parse '='
            lookahead = nextsymbol();
            if (lookahead == EQ) {
                // All right -> Parse type of constant
                lookahead = nextsymbol();

                switch (lookahead)
                {
                    case INTNUM:
                        insert(KONST);
                        lookahead = nextsymbol();
                        break;

                    default:
                        // Not a vlaid type -> Error
                        error(38);    // /*38*/   "Keine korrekte reelle Konstante "
                }

            }
        }

        // Parse next constant
    } while (lookahead == KOMMA);

    // Constants declaration must end with a SEMICOLON
    if (lookahead != SEMICOLON) {
        error(5);   // /* 5*/   " ';' oder ',' fehlt ",
    }

    lookahead = nextsymbol();

    return;        // end constdecl
}




/****************** block ***************************************************/

/* analysiert wird der korrekte Aufbau eines Blockes nach folgender Syntax:

			
		BLOCK		::= 	[ CONSTDECL ]
							[ VARDECL ]
							  PROCDECL 
							STATEMENT 



Der Parameter neusym ist ein Zeiger auf die Symboltabelle des zu 
analysierenden Blockes 
===> 
		äussersten (globalen)  Block:		firstsym 
		bei Prozeduren:		Zeiger auf neu angelegte ST für Prozedur 
		
Zu Beginn muss der globale Zeiger actsym auf die neue ST gesetzt werden
Vor Verlassen muss actsym wieder auf den vorigen Wert gesetzt werden

				
Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead


*/
void block(symtable *neuSymboltable)
{
    /* symtable *neuSymboltable :	Zeiger auf neue ST */


    if (tracesw) {
        trace << "\n Zeile:" << lineno << "Block";
    }

    // actsym auf neue Symboltabelle setzen
    neuSymboltable->precsym = actsym;
    actsym = neuSymboltable;

    // Parse BLOCK

    // Check if there is a CONSTDECL
    if (lookahead == CONST) {
        // There is at least one CONSTDECL (constant declaration)
        constdecl();
    }

    // Check if there is a VARDECL
    if (lookahead == VAR) {
        // There is at least one CONSTDECL (constant declaration)
        vardecl();
    }

    // Print symbol from symtable
    printsymtab(actsym);

    // Check if there is a PROCDECL
    if (lookahead == PROCEDURE) {
        // There is at least one PROCDECL (procedure declaration)
        procdecl();
    }

    // Parse a STATEMENT; Each block must contain a statement
    statement();


    // bei Blockende : Symboltabelle zurücksetzen
    // actsym = Zeiger auf vorherige Symboltabelle
    //actsym = oldSymboltable;
    actsym = neuSymboltable->precsym;

    return;        // end block
}





/****************** program    ***************************************************/
/* analysiert wird der korrekte Aufbau eines Programmes nach folgender Syntax:
			

			PROGRAM		::=		BLOCK	'$' 
				
			

				
Schnittstelle: 
	bei Aufruf :			nächstes Eingabesymbol befindet sich in lookahead
	bei korrektem Ende:		nächstes Eingabesymbol befindet sich in lookahead

*/
void program()
{

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Programm";
    }


    // globale Symboltabelle  anlegen (firstsym
    firstsym = create_newsym();

    // erstes Symbol lesen
    lookahead = nextsymbol();

    // Block muss folgen
    block(firstsym);

    //  nach Block muss '$' folgen
    if (lookahead == PROGEND)
    {
        // nächstes Symbol lesen
        lookahead = nextsymbol();
    }

    else
    {
        // korrektes Programmende fehlt
        error(31);  // /*31*/   "Korrektes Programmende fehlt"
    }

    // Dateiende erreicht ?
    if (lookahead != DONE)
    {
        error(33);  // /*33*/   "Nach PROGRAM noch Symbole in Eingabedatei"
    } // noch Symbole in Eingabedatei nach RPOGRAM

}    // end program

