#ifndef MESSAGE_JSON_PRINTER_H
#define MESSAGE_JSON_PRINTER_H

/* Generated by flatcc 0.6.1-dev FlatBuffers schema compiler for C by dvide.com */

#include "flatcc/flatcc_json_printer.h"
#ifndef EVENT_JSON_PRINTER_H
#include "event_json_printer.h"
#endif
#include "flatcc/flatcc_prologue.h"

/*
 * Prints the default root table or struct from a buffer which must have
 * the schema declared file identifier, if any. It is also possible to
 * call the type specific `print_json_as_root` function wich accepts an
 * optional identifier (or 0) as argument. The printer `ctx` object must
 * be initialized with the appropriate output type, or it can be 0 which
 * defaults to stdout. NOTE: `ctx` is not generally allowed to be null, only
 * here for a simplified interface.
 */
static int message_print_json(flatcc_json_printer_t *ctx, const char *buf, size_t bufsiz);

static void AzureIoTSecurity_Message_print_json_table(flatcc_json_printer_t *ctx, flatcc_json_printer_table_descriptor_t *td);

static void AzureIoTSecurity_Message_print_json_table(flatcc_json_printer_t *ctx, flatcc_json_printer_table_descriptor_t *td)
{
    flatcc_json_printer_string_field(ctx, td, 0, "security_module_id", 18);
    flatcc_json_printer_uint32_field(ctx, td, 1, "security_module_version", 23, 0);
    flatcc_json_printer_int8_field(ctx, td, 2, "timezone", 8, 0);
    flatcc_json_printer_table_vector_field(ctx, td, 3, "events", 6, AzureIoTSecurity_Event_print_json_table);
}

static inline int AzureIoTSecurity_Message_print_json_as_root(flatcc_json_printer_t *ctx, const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_json_printer_table_as_root(ctx, buf, bufsiz, fid, AzureIoTSecurity_Message_print_json_table);
}

static int message_print_json(flatcc_json_printer_t *ctx, const char *buf, size_t bufsiz)
{
    flatcc_json_printer_t printer;

    if (ctx == 0) {
        ctx = &printer;
        flatcc_json_printer_init(ctx, 0);
    }
    return AzureIoTSecurity_Message_print_json_as_root(ctx, buf, bufsiz, 0);}

#include "flatcc/flatcc_epilogue.h"
#endif /* MESSAGE_JSON_PRINTER_H */
