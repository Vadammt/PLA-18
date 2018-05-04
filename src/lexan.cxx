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
void readNextCharacter();
int readNumber();
int readIdentifcator();

// Checkers
bool isValidNumberTerminal(char c);
bool isValidIdentifierTerminal(char c);
bool isLineBreak(char c);
bool isWhiteSpace(char c);

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
    "const",     CONST,
    "var",       VAR,
    "procedure", PROCEDURE,
    "call",      CALL,
    "begin",     BEGIN,
    "end",       END,
    "if",        IF,
    "then",      THEN,
    "else",      ELSE,
    "while",     WHILE,
    "do",        DO,
    "int",       INT,
    "real",      REAL,
    "fi",        FI
};


/**
 * Suchen nach einem reservierten Symbol
 *
 * @return Sucht in Tabelle reservierter Worte nach s und liefert zugehörigen Token, sonst 0
 */
int lookforres(const string &s)
{
    struct ressw *ptr;

    for (ptr = restable; ptr < &restable[NORW]; ptr++)
    {

        if (strcmp(ptr->ressymbol, s.c_str()) == 0)
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
    fin.get(actchar);            /* Erstes Zeichen der Eingabe lesen */

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

        if (isWhiteSpace(actchar))
        {
            /* Blank und Tab in Ausgabedatei kopieren und überlesen */
            readNextCharacter();

        }


        else if (isLineBreak(actchar))
        {
            /* Newline in Ausgabedatei kopieren, überlesen/entfernen, Zeilennummer erhöhen */
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

            return readIdentifcator();
        }

        else
        {
            /***** Sonderzeichen oder Operatoren erkennen ***************/

            switch (actchar)
            {

                case '=':
                    readNextCharacter();
                    return (EQ);

                case '!':
                    readNextCharacter();
                    if(actchar == '=') {
                        readNextCharacter();
                        return (NE);
                    }
                    else {
                        errortext(const_cast<char *>("Inverting character '!' not defined; did you mean unequal '!=' ?"));
                        return -1;
                    }

                case '<':
                    readNextCharacter();
                    if(actchar == '=') {
                        readNextCharacter();
                        return(LE);
                    }
                    return (LT);

                case '>':
                    readNextCharacter();
                    if(actchar == '=') {
                        readNextCharacter();
                        return(GE);
                    }
                    return (GT);

                case ':':
                    readNextCharacter();
                    if(actchar == '=') {
                        readNextCharacter();
                        return (ASS);
                    }
                    return COLON;

                case ',':
                    readNextCharacter();
                    return (KOMMA);

                case ';':
                    readNextCharacter();
                    return (SEMICOLON);

                case '+':
                    readNextCharacter();
                    return (PLUS);

                case '-':
                    readNextCharacter();
                    return (MINUS);

                case '*':
                    readNextCharacter();
                    return (MULT);

                case '/':
                    readNextCharacter();
                    return (DIV);

                case '(':
                    readNextCharacter();
                    return (KLAUF);

                case ')':
                    readNextCharacter();
                    return (KLZU);

                case '$':
                    readNextCharacter();
                    return (PROGEND);

                default:
                    // Cannot read character
                    error(32);


            }
        }


    }/* end while */


    return (DONE);    /* Eingabe-Ende erreicht */

}



/**
 *
 * Read a NUMBER
 *
 */
int readNumber() {

    /*
     * NUMBER     ::= INTNUMBER | REALNUMBER
     * INTNUMBER  ::= DIGIT {DIGIT}*
     * REALNUMBER ::= DIGIT {DIGIT}* ‘.‘ DIGIT {DIGIT}*
     */

    string number = "";

    int numIntDigits = 0;
    bool isReal = false;
    int numRealDigits = 0;
    while (isValidNumberTerminal(actchar)) {

        if (isdigit(actchar)) {
            // Append digit to number
            number += actchar;

            // Check if number is real
            if (!isReal) {
                // Integer number -> increment int digits.
                numIntDigits++;
            } else {
                // Real number -> increment floating digits.
                numRealDigits++;
            }
        } else if (actchar == REAL_DELIMITER) {
            if(isReal && numRealDigits == 0) {
                errortext("Double dots -> Number has no fractional digit.");
            }

            // Switch INT to REAL
            isReal = true;

            // Append delimiter to number
            number += actchar;
        }

        // Read next character into actchar
        readNextCharacter();
    }

    // Error detection
    if(isalpha(actchar)) {
        errortext(const_cast<char *>("Characters in Numbers not allowed! Identifiers must start with a letter."));
    }

    // Interpret read number
    if(!isReal) {
        // integer number

        // Convert to int
        num = atoi(number.c_str());
        return INTNUM;
    }
    else {
        // real number

        // Error handling: Check if number has at least one fractional
        if(numRealDigits == 0) {
            errortext(const_cast<char *>("Incorrect floating number: Digit after <REAL_DELIMITER> missing."));
        }

        // Convert to real
        realnum = atof(number.c_str());
        return REALNUM;
    }
}


/**
 *
 * Rad an IDENT
 *
 */
int readIdentifcator() {
    /*
     * reg. Ausdruck   letter (letter|digit)*  erkennen ==>
     * solange Buchstaben oder Ziffern folgen --> Identifikator
     */

    string identifier = "";

    while (isValidIdentifierTerminal(actchar)) {
        // append new character to identifier
        identifier += actchar;

        // Read next character into actchar
        readNextCharacter();
    }

    // Interpret read identifier

    // Error detection: Is identfier length > BSIZE
    static const char* maxLengthError = ("Identifier length exceeds max size of " + to_string(BSIZE) + ".").c_str();
    if(identifier.length() > BSIZE) {
        errortext(const_cast<char *>(maxLengthError));
    }

    // Is identifier reserved
    int reservedIdent = lookforres(identifier);
    if(reservedIdent != 0) {
        // Identifier is reserved
        strcpy(idname, identifier.c_str());
        return reservedIdent;
    }
    else {
        // Identifier is a valid IDENT
        strcpy(idname, identifier.c_str());
        return ID;
    }
}

/**
 * Read the next character of the program and store it to actchar.
 */
void readNextCharacter()
{
    /* Write current character to output file */
    fout.put(actchar);

    /* Read next character */
    fin.get(actchar);
}


/**
 * Is the character valid in a number
 */
bool isValidNumberTerminal(char c) {
    return isdigit(c)
        || c == REAL_DELIMITER;
}

/**
 * Is the character valid in an identifier
 */
bool isValidIdentifierTerminal(char c) {
    return isdigit(c)
        || isalpha(c);
}

bool isLineBreak(char c) {
    return c == '\n'
        || c == '\r';
}

bool isWhiteSpace(char c) {
    return isspace(c);
}