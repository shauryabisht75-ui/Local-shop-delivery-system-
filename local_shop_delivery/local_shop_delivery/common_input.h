// ============================================================
// common_input.h  -  safe input helpers used by every module
// Part of: Customer module (Raj Vardhan, 2510010483)
// Team ID: DSCPP-III-2026-T113
// ============================================================
#ifndef COMMON_INPUT_H
#define COMMON_INPUT_H

#include <iostream>
#include <string>
#include <cctype>    // isdigit, tolower
#include <cstdlib>   // exit
using namespace std;

// Reads a whole number between low and high. Keeps asking until valid.
inline int getNumber(string message, int low, int high)
{
    while (true)
    {
        cout << message;
        string s;
        if (!getline(cin, s))
        {
            cout << "\nInput ended. Bye!\n";
            exit(0);
        }
        try
        {
            if (s.empty() || s.size() > 6)
                throw 1;
            int x = 0;
            for (int i = 0; i < (int)s.size(); i++)
            {
                if (!isdigit((unsigned char)s[i]))
                    throw 1;
                x = x * 10 + (s[i] - '0');
            }
            if (x < low || x > high)
                throw 2;
            return x;
        }
        catch (int code)
        {
            if (code == 1)
                cout << "Please type digits only (a whole number).\n";
            else
                cout << "Number must be between " << low << " and " << high << ".\n";
        }
    }
}

// Reads a line of text (spaces allowed). It cannot be empty.
inline string getText(string message)
{
    string s;
    while (true)
    {
        cout << message;
        if (!getline(cin, s))
        {
            cout << "\nInput ended. Bye!\n";
            exit(0);
        }
        for (int i = 0; i < (int)s.size(); i++)
        {
            if (s[i] != ' ')
                return s;
        }
        cout << "It cannot be empty. Try again.\n";
    }
}

// Converts a string to lower case (used for searching product names)
inline string toLower(string s)
{
    for (int i = 0; i < (int)s.size(); i++)
        s[i] = tolower((unsigned char)s[i]);
    return s;
}

#endif
