/*
wshtml.c - a program to convert a WordStar document to HTML

MIT License

Copyright (c) 2024 Jim Hall

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>

typedef unsigned char BYTE;
typedef enum { false, true } BOOL;

struct {
    BOOL bold;
    BOOL dblstrike;
    BOOL underline;
    BOOL superscr;
    BOOL subscr;
    BOOL strike;
    BOOL italic;
} fmt;

#define FLIP(A) ( (A) ? false : true )

int show_codes(BYTE ch, FILE *out)
{
    BYTE low;

    low = ch & 0x7f;

    switch (low) {                     /* ignore high bit */
        /* a few formatting codes */
    case 0x02:                        /* bold on/off */
        fmt.bold = FLIP(fmt.bold);
        if (fmt.bold) {
            fputs("<b>", out);
        }
        else {
            fputs("</b>", out);
        }
        break;
    case 0x04:                        /* double strike on/off */
        fmt.dblstrike = FLIP(fmt.dblstrike);
        if (fmt.dblstrike) {
            fputs("<bold>", out);
        }
        else {
            fputs("</bold>", out);
        }
        break;
    case 0x13:                        /* underline on/off */
        fmt.underline = FLIP(fmt.underline);
        if (fmt.underline) {
            fputs("<u>", out);
        }
        else {
            fputs("</u>", out);
        }
        break;
    case 0x14:                        /* superscript on/off */
        fmt.superscr = FLIP(fmt.superscr);
        if (fmt.superscr) {
            fputs("<sup>", out);
        }
        else {
            fputs("</sup>", out);
        }
        break;
    case 0x16:                        /* subscript on/off */
        fmt.subscr = FLIP(fmt.subscr);
        if (fmt.subscr) {
            fputs("<sub>", out);
        }
        else {
            fputs("</sub>", out);
        }
        break;
    case 0x18:                        /* strikethrough on/off */
        fmt.strike = FLIP(fmt.strike);
        if (fmt.strike) {
            fputs("<s>", out);
        }
        else {
            fputs("</s>", out);
        }
        break;
    case 0x19:                        /* italic on/off */
        fmt.italic = FLIP(fmt.italic);
        if (fmt.italic) {
            fputs("<i>", out);
        }
        else {
            fputs("</i>", out);
        }
        break;

        /* printable codes */

    case 0x09:                        /* tab */
        fputs("<span>&rarrb;</span>", out);
        break;
    case 0x0a:                        /* new line */
        fputs("<span>&ldsh;</span><br>", out);
        break;
    case 0x0c:                        /* page feed */
        fputs("<span>&dArr;</span>", out);
        break;
    case 0x0d:                        /* carr rtn */
        fputs("<span>&larrhk;</span>", out);
        break;
    case 0x1a:                        /* eof */
        fputs("<span>&squf;</span>", out);
        return -1;

    default:
        if (low < ' ') {               /* not printable */
            fprintf(out, "<span>0x%X</span>", low);
        }
        else {                         /* printable */
            fputc(low, out);
        }
    }

    return 0;
}

void show_file(FILE *in, FILE *out)
{
    BYTE str[100];
    size_t len, i;

    while (!feof(in)) {
        len = fread(str, sizeof(BYTE), 100, in);

        if (len > 0) {
            for (i = 0; i < len; i++) {
                if (show_codes(str[i], out) < 0) {
                    return;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    FILE *pfile;

    /* check command line */

    if (argc != 2) {
        fputs("usage: wshtml {file}\n", stderr);
        return 1;
    }

    /* init formatting */

    fmt.bold = false;
    fmt.dblstrike = false;
    fmt.underline = false;
    fmt.superscr = false;
    fmt.subscr = false;
    fmt.strike = false;
    fmt.italic = false;

    /* HTML start */

    puts("<!DOCTYPE html>");
    puts("<html><head><title>");
    puts(argv[1]);
    puts("</title><style>");
    puts("span{color:pink;}");
    /* puts("span{display:none;}"); */
    puts("</style></head><body>");

    /* process file */

    pfile = fopen(argv[1], "rb");

    if (pfile == NULL) {
        fputs("cannot open file: ", stderr);
        fputs(argv[1], stderr);
        fputc('\n', stderr);
    }
    else {
        show_file(pfile, stdout);
        fclose(pfile);
    }

    /* HTML end */

    puts("</body></html>");

    return 0;
}
