#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "gdbusp2p-generated.h"


static gboolean opt_vi = FALSE;
static gboolean opt_vs = FALSE;
static gboolean opt_ss = FALSE;
static GOptionEntry opt_entries[] =
{
    {"voidint", 'i', 0, G_OPTION_ARG_NONE, &opt_vi, "no input, return int", NULL},
    {"voidstring", 's', 0, G_OPTION_ARG_NONE, &opt_vs, "no input, return string", NULL},
    {"voidstructs", 'S', 0, G_OPTION_ARG_NONE, &opt_ss, "no input, return structs", NULL},
    {NULL }
};

struct test_structs {
    char *ret_str;
    int ret_int;
};

int main (int argc, char **argv)
{
    GDBusConnection *connection;
    const gchar *greeting_response;
    GVariant *value;
    gchar *greeting;
    GError *error;
    gint ret;
    gint ret_int;
    GOptionContext *opt_context = NULL;

    ret = 1;
    opt_context = g_option_context_new ("test gdbus p2p");
    g_option_context_add_main_entries (opt_context, opt_entries, NULL);
    error = NULL;
    if (!g_option_context_parse (opt_context, &argc, &argv, &error))
    {
        g_printerr ("Error parsing options: %s\n", error->message);
        g_error_free (error);
        goto out;
    }

    error = NULL;
    connection = g_dbus_connection_new_for_address_sync ("unix:path=/tmp/gdbusp2p",
                                                        G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                                        NULL, /* GDBusAuthObserver */
                                                        NULL, /* GCancellable */
                                                        &error);
    if (connection == NULL) {
        g_printerr ("Error connecting to D-Bus address %s: %s\n", "unix:path=/tmp/gdbusp2p", error->message);
        g_error_free (error);
        goto out;
    }
    g_print ("Connected.\n"
               "Negotiated capabilities: unix-fd-passing=%d\n",
               g_dbus_connection_get_capabilities (connection) & G_DBUS_CAPABILITY_FLAGS_UNIX_FD_PASSING);
    if (opt_vi) {
        value = g_dbus_connection_call_sync (connection,
                                           "org.freedesktop.Gdbusp2p", /* bus_name */
                                           "/org/freedesktop/Gdbusp2p/Base", /* object path */
                                           "org.freedesktop.Gdbusp2p.Base", /* interface name */
                                           "TestInt",                       /* method name */
                                           NULL,                           /* parameters */
                                           G_VARIANT_TYPE ("(i)"), /* reply type */
                                           G_DBUS_CALL_FLAGS_NONE,  /* flag */
                                           -1,                                         /* timeout msec */
                                           NULL,                                    /* cancellable */
                                           &error);                                  /* error */
        if (value == NULL) {
            g_printerr ("Error invoking TestInt(): %s\n", error->message);
            g_error_free (error);
            goto out;
        }
        g_variant_get (value, "(i)", &ret_int);
        g_print ("Server return : %d\n", ret_int);
        g_variant_unref (value);
        g_object_unref (connection);
    }

    if (opt_vs) {
        int index = 0;
        gchar *response;

        sscanf(argv[1], "%d", &index);
        value = g_dbus_connection_call_sync (connection,
                                           "org.freedesktop.Gdbusp2p", /* bus_name */
                                           "/org/freedesktop/Gdbusp2p/Base", /* object path */
                                           "org.freedesktop.Gdbusp2p.Base", /* interface name */
                                           "TestStr",                       /* method name */
                                           g_variant_new("(i)", index),                           /* parameters */
                                           G_VARIANT_TYPE ("(s)"), /* reply type */
                                           G_DBUS_CALL_FLAGS_NONE,  /* flag */
                                           -1,                                         /* timeout msec */
                                           NULL,                                    /* cancellable */
                                           &error);                                  /* error */
        if (value == NULL) {
            g_printerr ("Error invoking TestStr(): %s\n", error->message);
            g_error_free (error);
            goto out;
        }
        g_variant_get (value, "(s)", &response);
        g_print ("Server return : %s\n", response);
        g_variant_unref (value);
        g_object_unref (connection);
    }

    if (opt_ss) {
        //GVariantIter *iter;
        GVariantIter iter;
        gchar *ret_str;
        gint index;

        value = g_dbus_connection_call_sync (connection,
                                           "org.freedesktop.Gdbusp2p", /* bus_name */
                                           "/org/freedesktop/Gdbusp2p/Base", /* object path */
                                           "org.freedesktop.Gdbusp2p.Base", /* interface name */
                                           "TestStructs",                       /* method name */
                                           NULL,                           /* parameters */
                                           G_VARIANT_TYPE ("a(si)"), /* reply type */
                                           G_DBUS_CALL_FLAGS_NONE,  /* flag */
                                           -1,                                         /* timeout msec */
                                           NULL,                                    /* cancellable */
                                           &error);                                  /* error */
        if (value == NULL) {
            g_printerr ("Error invoking TestStructs(): %s\n", error->message);
            g_error_free (error);
            goto out;
        }
        /*
        g_printerr("iter\n");
        g_variant_iter_init(&iter, value);
        while (g_variant_iter_next(&iter, "(si)", &ret_str, &ret_int)) {
            g_printerr("sync reply:%s\n", ret_str);
            g_printerr("sync reply:%d\n", ret_int);
            g_free(ret_str);
        }
        */
/*
        g_variant_get (value, "(&si)", &iter);
        while (g_variant_iter_loop (iter, "si", &str, &index)) {
            g_print ("Server return : %s %d\n", str, index);
            g_free(str);
        }
        g_variant_iter_free (iter);
*/
        g_variant_unref (value);
        g_object_unref (connection);
    }

    ret = 0;
out:
    if (opt_context != NULL)
        g_option_context_free (opt_context);
    return ret;
}
