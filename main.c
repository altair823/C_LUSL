#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "argparse.h"

#include "serializer.h"
#include "deserializer.h"

static const char *const usages[] = {
    "basic [options] [[--] args]",
    "basic [options]",
    NULL,
};

int main(int argc, const char **argv) {
    const char *op;
    bool ser_flag = false;
    bool deser_flag = false;
    char *origin = NULL;
    char *target = NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Serializer options"),
        OPT_STRING('s', "operation", &op, "Operation type", NULL, 0, OPT_POSITIONAL),
        OPT_STRING('f', "from", &origin, "Path to the input file", NULL, 0, OPT_POSITIONAL),
        OPT_STRING('t', "to", &target, "Path to the output file", NULL, 0, OPT_POSITIONAL),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
    argc = argparse_parse(&argparse, argc, argv);
    if (strcmp(op, "s") == 0) {
        ser_flag = true;
        INIT_SERIALIZER(serializer, origin, target);
        SET_OPTION(serializer, 0);
        if (!serialize(&serializer)) {
            printf("Error while serializing\n");
            return 1;
        } else {
            printf("Serialization completed successfully\n");
        }
        FREE_SERIALIZER(serializer);
    } else if (strcmp(op, "d") == 0) {
        deser_flag = true;
        INIT_DESERIALIZER(deserializer, origin, target);
        if (!deserialize(&deserializer)) {
            printf("Error while deserializing\n");
            return 1;
        } else {
            printf("Deserialization completed successfully\n");
        }
        FREE_DESERIALIZER(deserializer);
    } else {
        printf("Invalid operation type: %s\n", op);
        return 1;
    }
    return 0;
}