#include <stdio.h>

#define LONG    long
#define WORD    unsigned short

int read_name(
    FILE    *filehandle,
    char    *pch
) {
    char    length;
    int     rc;

    rc = fread( &length, sizeof(length), 1, filehandle );
    if ( rc != 1 ) {
        printf("Could not read name length\n");
        *pch = '\0';
        return( 0 );
    }
    rc = fread( pch, length, 1, filehandle );
    if ( rc != 1 ) {
        printf("Could not read name\n");
        *pch = '\0';
        return( 0 );
    }
    *(pch + length) = '\0';
    return( (int)length );
}

int read_symbol(
    FILE    *filehandle,
    char    *pch,
    LONG    *offset
) {
    int     rc;
    WORD    word;

    rc = fread( &word, sizeof(WORD), 1, filehandle );
    if ( rc != 1 ) {
        printf("Could not read symbol offset\n");
        *pch = '\0';
        *offset = 0L;
        return(0);
    }
    *offset = (LONG)word;
    rc = read_name( filehandle, pch );
    return( rc );
}

int main(
    int     argc,
    char    *argv[]
) {
    FILE    *symfile;
    LONG    filesize;
    LONG    start_position;
    LONG    position;
    WORD    w1;
    WORD    num_syms;
    WORD    w3;
    WORD    w4;
    WORD    next_off;
    WORD    index;
    char    c1;
    char    c2;
    int     rc;
    int     cnt;
    LONG    offset;
    WORD    r2;
    WORD    seg_num;
    char    b[12];

    char    name_buff[128];

    if ( argc == 3 ) {
        _asm {
            int 3
        }
    }

    symfile = fopen( argv[1], "rb" );

    if ( symfile == NULL ) {
        printf("Could not open sym file\n");
        exit(1);
    }

    rc = fread( &filesize, sizeof(filesize), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read file size\n");
        exit(1);
    }
    filesize <<= 4;

    rc = fread( &w1, sizeof(w1), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read w1\n");
        exit(1);
    }

    rc = fread( &num_syms, sizeof(num_syms), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read num_syms\n");
        exit(1);
    }
    rc = fread( &w3, sizeof(w3), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read w3\n");
        exit(1);
    }
    rc = fread( &w4, sizeof(w4), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read w4\n");
        exit(1);
    }
    rc = fread( &next_off, sizeof(next_off), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read next_off\n");
        exit(1);
    }
    start_position = ((LONG)next_off) << 4;

    rc = fread( &c1, sizeof(c1), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read c1\n");
        exit(1);
    }

    read_name( symfile, name_buff );
    printf("Symbols are for file: <%s>\n", name_buff );

    rc = fread( &c2, sizeof(c2), 1, symfile );
    if ( rc != 1 ) {
        printf("Could not read c2\n");
        exit(1);
    }

    cnt = num_syms;
    while ( cnt ) {
        read_symbol( symfile, name_buff, &offset );
        printf("Symbol: %08lX:<%s>\n", offset, name_buff );
        --cnt;
    }
#ifdef NEED_INDICES
    cnt = num_syms;
    while ( cnt ) {
        rc = fread( &index, sizeof(index), 1, symfile );
        if ( rc != 1 ) {
            printf("Could not read index table entry\n");
            exit(1);
        }
        printf("Index: %04X\n", index );
        --cnt;
    }
#endif

    position = start_position;
    do {
        rc = fseek( symfile, position, SEEK_SET );
        if ( rc != 0 ) {
            printf("Failed to seek to next record\n");
            exit(1);
        }
        rc = fread( &next_off, sizeof(next_off), 1, symfile );
        if ( rc != 1 ) {
            printf("Could not read next_off\n");
            exit(1);
        }
        position = ((LONG)next_off) << 4;

        rc = fread( &num_syms, sizeof(num_syms), 1, symfile );
        if ( rc != 1 ) {
            printf("Could not read num_syms\n");
            exit(1);
        }

        rc = fread( &r2, sizeof(r2), 1, symfile );
        if ( rc != 1 ) {
            printf("Could not read r2\n");
            exit(1);
        }

        rc = fread( &seg_num, sizeof(seg_num), 1, symfile );
        if ( rc != 1 ) {
            printf("Could not read seg_num\n");
            exit(1);
        }

        cnt = 0;
        while ( cnt < 12 ) {
            rc = fread( &b[cnt], sizeof(b[0]), 1, symfile );
            if ( rc != 1 ) {
                printf("Could not read 12 byte b array\n");
                exit(1);
            }
            cnt++;
        }
        read_name( symfile, name_buff );
        printf("Symbols are for file: <%s>\n", name_buff );

        cnt = num_syms;
        while ( cnt ) {
            read_symbol( symfile, name_buff, &offset );
            printf("Symbol: %04X:%08lX:<%s>\n", seg_num, offset, name_buff );
            --cnt;
        }
#ifdef NEED_INDICES
        cnt = num_syms;
        while ( cnt ) {
            rc = fread( &index, sizeof(index), 1, symfile );
            if ( rc != 1 ) {
                printf("Could not read index table entry\n");
                exit(1);
            }
            --cnt;
        }
#endif

    } while ( position != start_position && position != 0 );

    fclose( symfile );

}
