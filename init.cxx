/******************************** init.cxx ************************/



/* Initialisierungen für Compiler PLA  

		Stand Sommersemester 2018
		

*/





#ifndef GLOBAL_H

#include <unistd.h>
#include "global.h"

#endif


ifstream fin;                    /* Zu übersetzende Eingaben */
ofstream fout,                   /* Ausgabedatei */
         ferr,                   /* Fehlerausgaben */
         fsym,                   /* Ausgabe für Symboltabelle */
         trace;                  /* Ausgabe für Trace */



int tracesw;
int level = 0;

/***************** Initialisierungen durchführen ******************/

void initialize()
{
    string filename = "/mnt/c/Users/Simon/Development/Compiler-A1/tests/lex_num.tst";

    // Input file
    //
    cout << "\n Name der Eingabedatei eingeben: ";
    getline(cin, filename);

    /* Eingabefile öffnen*/
    if( access(filename.c_str(), F_OK ) != -1 ) {
        // File exists
        fin.open(filename, ios::in);
    }
    else {
        // File does not exist
        cout << "\n   ERROR: Input file not found! \n";
        exit(-1);
    }

    // Output file
    //
    cout << "\n Name der Ausgabedatei eingeben (outputs.out): ";
    getline(cin, filename);
    if(filename.empty()) {
        filename = "outputs.out";       // Assign default value if filename not set.
    }

    /* Ausgabedatei öffnen */
    fout.open(filename, ios::out);
    fout << "\n\n***************** Ausgabe ***************\n";


    // Error file
    //
    cout << "\n Name der Fehlerdatei eingeben (errors.out): ";
    getline(cin, filename);
    if(filename.empty()) {
        filename = "errors.out";       // Assign default value if filename not set.
    }

    /* Fehlerdatei öffnen */
    ferr.open(filename, ios::out);
    ferr << "\n\n**************** Fehlermeldungen*************\n";


    // Enable trace
    //
    cout << "\n Trace gewünscht ? (y/N): ";
    string c = "n";
    getline(cin, c);
    if (c == "y")
    { /* Datei für Trace  öffnen */
        trace.open("trace.out", ios::out);
        tracesw = TRUE;

        cout << " Name der Tracedatei: trace.out \n";
    }
    else
    {
        tracesw = FALSE;
    }



    // Symbol table file
    //

    /* Datei für Symboltabellenausgabe öffnen */
    fsym.open("symtable.out", ios::out);
    fsym << "\n\n**************** Symboltabellen *************\n";

    cout << "\n Name der Symboltabellen-Datei: symtable.out \n";



    /* Initialisieren des Scanners */
    initlexan();


}



/************ Endebehandlung bei fehlerfreier Übersetzung  *****/
/* zur Zeit nur Ausgabe der Symboltabelle */


void stop()
{ /* Symboltabellen ausgeben  */

    printsymtab(firstsym);

}



 
