/************************ lexan.cxx  Sommersemester 2018 ********************/
/*************** Scanner *******************************/


#ifndef GLOBAL_H

#include "global.h"

#endif


#define NORW     14                 /* Zahl der reservierten Worte */

#define RWSYMLEN 15                 /* Max. Länge reservierter Symbole */


int lineno;                         /* Zeilennummer */

int num;                            /* Wert einer int-Konstanten  */
double realnum;                     /* Wert einer real-Konstanten*/

char idname[BSIZE];                 /* Name eines Bezeichners ; wird vom Parser weiterverwendet */


char actchar;                       /* gelesenes Zeichen */


/* Register functions */
int readNumber();
int readIdentifcator();
bool isValidNumberTerminal(char c);
bool isNumberTerminator(char c);
void readNextCharacter();

/*
Um Bezeichner von reservierten Symbolene unterscheiden zu können,
wird eine Tabelle reservierte Worte verwendet (restable). 

Die Tabelle enthält Einträge für jedes Schlüsselwort, bestehend aus 
dem Schlüsselwort selbst und dem zugehörigen Tokentyp (Codierung vgl. global.h):

Bei Erkennen eines möglichen Bezeichners wird zuerst die Tabelle
der reservierten Symbole durchsucht (lookforres); 
wird ein Schlüsselwort gefunden, liefert lookforres den dem Schlüsselwort 
zugeordneten Tokentyp; sonst 0. Bei Ergebnis 0 liegt dann tatsächlich ein
Bezeichner vor. 
*/

/**
 * Struktur eines Eintrags in der Tabelle reservierter Symbole
 */
struct ressw
{
    char ressymbol[RWSYMLEN];       /* Symbol */
    int token;                      /* zugehöriger Tokentyp */
};


/**
 * Tabelle reservierter Worte
 */
struct ressw restable[] =
{
    "const", CONST,
    "var", VAR,
    "procedure", PROCEDURE,
    "call", CALL,
    "begin", BEGIN,
    "end", END,
    "if", IF,
    "then", THEN,
    "else", ELSE,
    "while", WHILE,
    "do", DO,
    "int", INT,
    "real", REAL,
    "fi", FI
};


/**
 * Suchen nach einem reservierten Symbol
 *
 * @return Sucht in Tabelle reservierter Worte nach s und liefert zugehörigen Token, sonst 0
 */
int lookforres(char *s)
{
    struct ressw *ptr;

    for (ptr = restable; ptr < &restable[NORW]; ptr++)
    {

        if (strcmp(ptr->ressymbol, s) == 0)
        {
            /* Symbol gefunden */
            return (ptr->token);
        }
    }

    return (0);                /* Symbol nicht gefunden */
}



// Custom definitions
//
const char REAL_DELIMITER = '.'; // Use this character to indicate the fraction: 123456<REAL_DELIMITER>456 => 123456.789




/******************* Initialisieren des Scanners **********************/

/**
 * Initialisieren des Scanners
 * Wird aufgerufen von  initialize () aus init.cxx ;
 * nimmt Vorbesetzungen der Variablen num, realnum,  idname und lineno vor;
 *
 * liest das erste Zeichen aus der Eingabe
 */
void initlexan()
{
    num = NONE;
    realnum = 0.0;
    idname[0] = '\0';
    lineno = 1;
    readNextCharacter();            /* Erstes Zeichen der Eingabe lesen */

}






/*******************************************************************************/
/********** Funktion nextsymbol ************************************************/
/*******************************************************************************/


/**** zentrale Funktion zum Lesen des nächsten lexikalischen Symbols ***************/
/*****  identifiziert nächstes lexikalisches Symbol der Eingabe **********/

/***** WICHTIG!!!!!   
		Bei Aufruf von nextsymbol muss  sich das nächste Eingabezeichen bereits in actchar befinden

*******/


/**    liefert Codierung des nächsten Symbols (token): 

		- Konstante:				token == INTNUM und Wert der Konstanten in Variable num
									token == REALNUM und Wert in realnum
		- Bezeichner:				token == ID  und Zeiger auf Name in idname 
		- Schlüsselwort:			token == Tokentyp des reservierten Symbols nach 
									Suche in restable 
		- Operatoren,Sonderzeichen :entsprechende Token 

**/


