#include <ncurses.h>
#include <NetworkManager.h>

void addConnection(NMClient *client,const char *con_name);
void activateConnection(NMClient *client,NMConnection *connection);

static void added_connection(GObject *client,GAsyncResult *result,gpointer user_data);
static void activated_connection(GObject *client,GAsyncResult *result,gpointer user_data);

void show_connection (NMConnection *connection);

int main (int argc, char *argv[])
{
  NMClient *client;
  GError *error = NULL;
  const GPtrArray *connections;
  
  /* Connect to NetworkManager */
  client = nm_client_new (NULL, &error);
  if (!client) {
    g_message ("Error: Could not connect to NetworkManager: %s.", error->message);
    g_error_free (error);
    return 1;
  }
  /* Now the connections can be listed. */
  connections = nm_client_get_connections (client);

  printf ("Connections:\n===================\n");
  //i = 5
  for (int i = 0; i < connections->len; i++)
  { 
    show_connection (connections->pdata[i]);
  }
  
  //addConnection(NM_CLIENT(client),"Moussa");
  activateConnection(client,connections->pdata[4]);
  g_object_unref (client);

  return 0;
}

void activateConnection(NMClient *client,NMConnection *connection)
{
  NMDevice *device;
  NMAccessPoint *point;
  device = nm_client_get_device_by_iface(client,"wlp4s0");

  nm_client_activate_connection_async(client,connection,device, NULL,NULL,activated_connection,NULL);
}

static void activated_connection(GObject *client,GAsyncResult *result,gpointer user_data)
{
  NMActiveConnection *remote;
  GError *error = NULL;
    printf("hehexd?\n");
  remote = nm_client_activate_connection_finish (NM_CLIENT (client), result, &error);

  if (error) {
    g_print ("Error adding connection: %s", error->message);
    g_error_free (error);
  } else {
    g_print ("Connected: %s\n", nm_connection_get_path (NM_CONNECTION (remote)));
    g_object_unref (remote);
  } 
}
void addConnection(NMClient *client,const char *con_name)
{
  	
  NMConnection *connection;
  NMSettingWireless *s_wifi;
  NMSettingConnection *s_con;
  NMSettingIP4Config *s_ip4;
  char *uuid;
  
  connection = nm_simple_connection_new ();
  
  s_con = (NMSettingConnection *) nm_setting_connection_new ();
  uuid = nm_utils_uuid_generate ();
  g_object_set (G_OBJECT (s_con),
		NM_SETTING_CONNECTION_UUID, uuid,
		NM_SETTING_CONNECTION_ID, con_name,
		NM_SETTING_CONNECTION_TYPE, "802-11-wifi",
		NULL);
  g_free (uuid);
  nm_connection_add_setting (connection, NM_SETTING (s_con));
  
  s_wifi = (NMSettingWireless *) nm_setting_wireless_new();
  nm_connection_add_setting (connection,NM_SETTING(s_wifi));

  s_ip4 = (NMSettingIP4Config *) nm_setting_ip4_config_new ();
  g_object_set (G_OBJECT (s_ip4),
		"method", NM_SETTING_IP4_CONFIG_METHOD_AUTO,
		NULL);
  nm_connection_add_setting (connection, NM_SETTING (s_ip4));
  
  nm_client_add_connection_async (client, connection, TRUE, NULL,added_connection,NULL);
  g_object_unref (connection);
    
}

static void added_connection(GObject *client,GAsyncResult *result,gpointer user_data)
{
  NMRemoteConnection *remote;
  GError *error = NULL;

  remote = nm_client_add_connection_finish (NM_CLIENT (client), result, &error);

  if (error) {
    g_print ("Error adding connection: %s", error->message);
    g_error_free (error);
  } else {
    g_print ("Added: %s\n", nm_connection_get_path (NM_CONNECTION (remote)));
    g_object_unref (remote);
  }
  
}

void show_connection (NMConnection *connection)
{
  NMSettingConnection *s_con;
  guint64 timestamp;
  char *timestamp_str;
  char timestamp_real_str[64];
  const char *val1, *val2, *val3, *val4, *val5;

  s_con = nm_connection_get_setting_connection (connection);
  if (s_con) {
    /* Get various info from NMSettingConnection and show it */
    timestamp = nm_setting_connection_get_timestamp (s_con);
    timestamp_str = g_strdup_printf ("%" G_GUINT64_FORMAT, timestamp);
    strftime (timestamp_real_str, sizeof (timestamp_real_str), "%c", localtime ((time_t *) &timestamp));

    val1 = nm_setting_connection_get_id (s_con);
    val2 = nm_setting_connection_get_uuid (s_con);
    val3 = nm_setting_connection_get_connection_type (s_con);
    val4 = nm_connection_get_path (connection);
    val5 = timestamp ? timestamp_real_str : "never";

    printf ("%-25s | %s | %-15s | %-43s | %s\n", val1, val2, val3, val4, val5);

    g_free (timestamp_str);
  }
}
