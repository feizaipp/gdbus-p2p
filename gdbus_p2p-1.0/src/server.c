#include "gdbusp2p-generated.h"
#include <stdio.h>

static Gdbusp2pBase *skeleton = NULL;

static gboolean on_handle_test_int(Gdbusp2pBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gpointer user_data)
{
    g_printerr ("Method call:on_handle_test_int\n");
    gdbusp2p_base_complete_test_int(skeleton, invocation, 55);

    return TRUE;
}

static gboolean on_handle_test_structs(Gdbusp2pBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gpointer user_data)
{
    GVariantBuilder builder;
    GVariant *ret = NULL;

    g_printerr ("Method call:on_handle_test_structs\n");
    g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(si)"));
    g_variant_builder_add (&builder, "(si)", "test1", 1);
    g_variant_builder_add (&builder, "(si)", "test2", 2);
    g_variant_builder_add (&builder, "(si)", "test3", 3);
    ret = g_variant_builder_end (&builder);
    gdbusp2p_base_complete_test_structs(skeleton, invocation, ret);
    return TRUE;
}

static gboolean on_handle_test_str(Gdbusp2pBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gint index,
                                                    gpointer user_data)
{
    g_printerr ("Method call:on_handle_test_str\n");
    char *reply1 = "hello world";
    char *reply2 = "hello dbus";
    if (index == 1) {
        gdbusp2p_base_complete_test_str(skeleton, invocation, reply1);
    } else if (index == 2) {
        gdbusp2p_base_complete_test_str(skeleton, invocation, reply2);
    } else {
        gdbusp2p_base_complete_test_str(skeleton, invocation, "error");
    }

    return TRUE;
}

static gboolean
on_new_connection (GDBusServer *server,
                   GDBusConnection *connection,
                   gpointer user_data)
{
    GError *error = NULL;

    skeleton = gdbusp2p_base_skeleton_new ();
    g_signal_connect(skeleton, "handle-test-int", G_CALLBACK(on_handle_test_int), NULL);
    g_signal_connect(skeleton, "handle-test-str", G_CALLBACK(on_handle_test_str), NULL);
    g_signal_connect(skeleton, "handle-test-structs", G_CALLBACK(on_handle_test_structs), NULL);
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(skeleton), connection,
                                                        "/org/freedesktop/Gdbusp2p/Base", &error);

    if (error != NULL) {
        g_printerr("Error:failed to export object. Reason:%s\n", error->message);
        g_error_free(error);
        return;
    }
    g_printerr ("Connected to the system bus\n");

    return TRUE;
}

int main(int argc, char **argv)
{
    GDBusServer *server;
    gchar *guid;
    GMainLoop *loop;
    GDBusServerFlags server_flags;
    GError *error;
    gint ret;

    guid = g_dbus_generate_guid ();
    server_flags = G_DBUS_SERVER_FLAGS_NONE;
    error = NULL;
    server = g_dbus_server_new_sync ("unix:path=/tmp/gdbusp2p",  /* address */
                                       server_flags,                                           /* flags */
                                       guid,                                                       /* guid */
                                       NULL, /* GDBusAuthObserver */
                                       NULL, /* GCancellable */
                                       &error);
    if (server == NULL) {
        g_printerr ("Error creating server at address %s: %s\n", "unix:path=/tmp/gdbusp2p", error->message);
        g_error_free (error);
        goto out;
    }
    g_dbus_server_start (server);
    g_free (guid);
    g_print ("Server is listening at: %s\n", g_dbus_server_get_client_address (server));
    g_signal_connect (server,
                        "new-connection",
                        G_CALLBACK (on_new_connection),
                        NULL);
    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    g_object_unref (server);
    g_main_loop_unref (loop);
    ret = 0;

out:
    return ret;
}