/** 
 * zentrale Funktion zum Lesen des nächsten lexikalischen Symbols
 * identifiziert nächstes lexikalisches Symbol der Eingabe
 * 
 * <b>WICHTIG!!!!!</b>
 *         Bei Aufruf von nextsymbol muss  sich das nächste Eingabezeichen bereits in actchar befinden
 *         
 *         
 * @return liefert Codierung des nächsten Symbols (token): 
 * 
 * <ul>
 *      <li> <b>Konstante:</b>                  <code>token == INTNUM</code> und Wert der Konstanten in Variable num <br/>
 *                                              <code>token == REALNUM</code> und Wert in realnum <br/></li>
 *      <li> <b>Bezeichner:</b>                 <code>token == ID</code> und Zeiger auf Name in idname <br/></li>
 *      <li> <b>Schlüsselwort:</b>              <code>token == Tokentyp</code> des reservierten Symbols nach Suche in restable</li>
 *      <li> <b>Operatoren,Sonderzeichen:</b>   entsprechende Token  <br/></li>
 * </ul>

 */
int nextsymbol()
{
    int token;
    char lexbuf[BSIZE];             /* lokaler Puffer für Eingabezeichen */



    while (!fin.eof())              /* Eingabe-Dateiende nicht erreicht */
    {

        if (actchar == ' ' || actchar == '\t')
        {
            /* Blank und Tab in Ausgabedatei kopieren und überlesen */
            fout.put(actchar);
            readNextCharacter();

        }


        else if (actchar == '\n' || actchar == '\r')
        {
            /* Newline in Ausgabedatei kopieren, überlesen/entfernen, Zeilennummer erhöhen */
            fout.put(actchar);
            readNextCharacter();
            lineno++;
        }


        else if (isdigit(actchar))
        {
            /***** actchar ist Ziffer --> Konstanten erkennen  *****/

            return readNumber();

        }


        else if (isalpha(actchar))
        {
            /***** actchar ist Buchstabe -->  Identifikatoren erkennen ****/



            int b = 0;                  /* Zeichenzahl */

            /* reg. Ausdruck   letter (letter|digit)*  erkennen ==>
                solange Buchstaben oder Ziffern folgen --> Identifikator */

            return readIdentifcator();
        }

        /***** Sonderzeichen oder Operatoren erkennen ***************/


        else
        {
            fout.put(actchar);          /* Zeichen in Ausgabedatei */

            switch (actchar)
            {
                case '=':
                    readNextCharacter();
                    return (EQ);


                default:
                    error(32);


            } /* end-switch */
        } /* end-else */


    }/* end while */


    return (DONE);    /* Eingabe-Ende erreicht */

}

int readNumber() {

    /*
     * NUMBER     ::= INTNUMBER | REALNUMBER
     * INTNUMBER  ::= DIGIT {DIGIT}*
     * REALNUMBER ::= DIGIT {DIGIT}* ‘.‘ DIGIT {DIGIT}*
     */

    string number = "";

    bool isReal = false;
    bool hasFractional = false;
    while (isValidNumberTerminal(actchar))
    {
        /* Write current character to output file */
        fout.put(actchar);

        if(isdigit(actchar)) {
            // Append digit to number
            number += actchar;

            // Check if number is real
            if(isReal) {
                // Has (at least) one digit after delimiter
                hasFractional = true;
            }
        }
        else if(actchar == REAL_DELIMITER) {
            // Switch INT to REAL
            isReal = true;

            // Append delimiter to number
            number += actchar;
        }
        else if(isNumberTerminator(actchar)){
            // Number was completely read -> parse

            if(!isReal) {
                // Convert to int
                num = atoi(number.c_str());
                return INTNUM;
            }
            else {
                // Error handling: Check if number has at least one fractional
                if(!hasFractional) {
                    errortext("Fehlerhafte Gleitkommazahl: Ziffer nach <REAL_DELIMITER> fehlt.");
                }

                // Convert to real
                realnum = atof(number.c_str());
                return REALNUM;
            }
        }
        else {
            if(isalpha(actchar)) {
                errortext("Characters in Numbers not allowed! Did you mean an identifier? Identifiers must not start with a digit.");
            }

            // TODO Maybe handle character as identifier?
            error(32);    /* 32 = " unzulässiges Eingabezeichen (Scanner)" */

        }

        // Read next
        readNextCharacter();
    }
}


int readIdentifcator() {
    return NULL;
}


/**
 * Is the character valid in a number
 */
bool isValidNumberTerminal(char c) {
    return isdigit(c) || c == REAL_DELIMITER;
}

/**
 * Valid end of a number.
 */
bool isNumberTerminator(char c) {
    // TODO implement me!
    return true;
}

/**
 * Read the next character of the program and store it to actchar.
 */
void readNextCharacter()
{
    fin.get(actchar);
}

