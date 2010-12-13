#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sample_parser.h"
#include "sample.h"

typedef struct ParseContext {
    double        *doubleOffset;
    unsigned int  *boolOffset;
    char          lastError[512];
    Sample        *sample;
} ParseContext;


static int reformat_boolean(void * ctx, int boolean)
{
    ParseContext *p  = (ParseContext *)ctx;
    if(p->boolOffset != NULL) {
        *(p->boolOffset) = boolean;
    } else {
        return 0;
    }
    return 1;
}

static int reformat_number(void * ctx, const char * s, unsigned int l)
{
    ParseContext *p  = (ParseContext *)ctx;
    if(p->doubleOffset != NULL) {
        *(p->doubleOffset) = atof(s);
    } else {
        return 0;
    }
    return 1;
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal,
                            unsigned int stringLen) {
    ParseContext *p = (ParseContext *)ctx;
    p->doubleOffset = NULL;
    p->boolOffset = NULL;

    if(strncmp((const char*)stringVal, "wholeLeafThickness", stringLen) == 0) {
        p->doubleOffset = &(p->sample->wholeLeafThickness);
    } else if(strncmp((const char *)stringVal, "cuticleUndulationsAspectRatio", stringLen) == 0) {
        p->doubleOffset = &(p->sample->cuticleUndulationsAspectRatio);
    } else if(strncmp((const char *)stringVal, "epidermisCellCapsAspectRatio", stringLen) == 0) {
        p->doubleOffset = &(p->sample->epidermisCellCapsAspectRatio);
    } else if(strncmp((const char *)stringVal, "spongyCellCapsAspectRatio", stringLen) == 0) {
        p->doubleOffset = &(p->sample->spongyCellCapsAspectRatio);
    } else if(strncmp((const char *)stringVal, "palisadeCellCapsAspectRatio", stringLen) == 0) {
        p->doubleOffset = &(p->sample->palisadeCellCapsAspectRatio);
    } else if(strncmp((const char *)stringVal, "proteinConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->proteinConcentration);
    } else if(strncmp((const char *)stringVal, "celluloseConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->celluloseConcentration);
    } else if(strncmp((const char *)stringVal, "linginConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->linginConcentration);
    } else if(strncmp((const char *)stringVal, "chlorophyllAConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->chlorophyllAConcentration);
    } else if(strncmp((const char *)stringVal, "chlorophyllBConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->chlorophyllBConcentration);
    } else if(strncmp((const char *)stringVal, "carotenoidConcentration", stringLen) == 0) {
        p->doubleOffset = &(p->sample->carotenoidConcentration);
    } else if(strncmp((const char *)stringVal, "mesophyllFraction", stringLen) == 0) {
        p->doubleOffset = &(p->sample->mesophyllFraction);
    } else if(strncmp((const char *)stringVal, "bifacial", stringLen) == 0) {
        p->boolOffset = &(p->sample->bifacial);
    } else {
        char tmp[512];
        strncpy(tmp, (const char *)stringVal, stringLen < 512 ? stringLen : 512);
        sprintf(p->lastError, "Unknown key: '%s'", tmp);
    }


    return 1;
}

static yajl_callbacks callbacks = {
    NULL, // NULL
    reformat_boolean, //BOOL
    NULL,
    NULL,
    reformat_number, //NUMBER
    NULL, //STRING
    NULL, //START_MAP
    reformat_map_key, //MAP_KEY
    NULL, //END_MAP
    NULL, //START_ARRAY
    NULL  //END_ARRAY
};


#define READ_BUFFER_SIZE 65536
int parseSampleFromFile(Sample *sample, FILE *inputFile) {
    yajl_handle hand;
    static unsigned char fileData[READ_BUFFER_SIZE];
    ParseContext g;
    g.sample = sample;

    yajl_status stat;
    size_t rd;
    /* allow comments */
    yajl_parser_config cfg = { 1, 1 };
    int retval = 1, done = 0;

    /* ok.  open file.  let's read and parse */
    hand = yajl_alloc(&callbacks, &cfg, NULL, (void *) &g);
        
    while (!done) {
        rd = fread((void *) fileData, 1, sizeof(fileData) - 1, inputFile);
        
        if (rd == 0) {
            if (!feof(inputFile)) {
                fprintf(stderr, "error on file read.\n");
                retval = 0;
                break;
            }
            done = 1;
        }
        fileData[rd] = 0;
        
        if (done)
            /* parse any remaining buffered data */
            stat = yajl_parse_complete(hand);
        else
            /* read file data, pass to parser */
            stat = yajl_parse(hand, fileData, rd);

        if (stat != yajl_status_ok &&
            stat != yajl_status_insufficient_data)
        {
            unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
            fprintf(stderr, "%s", (const char *) str);
            yajl_free_error(hand, str);
            retval = 0;
            if( stat == yajl_status_client_canceled) {
                fprintf(stderr, "%s\n", g.lastError);
            }
            break;
        }
    }

    yajl_free(hand);
    
    return retval;
}
