
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
            if (lookahead == KLZU)
            {
                // korrekt ; nächstes Symbol lesen --> Ende
                lookahead = nextsymbol();
            }
            else
            {
                error(27);
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


            if (found == NULL)
                /* nicht gefunden --> Fehler: Id nicht deklariert*/
                error(10);

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
                        error(20); // --> exit
                        // break;

                } // endswitch (kind)

                // nächstes Symbol lesen

                lookahead = nextsymbol();
            }    // endif


            break;

        default:    // kein korrekter Faktor
            error(27);
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
            error(19);
    }
    if (typ_left != typ_right)
    {
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


    switch(lookahead){

        case ID :
            // Procedure IDENT
            if(lookahead == ID) {
                // Found identifier in symtable
                st_entry *found = lookup(idname);

                // Check IDENT type (variable or const/proc)
                if(found->token == INTIDENT || found->token == REALIDENT) {

                    /*
                     * IDENT is an INT or REAL variable -> Okay!
                     */


                    // Parse ASS ":="
                    lookahead = nextsymbol();
                    if(lookahead == ASS) {

                        // Parse EXPRESSION
                        lookahead = nextsymbol();
                        exp();

                    }
                    else {
                        // Not an ":="
                        error(12);   // Expected ':=' after IDENT
                    }

                }
                else if(found->token == PROC || found->token == KONST) {
                    // IDENT is a procedure or constant
                    error(11);  // Keine Zuweisung an Konstanten oder Prozeduren zulässig
                }
                else {
                    // IDENT is not in symbol table
                    error(10);  // IDENT not defined
                }
            }
            else {
                // lookahead != ID
                error(13); // Expected identifier
            }

            break;
        case CALL:

            lookahead = nextsymbol();
            if(lookahead == ID){
                if(lookahead == PROC ) {
                    // IDENT is a PROC -> OK

                } else{
                    errortext("Expected PROC!");
                }



            }
            else{
                // lookahead != ID
                error(10); // Expected identifier
            }

            break;


        case BEGIN:

            do{
                lookahead = nextsymbol();
                statement();


            }while(lookahead == SEMICOLON);

            if(lookahead == END){

            }
            else{
                error(16); // END expected
            }
            break;




        case IF:

            lookahead = nextsymbol();

            condition();

            if(lookahead == THEN){
                lookahead = nextsymbol();
                statement();

            } else{
                error(15); //THEN expected
            }

            lookahead = nextsymbol();
            if(lookahead == ELSE){
                lookahead = nextsymbol();
                statement();
            }
            else if(lookahead == FI){

            }
            else{
                error(39);  // FI not found
            }
            break;

        case WHILE:

            lookahead = nextsymbol();
            condition();

            if (lookahead == DO){
                lookahead = nextsymbol();
                statement();
            }
            else{
                error(17); // expected DO
            }
            break;

        default:
            error(30); // Statement not found
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

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Procdeklaration:";
    }


    // PROCDECL 	::=		{procedure IDENT ';' BLOCK ';' }*

    while (lookahead == PROCEDURE) {

        // PROCDECL starts with 'procedure'
        if (lookahead != PROCEDURE) {
            errortext("Found procedure declaration and expected key word 'procedure'.");
        }


        // Procedure IDENT
        lookahead = nextsymbol();   // read IDENT
        if(lookahead == ID) {
            st_entry *found = lookup(idname);
            if(found != NULL) {
                // IDENT not in symtable

                neu = insert(PROC);
            }
            else {
                // IDENT is already in symbol table
                error(34);  // Redeclaration error
            }
        }
        else {
            error(13); // Expected identifier
        }


        // Parse ';' (SEMICOLON)
        lookahead = nextsymbol();
        if (lookahead == SEMICOLON) {
            /* ':' (COLON) gefunden --> okay */
        }
        else if(lookahead == KOMMA) {
                errortext("Procedure declaration and block end with ';'.");
        }
        else if (lookahead != SEMICOLON) {
            error(5);   // Expected '=' after IDENT
        }




        // Parse BLOCK
        lookahead = nextsymbol();
        symtable *newSym = create_newsym(); // Create new symbol table for BLOCK
        newSym->precsym = actsym;           // Set pointer to presym
        block(newSym);                      // Parse BLOCK with its own symbol table.


        // Parse ';' (SEMICOLON)
        if (lookahead == SEMICOLON) {
            /* ':' (COLON) gefunden --> okay */
        }
        else if(lookahead == KOMMA) {
            errortext("Procedure declaration and block end with ';'.");
        }
        else if (lookahead != SEMICOLON) {
            error(5);   // Expected '=' after IDENT
        }


        // Read next symbol (for next loop)
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
    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Variablendeklaration:";
    }


    // CONSTDECL starts with 'var'
    if(lookahead != VAR) {
        errortext("Found variable declaration and expected key word 'var'.");
    }

    // Parse identifier declaration at least once:
    //                  IDENT '=' INTNUMBER {',' IDENT '=' INTNUMBER } *
    do {

        // nach var muss Identifikator folgen
        lookahead = nextsymbol();   // read IDENT
        if (lookahead == ID) {
            st_entry *found = lookup(idname);
            if (found != NULL) {
                // IDENT not in symtable -> OK.
            } else {
                // IDENT is already in symbol table
                error(34);  // Redeclaration error
            }
        } else {
            error(13); // Expected identifier
        }


        // Parse ':' (COLON)
        lookahead = nextsymbol();
        if (lookahead == COLON) {
            /* ':' (COLON) gefunden --> okay */
        } else if (lookahead != COLON) {
            error(3);   // Expected '=' after IDENT
        }


        // Parse TYP
        lookahead = nextsymbol();
        if (lookahead == INT) {
            st_entry *neu = insert(INTIDENT);
        } else if (lookahead == REAL) {
            st_entry *neu = insert(REALIDENT);
        } else {
            error(36);
        }

        // Read next symbol
        lookahead = nextsymbol();

        // Parse next variable separated with ',' (KOMMA)
    }while (lookahead == KOMMA);


    // Parse ';'
    if(lookahead == SEMICOLON) {
        /* ; (SEMICOLON) gefunden --> okay */
    }
    else {
        error(5);   // Missing SEMICOLON
    }

    lookahead = nextsymbol();

    return;    // ende vardecl

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
    st_entry *neu = insert(KONST);

    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Konstantendeklaration:";
    }

    // CONSTDECL starts with 'const'
    if(lookahead != CONST) {
        errortext("Found constant declaration and expected key word 'const'.");
    }


    // Parse identifier declaration at least once:
    //                  IDENT '=' INTNUMBER {',' IDENT '=' INTNUMBER } *
    do {

        // auf const muss IDENT folgen
        lookahead = nextsymbol();   // read IDENT
        if(lookahead == ID) {
            st_entry *found = lookup(idname);
            if(found == NULL) {
                // IDENT not in symtable
                neu->name = idname;
            }
            else {
                // IDENT is already in symbol table
                error(34);  // Redeclaration error
            }
        }
        else {
            error(13); // Expected identifier
        }


        // Parse '=' (EQ)
        lookahead = nextsymbol();
        if(lookahead == EQ) {
            /* '=' (EQ) gefunden --> okay */
        }
        else if (lookahead == ASS) {
            error(1);   // Error: Expected '=' instead of ':='
        }
        else if (lookahead != EQ) {
            error(3);   // Expected '=' after IDENT
        }


        // Parse NUMBER
        lookahead = nextsymbol();
        if (lookahead == INTNUM) {
            /* Int-Zahl (INTNUMBER) gefunden --> okay */
            neu->wertaddr = num;    // Assign inter value
        }
        else if (lookahead == REALNUM) {
            errortext("There are no real constants, only integer is allowed.");
        }
        else {
            // Neither INTNUM nor REALNUM
            error(2);   // expected integer constant after '='
        }


        // Read next symbol
        lookahead = nextsymbol();

    // Parse next constant separated with ',' (KOMMA)
    } while (lookahead == KOMMA);


    // Parse ';'
    if(lookahead == SEMICOLON) {
        /* ; (SEMICOLON) gefunden --> okay */
    }
    else {
        error(5);   // Missing SEMICOLON
    }

    // Insert into symtable
    insert(neu->token);

    // All right => Read next symbol
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


    if (tracesw)
    {
        trace << "\n Zeile:" << lineno << "Block";
    }

    // actsym auf neue Symboltabelle setzen
    symtable *oldSymboltable = actsym;
    actsym = neuSymboltable;

    // Parse BLOCK

    // Check if there is a CONSTDECL
    if(lookahead == CONST) {
        // There is at least one CONSTDECL (constant declaration)
        constdecl();
    }

    // Check if there is a VARDECL
    if(lookahead == VAR) {
        // There is at least one CONSTDECL (constant declaration)
        vardecl();
    }

    // Check if there is a PROCDECL
    if(lookahead == PROCEDURE) {
        procdecl();
    }

    // Parse a STATEMENT
    statement();


    // bei Blockende : Symboltabelle zurücksetzen
    // actsym = Zeiger auf vorherige Symboltabelle
    actsym = oldSymboltable;


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
        error(31);
    }

    // Dateiende erreicht ?
    if (lookahead != DONE)
    {
        error(33);
    } // noch Symbole in Eingabedatei nach RPOGRAM

}    // end program

